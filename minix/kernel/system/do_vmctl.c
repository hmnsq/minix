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
صحیح do_vmctl(ساخت proc * caller, پیغام * m_ptr)
{
  صحیح proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ساخت proc *p, *rp, **rpp, *ہدف;

  اگر(ep == SELF) { ep = caller->p_endpoint; }

  اگر(!isokendpt(ep, &proc_nr)) {
	لکھو("do_vmctl: unexpected endpoint %d from VM\n", ep);
	واپس EINVAL;
  }

  p = proc_addr(proc_nr);

  انتخاب(m_ptr->SVMCTL_PARAM) {
	حالت VMCTL_CLEAR_PAGEFAULT:
		أكد(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		واپس OK;
	حالت VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		برائے (rpp = &vmrequest; *rpp != تہی;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			أكد(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.ہدف, &proc_nr);
			ہدف = proc_addr(proc_nr);

			/* Check against IPC filters. */
			اگر (!allow_ipc_filtered_memreq(rp, ہدف))
				جاری;

			/* Reply with request fields. */
			اگر (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_ہدف	=
				rp->p_vmrequest.ہدف;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.آغاز;
			m_ptr->SVMCTL_MRG_لمبائی	=
				rp->p_vmrequest.params.check.لمبائی;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(خالی *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			واپس rp->p_vmrequest.req_type;
		}

		واپس ENOENT;

	حالت VMCTL_MEMREQ_جواب:
		أكد(RTS_ISSET(p, RTS_VMREQUEST));
		أكد(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.ہدف, &proc_nr);
		ہدف = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_قدر;
		أكد(p->p_vmrequest.vmresult != VMSUSPEND);

		انتخاب(p->p_vmrequest.type) {
		حالت VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			توڑو;
		حالت VMSTYPE_DELIVERMSG:
			أكد(p->p_misc_flags & MF_DELIVERMSG);
			أكد(p == ہدف);
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			توڑو;
		حالت VMSTYPE_MAP:
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			توڑو;
		طےشدہ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		واپس OK;

	حالت VMCTL_KERN_PHYSMAP:
	{
		صحیح i = m_ptr->SVMCTL_قدر;
		واپس arch_phys_map(i,
			(phys_بائٹیں *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_بائٹیں *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	حالت VMCTL_KERN_MAP_جواب:
	{
		واپس arch_phys_map_جواب(m_ptr->SVMCTL_قدر,
			(vir_بائٹیں) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	حالت VMCTL_VMINHIBIT_سیٹ:
		/* check if we must stop a process on a different CPU */
#اگر CONFIG_SMP
		اگر (p->p_cpu != cpuid) {
			smp_جدول_وقت_vminhibit(p);
		} ورنہ
#اگر_ختم
			RTS_سیٹ(p, RTS_VMINHIBIT);
#اگر CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#اگر_ختم
		واپس OK;
	حالت VMCTL_VMINHIBIT_CLEAR:
		أكد(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#اگر_تعریف_شدہ CONFIG_SMP
		اگر (p->p_misc_flags & MF_SENDA_VM_ناکامی) {
			ساخت priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_ناکامی;
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
		بٹ_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#اگر_ختم
		واپس OK;
	حالت VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		واپس OK;
	حالت VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		واپس OK;
  }

  /* Try architecture-specific vmctls. */
  واپس arch_do_vmctl(m_ptr, p);
}
