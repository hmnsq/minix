/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
पूर्णाङ्क do_vmctl(संरचना proc * caller, message * m_ptr)
{
  पूर्णाङ्क proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  संरचना proc *p, *rp, **rpp, *target;

  सचे(ep == SELF) { ep = caller->p_endpoint; }

  सचे(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	पटिनिवत्त EINVAL;
  }

  p = proc_addr(proc_nr);

  विकल्प(m_ptr->SVMCTL_PARAM) {
	पक्ष VMCTL_CLEAR_PAGEFAULT:
		प्रतिज्ञा(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		पटिनिवत्त OK;
	पक्ष VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		अत्थाय (rpp = &vmrequest; *rpp != रिक्त;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			प्रतिज्ञा(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			सचे (!allow_ipc_filtered_memreq(rp, target))
				अनुवर्त;

			/* Reply with request fields. */
			सचे (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(शून्य *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			पटिनिवत्त rp->p_vmrequest.req_type;
		}

		पटिनिवत्त ENOENT;

	पक्ष VMCTL_MEMREQ_REPLY:
		प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
		प्रतिज्ञा(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_मानम्;
		प्रतिज्ञा(p->p_vmrequest.vmresult != VMSUSPEND);

		विकल्प(p->p_vmrequest.type) {
		पक्ष VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			भङ्ग;
		पक्ष VMSTYPE_DELIVERMSG:
			प्रतिज्ञा(p->p_misc_flags & MF_DELIVERMSG);
			प्रतिज्ञा(p == target);
			प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
			भङ्ग;
		पक्ष VMSTYPE_MAP:
			प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
			भङ्ग;
		स्वभाव:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		पटिनिवत्त OK;

	पक्ष VMCTL_KERN_PHYSMAP:
	{
		पूर्णाङ्क i = m_ptr->SVMCTL_मानम्;
		पटिनिवत्त arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	पक्ष VMCTL_KERN_MAP_REPLY:
	{
		पटिनिवत्त arch_phys_map_reply(m_ptr->SVMCTL_मानम्,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	पक्ष VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#सचे CONFIG_SMP
		सचे (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} अञ्ञथा
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#सचे CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		पटिनिवत्त OK;
	पक्ष VMCTL_VMINHIBIT_CLEAR:
		प्रतिज्ञा(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		सचे (p->p_misc_flags & MF_SENDA_VM_MISS) {
			संरचना priv *privp;
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
		पटिनिवत्त OK;
	पक्ष VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		पटिनिवत्त OK;
	पक्ष VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		पटिनिवत्त OK;
  }

  /* Try architecture-specific vmctls. */
  पटिनिवत्त arch_do_vmctl(m_ptr, p);
}
