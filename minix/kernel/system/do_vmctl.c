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
ಪೂರ್ಣಾಂಕ do_vmctl(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
  ಪೂರ್ಣಾಂಕ proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ರಚನೆ proc *p, *rp, **rpp, *ಗುರಿ;

  ಆದರೆ(ep == SELF) { ep = caller->p_endpoint; }

  ಆದರೆ(!isokendpt(ep, &proc_nr)) {
	ಬರೆ("do_vmctl: unexpected endpoint %d from VM\n", ep);
	ಹಿಂತಿರುಗಿ EINVAL;
  }

  p = proc_addr(proc_nr);

  ಆಯ್ಕೆ(m_ptr->SVMCTL_PARAM) {
	ಸ್ಥಿತಿ VMCTL_CLEAR_PAGEFAULT:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		ಗಾಗಿ (rpp = &vmrequest; *rpp != ಖಾಲಿ;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.ಗುರಿ, &proc_nr);
			ಗುರಿ = proc_addr(proc_nr);

			/* Check against IPC filters. */
			ಆದರೆ (!allow_ipc_filtered_memreq(rp, ಗುರಿ))
				ಮುಂದುವರಿ;

			/* Reply with request fields. */
			ಆದರೆ (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_ಗುರಿ	=
				rp->p_vmrequest.ಗುರಿ;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.ಆರಂಭ;
			m_ptr->SVMCTL_MRG_ಉದ್ದಳತೆ	=
				rp->p_vmrequest.params.check.ಉದ್ದಳತೆ;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(ಶೂನ್ಯ *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			ಹಿಂತಿರುಗಿ rp->p_vmrequest.req_type;
		}

		ಹಿಂತಿರುಗಿ ENOENT;

	ಸ್ಥಿತಿ VMCTL_MEMREQ_ಉತ್ತರ:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
		ಪ್ರತಿಜ್ಞಾ(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.ಗುರಿ, &proc_nr);
		ಗುರಿ = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_ಮೌಲ್ಯ;
		ಪ್ರತಿಜ್ಞಾ(p->p_vmrequest.vmresult != VMSUSPEND);

		ಆಯ್ಕೆ(p->p_vmrequest.type) {
		ಸ್ಥಿತಿ VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VMSTYPE_DELIVERMSG:
			ಪ್ರತಿಜ್ಞಾ(p->p_misc_flags & MF_DELIVERMSG);
			ಪ್ರತಿಜ್ಞಾ(p == ಗುರಿ);
			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VMSTYPE_MAP:
			ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMREQUEST));
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		ಹಿಂತಿರುಗಿ OK;

	ಸ್ಥಿತಿ VMCTL_KERN_PHYSMAP:
	{
		ಪೂರ್ಣಾಂಕ i = m_ptr->SVMCTL_ಮೌಲ್ಯ;
		ಹಿಂತಿರುಗಿ arch_phys_map(i,
			(phys_ಬೈಟುಗಳು *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_ಬೈಟುಗಳು *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	ಸ್ಥಿತಿ VMCTL_KERN_MAP_ಉತ್ತರ:
	{
		ಹಿಂತಿರುಗಿ arch_phys_map_ಉತ್ತರ(m_ptr->SVMCTL_ಮೌಲ್ಯ,
			(vir_ಬೈಟುಗಳು) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	ಸ್ಥಿತಿ VMCTL_VMINHIBIT_ಗಣ:
		/* check if we must stop a process on a different CPU */
#ಆದರೆ CONFIG_SMP
		ಆದರೆ (p->p_cpu != cpuid) {
			smp_ವೇಳಾಪಟ್ಟಿ_vminhibit(p);
		} ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			RTS_ಗಣ(p, RTS_VMINHIBIT);
#ಆದರೆ CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_VMINHIBIT_CLEAR:
		ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
		ಆದರೆ (p->p_misc_flags & MF_SENDA_VM_ತಪ್ಪಿದ) {
			ರಚನೆ priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_ತಪ್ಪಿದ;
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
		ಬಿಟ್ಟುಗಳು_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		ಹಿಂತಿರುಗಿ OK;
  }

  /* Try architecture-specific vmctls. */
  ಹಿಂತಿರುಗಿ arch_do_vmctl(m_ptr, p);
}
