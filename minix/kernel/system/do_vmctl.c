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
पूर्णांक do_vmctl(संरचना proc * caller, message * m_ptr)
{
  पूर्णांक proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  संरचना proc *p, *rp, **rpp, *target;

  जर(ep == SELF) { ep = caller->p_endpoint; }

  जर(!isokendpt(ep, &proc_nr)) {
	म_लिखो("do_vmctl: unexpected endpoint %d from VM\n", ep);
	परत EINVAL;
  }

  p = proc_addr(proc_nr);

  निवड(m_ptr->SVMCTL_PARAM) {
	स्थिती VMCTL_CLEAR_PAGEFAULT:
		प्रतिज्ञा(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		परत OK;
	स्थिती VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		साठी (rpp = &vmrequest; *rpp != रिक्त;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			प्रतिज्ञा(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			जर (!allow_ipc_filtered_memreq(rp, target))
				सुरूठेवा;

			/* Reply with request fields. */
			जर (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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

			परत rp->p_vmrequest.req_type;
		}

		परत ENOENT;

	स्थिती VMCTL_MEMREQ_REPLY:
		प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
		प्रतिज्ञा(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_मूल्य;
		प्रतिज्ञा(p->p_vmrequest.vmresult != VMSUSPEND);

		निवड(p->p_vmrequest.type) {
		स्थिती VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			थांबा;
		स्थिती VMSTYPE_DELIVERMSG:
			प्रतिज्ञा(p->p_misc_flags & MF_DELIVERMSG);
			प्रतिज्ञा(p == target);
			प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
			थांबा;
		स्थिती VMSTYPE_MAP:
			प्रतिज्ञा(RTS_ISSET(p, RTS_VMREQUEST));
			थांबा;
		पूर्वनिर्धारित:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		परत OK;

	स्थिती VMCTL_KERN_PHYSMAP:
	{
		पूर्णांक i = m_ptr->SVMCTL_मूल्य;
		परत arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	स्थिती VMCTL_KERN_MAP_REPLY:
	{
		परत arch_phys_map_reply(m_ptr->SVMCTL_मूल्य,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	स्थिती VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#जर CONFIG_SMP
		जर (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} अन्यथा
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#जर CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		परत OK;
	स्थिती VMCTL_VMINHIBIT_CLEAR:
		प्रतिज्ञा(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		जर (p->p_misc_flags & MF_SENDA_VM_MISS) {
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
		परत OK;
	स्थिती VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		परत OK;
	स्थिती VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		परत OK;
  }

  /* Try architecture-specific vmctls. */
  परत arch_do_vmctl(m_ptr, p);
}
