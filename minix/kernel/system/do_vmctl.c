/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_vmctl(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  ಪೂರ್ಣಾಙ್ಕ proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ಸಂರಚನಾ proc *p, *rp, **rpp, *target;

  ಯದಿ(ep == SELF) { ep = caller->p_endpoint; }

  ಯದಿ(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	ಪ್ರತ್ಯಾಗಮ EINVAL;
  }

  p = proc_addr(proc_nr);

  ವಿಕಲ್ಪ(m_ptr->SVMCTL_PARAM) {
	ಪಕ್ಷ VMCTL_CLEAR_PAGEFAULT:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		ಪ್ರತಿ (rpp = &vmrequest; *rpp != ರಿಕ್ತ;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			ಯದಿ (!allow_ipc_filtered_memreq(rp, target))
				ಅನುವರ್ತ;

			/* Reply with request fields. */
			ಯದಿ (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(ಶೂನ್ಯ *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			ಪ್ರತ್ಯಾಗಮ rp->p_vmrequest.req_type;
		}

		ಪ್ರತ್ಯಾಗಮ ENOENT;

	ಪಕ್ಷ VMCTL_MEMREQ_REPLY:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
		ಪ್ರತಿಜ್ಞಾ(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_ಮೌಲ್ಯ;
		ಪ್ರತಿಜ್ಞಾ(p->p_vmrequest.vmresult != VMSUSPEND);

		ವಿಕಲ್ಪ(p->p_vmrequest.type) {
		ಪಕ್ಷ VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ಭಙ್ಗ;
		ಪಕ್ಷ VMSTYPE_DELIVERMSG:
			ಪ್ರತಿಜ್ಞಾ(p->p_misc_flags & MF_DELIVERMSG);
			ಪ್ರತಿಜ್ಞಾ(p == target);
			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
			ಭಙ್ಗ;
		ಪಕ್ಷ VMSTYPE_MAP:
			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		ಪ್ರತ್ಯಾಗಮ OK;

	ಪಕ್ಷ VMCTL_KERN_PHYSMAP:
	{
		ಪೂರ್ಣಾಙ್ಕ i = m_ptr->SVMCTL_ಮೌಲ್ಯ;
		ಪ್ರತ್ಯಾಗಮ arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	ಪಕ್ಷ VMCTL_KERN_MAP_REPLY:
	{
		ಪ್ರತ್ಯಾಗಮ arch_phys_map_reply(m_ptr->SVMCTL_ಮೌಲ್ಯ,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	ಪಕ್ಷ VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#ಯದಿ CONFIG_SMP
		ಯದಿ (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} ಅನ್ಯಥಾ
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#ಯದಿ CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_VMINHIBIT_CLEAR:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		ಯದಿ (p->p_misc_flags & MF_SENDA_VM_MISS) {
			ಸಂರಚನಾ priv *privp;
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
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		ಪ್ರತ್ಯಾಗಮ OK;
  }

  /* Try architecture-specific vmctls. */
  ಪ್ರತ್ಯಾಗಮ arch_do_vmctl(m_ptr, p);
}
