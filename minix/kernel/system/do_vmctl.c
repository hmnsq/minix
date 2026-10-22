/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#شامل "kernel/system.h"
#شامل "kernel/vm.h"
#شامل <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
صحیح do_vmctl(ساختار proc * caller, message * m_ptr)
{
  صحیح proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ساختار proc *p, *rp, **rpp, *target;

  اگر(ep == SELF) { ep = caller->p_endpoint; }

  اگر(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	بازگشت EINVAL;
  }

  p = proc_addr(proc_nr);

  انتخاب(m_ptr->SVMCTL_PARAM) {
	حالت VMCTL_CLEAR_PAGEFAULT:
		أكد(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		بازگشت OK;
	حالت VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		برای (rpp = &vmrequest; *rpp != پوچ;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			أكد(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			اگر (!allow_ipc_filtered_memreq(rp, target))
				ادامه;

			/* Reply with request fields. */
			اگر (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_TARGET	=
				rp->p_vmrequest.target;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.start;
			m_ptr->SVMCTL_MRG_LENGTH	=
				rp->p_vmrequest.params.check.length;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(تهی *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			بازگشت rp->p_vmrequest.req_type;
		}

		بازگشت ENOENT;

	حالت VMCTL_MEMREQ_REPLY:
		أكد(RTS_ISSET(p, RTS_VMREQUEST));
		أكد(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_مقدار;
		أكد(p->p_vmrequest.vmresult != VMSUSPEND);

		انتخاب(p->p_vmrequest.type) {
		حالت VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			بشکن;
		حالت VMSTYPE_DELIVERMSG:
			أكد(p->p_misc_flags & MF_DELIVERMSG);
			أكد(p == target);
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			بشکن;
		حالت VMSTYPE_MAP:
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			بشکن;
		پیشفرض:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		بازگشت OK;

	حالت VMCTL_KERN_PHYSMAP:
	{
		صحیح i = m_ptr->SVMCTL_مقدار;
		بازگشت arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	حالت VMCTL_KERN_MAP_REPLY:
	{
		بازگشت arch_phys_map_reply(m_ptr->SVMCTL_مقدار,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	حالت VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#اگر CONFIG_SMP
		اگر (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} وگرنه
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#اگر CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		بازگشت OK;
	حالت VMCTL_VMINHIBIT_CLEAR:
		أكد(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		اگر (p->p_misc_flags & MF_SENDA_VM_MISS) {
			ساختار priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_MISS;
			privp = priv(p);
			try_deliver_senda(p, (asynmsg_t *) privp->s_asyntab,
							privp->s_asynsize);
		}
		/*
		 * We don't know whether kernel has the changed mapping
		 * installed to access userspace memory. And if so, on what CPU.
		 * More over we don't know what mapping has changed and how and
		 * therefore we must invalidate all mappings we have anywhere.
		 * Next time we map memory, we map it fresh.
		 */
		bits_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#endif
		بازگشت OK;
	حالت VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		بازگشت OK;
	حالت VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		بازگشت OK;
  }

  /* Try architecture-specific vmctls. */
  بازگشت arch_do_vmctl(m_ptr, p);
}
