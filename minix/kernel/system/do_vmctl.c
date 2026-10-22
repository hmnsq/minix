/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ "kernel/vm.h"
#அந்தர்பாவ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
முழு do_vmctl(கட்டமைப்பு proc * caller, message * m_ptr)
{
  முழு proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  கட்டமைப்பு proc *p, *rp, **rpp, *target;

  எனில்(ep == SELF) { ep = caller->p_endpoint; }

  எனில்(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	திரும்பு EINVAL;
  }

  p = proc_addr(proc_nr);

  தேர்வு(m_ptr->SVMCTL_PARAM) {
	நிலை VMCTL_CLEAR_PAGEFAULT:
		ப்ரதிஜ்ஞா(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		திரும்பு OK;
	நிலை VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		க்கு (rpp = &vmrequest; *rpp != வெற்று;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ப்ரதிஜ்ஞா(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			எனில் (!allow_ipc_filtered_memreq(rp, target))
				தொடர்;

			/* Reply with request fields. */
			எனில் (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(வெறுமை *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			திரும்பு rp->p_vmrequest.req_type;
		}

		திரும்பு ENOENT;

	நிலை VMCTL_MEMREQ_REPLY:
		ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_VMREQUEST));
		ப்ரதிஜ்ஞா(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_மதிப்பு;
		ப்ரதிஜ்ஞா(p->p_vmrequest.vmresult != VMSUSPEND);

		தேர்வு(p->p_vmrequest.type) {
		நிலை VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			நிறுத்து;
		நிலை VMSTYPE_DELIVERMSG:
			ப்ரதிஜ்ஞா(p->p_misc_flags & MF_DELIVERMSG);
			ப்ரதிஜ்ஞா(p == target);
			ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_VMREQUEST));
			நிறுத்து;
		நிலை VMSTYPE_MAP:
			ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_VMREQUEST));
			நிறுத்து;
		இயல்பு:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		திரும்பு OK;

	நிலை VMCTL_KERN_PHYSMAP:
	{
		முழு i = m_ptr->SVMCTL_மதிப்பு;
		திரும்பு arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	நிலை VMCTL_KERN_MAP_REPLY:
	{
		திரும்பு arch_phys_map_reply(m_ptr->SVMCTL_மதிப்பு,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	நிலை VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#எனில் CONFIG_SMP
		எனில் (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} இல்லையெனில்
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#எனில் CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		திரும்பு OK;
	நிலை VMCTL_VMINHIBIT_CLEAR:
		ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		எனில் (p->p_misc_flags & MF_SENDA_VM_MISS) {
			கட்டமைப்பு priv *privp;
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
		திரும்பு OK;
	நிலை VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		திரும்பு OK;
	நிலை VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		திரும்பு OK;
  }

  /* Try architecture-specific vmctls. */
  திரும்பு arch_do_vmctl(m_ptr, p);
}
