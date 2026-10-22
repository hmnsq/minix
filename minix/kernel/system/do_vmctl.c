/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ "kernel/vm.h"
#અન્તર્ભાવ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
પૂર્ણાંક do_vmctl(સંરચના proc * caller, message * m_ptr)
{
  પૂર્ણાંક proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  સંરચના proc *p, *rp, **rpp, *target;

  જો(ep == SELF) { ep = caller->p_endpoint; }

  જો(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	પાછા EINVAL;
  }

  p = proc_addr(proc_nr);

  પસંદગી(m_ptr->SVMCTL_PARAM) {
	સ્થિતિ VMCTL_CLEAR_PAGEFAULT:
		પ્રતિજ્ઞા(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		પાછા OK;
	સ્થિતિ VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		માટે (rpp = &vmrequest; *rpp != ખાલી;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			પ્રતિજ્ઞા(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			જો (!allow_ipc_filtered_memreq(rp, target))
				ચાલુ;

			/* Reply with request fields. */
			જો (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(શૂન્ય *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			પાછા rp->p_vmrequest.req_type;
		}

		પાછા ENOENT;

	સ્થિતિ VMCTL_MEMREQ_REPLY:
		પ્રતિજ્ઞા(RTS_ISSET(p, RTS_VMREQUEST));
		પ્રતિજ્ઞા(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_કિંમત;
		પ્રતિજ્ઞા(p->p_vmrequest.vmresult != VMSUSPEND);

		પસંદગી(p->p_vmrequest.type) {
		સ્થિતિ VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			તોડો;
		સ્થિતિ VMSTYPE_DELIVERMSG:
			પ્રતિજ્ઞા(p->p_misc_flags & MF_DELIVERMSG);
			પ્રતિજ્ઞા(p == target);
			પ્રતિજ્ઞા(RTS_ISSET(p, RTS_VMREQUEST));
			તોડો;
		સ્થિતિ VMSTYPE_MAP:
			પ્રતિજ્ઞા(RTS_ISSET(p, RTS_VMREQUEST));
			તોડો;
		મૂળભૂત:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		પાછા OK;

	સ્થિતિ VMCTL_KERN_PHYSMAP:
	{
		પૂર્ણાંક i = m_ptr->SVMCTL_કિંમત;
		પાછા arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	સ્થિતિ VMCTL_KERN_MAP_REPLY:
	{
		પાછા arch_phys_map_reply(m_ptr->SVMCTL_કિંમત,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	સ્થિતિ VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#જો CONFIG_SMP
		જો (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} નહીંતર
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#જો CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		પાછા OK;
	સ્થિતિ VMCTL_VMINHIBIT_CLEAR:
		પ્રતિજ્ઞા(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		જો (p->p_misc_flags & MF_SENDA_VM_MISS) {
			સંરચના priv *privp;
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
		પાછા OK;
	સ્થિતિ VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		પાછા OK;
	સ્થિતિ VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		પાછા OK;
  }

  /* Try architecture-specific vmctls. */
  પાછા arch_do_vmctl(m_ptr, p);
}
