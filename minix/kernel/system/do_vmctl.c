/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
ਪੂਰਨ do_vmctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਪੂਰਨ proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ਬਣਤਰ proc *p, *rp, **rpp, *ਨਿਸ਼ਾਨਾ;

  ਜੇ(ep == SELF) { ep = caller->p_endpoint; }

  ਜੇ(!isokendpt(ep, &proc_nr)) {
	ਲਿਖੋ("do_vmctl: unexpected endpoint %d from VM\n", ep);
	ਵਾਪਸ EINVAL;
  }

  p = proc_addr(proc_nr);

  ਚੋਣ(m_ptr->SVMCTL_PARAM) {
	ਹਾਲਤ VMCTL_CLEAR_PAGEFAULT:
		ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		ਲਈ (rpp = &vmrequest; *rpp != ਨਿਰਾ;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.ਨਿਸ਼ਾਨਾ, &proc_nr);
			ਨਿਸ਼ਾਨਾ = proc_addr(proc_nr);

			/* Check against IPC filters. */
			ਜੇ (!allow_ipc_filtered_memreq(rp, ਨਿਸ਼ਾਨਾ))
				ਜਾਰੀ;

			/* Reply with request fields. */
			ਜੇ (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_ਨਿਸ਼ਾਨਾ	=
				rp->p_vmrequest.ਨਿਸ਼ਾਨਾ;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.ਸ਼ੁਰੂ;
			m_ptr->SVMCTL_MRG_ਲੰਮਾਈ	=
				rp->p_vmrequest.params.check.ਲੰਮਾਈ;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(ਖਾਲੀ *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			ਵਾਪਸ rp->p_vmrequest.req_type;
		}

		ਵਾਪਸ ENOENT;

	ਹਾਲਤ VMCTL_MEMREQ_ਜਵਾਬ:
		ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_VMREQUEST));
		ਪ੍ਰਤਿਜ੍ਞਾ(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.ਨਿਸ਼ਾਨਾ, &proc_nr);
		ਨਿਸ਼ਾਨਾ = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_ਮੁੱਲ;
		ਪ੍ਰਤਿਜ੍ਞਾ(p->p_vmrequest.vmresult != VMSUSPEND);

		ਚੋਣ(p->p_vmrequest.type) {
		ਹਾਲਤ VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ਰੋਕੋ;
		ਹਾਲਤ VMSTYPE_DELIVERMSG:
			ਪ੍ਰਤਿਜ੍ਞਾ(p->p_misc_flags & MF_DELIVERMSG);
			ਪ੍ਰਤਿਜ੍ਞਾ(p == ਨਿਸ਼ਾਨਾ);
			ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_VMREQUEST));
			ਰੋਕੋ;
		ਹਾਲਤ VMSTYPE_MAP:
			ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_VMREQUEST));
			ਰੋਕੋ;
		ਮੂਲ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		ਵਾਪਸ OK;

	ਹਾਲਤ VMCTL_KERN_PHYSMAP:
	{
		ਪੂਰਨ i = m_ptr->SVMCTL_ਮੁੱਲ;
		ਵਾਪਸ arch_phys_map(i,
			(phys_ਬਾਈਟਾਂ *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_ਬਾਈਟਾਂ *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	ਹਾਲਤ VMCTL_KERN_MAP_ਜਵਾਬ:
	{
		ਵਾਪਸ arch_phys_map_ਜਵਾਬ(m_ptr->SVMCTL_ਮੁੱਲ,
			(vir_ਬਾਈਟਾਂ) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	ਹਾਲਤ VMCTL_VMINHIBIT_ਸੈੱਟ:
		/* check if we must stop a process on a different CPU */
#ਜੇ CONFIG_SMP
		ਜੇ (p->p_cpu != cpuid) {
			smp_ਸਮਾਂ_ਸਾਰਣੀ_vminhibit(p);
		} ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			RTS_ਸੈੱਟ(p, RTS_VMINHIBIT);
#ਜੇ CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#ਜੇ_ਅੰਤ
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_VMINHIBIT_CLEAR:
		ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
		ਜੇ (p->p_misc_flags & MF_SENDA_VM_ਖੁੰਝ) {
			ਬਣਤਰ priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_ਖੁੰਝ;
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
		ਬਿੱਟ_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#ਜੇ_ਅੰਤ
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		ਵਾਪਸ OK;
  }

  /* Try architecture-specific vmctls. */
  ਵਾਪਸ arch_do_vmctl(m_ptr, p);
}
