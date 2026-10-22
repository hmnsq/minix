/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#समावेश "kernel/system.h"
#समावेश "kernel/vm.h"
#समावेश <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
पूर्णांक do_vmctl(काष्ठा proc * caller, message * m_ptr)
{
  पूर्णांक proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  काष्ठा proc *p, *rp, **rpp, *target;

  अगर(ep == SELF) { ep = caller->p_endpoint; }

  अगर(!isokendpt(ep, &proc_nr)) {
	म_लिखो("do_vmctl: unexpected endpoint %d from VM\n", ep);
	वापस EINVAL;
  }

  p = proc_addr(proc_nr);

  चयन(m_ptr->SVMCTL_PARAM) {
	हाल VMCTL_CLEAR_PAGEFAULT:
		निश्चित(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		वापस OK;
	हाल VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		क्रम (rpp = &vmrequest; *rpp != शून्य;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			निश्चित(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			अगर (!allow_ipc_filtered_memreq(rp, target))
				जारी;

			/* Reply with request fields. */
			अगर (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_TARGET	=
				rp->p_vmrequest.target;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.start;
			m_ptr->SVMCTL_MRG_लंबाई	=
				rp->p_vmrequest.params.check.लंबाई;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(व्योम *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			वापस rp->p_vmrequest.req_type;
		}

		वापस ENOENT;

	हाल VMCTL_MEMREQ_REPLY:
		निश्चित(RTS_ISSET(p, RTS_VMREQUEST));
		निश्चित(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_मूल्य;
		निश्चित(p->p_vmrequest.vmresult != VMSUSPEND);

		चयन(p->p_vmrequest.type) {
		हाल VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			अवरोध;
		हाल VMSTYPE_DELIVERMSG:
			निश्चित(p->p_misc_flags & MF_DELIVERMSG);
			निश्चित(p == target);
			निश्चित(RTS_ISSET(p, RTS_VMREQUEST));
			अवरोध;
		हाल VMSTYPE_MAP:
			निश्चित(RTS_ISSET(p, RTS_VMREQUEST));
			अवरोध;
		शेष:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		वापस OK;

	हाल VMCTL_KERN_PHYSMAP:
	{
		पूर्णांक i = m_ptr->SVMCTL_मूल्य;
		वापस arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	हाल VMCTL_KERN_MAP_REPLY:
	{
		वापस arch_phys_map_reply(m_ptr->SVMCTL_मूल्य,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	हाल VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#अगर CONFIG_SMP
		अगर (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} अन्यथा
#पूर्ण_अगर
			RTS_SET(p, RTS_VMINHIBIT);
#अगर CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#पूर्ण_अगर
		वापस OK;
	हाल VMCTL_VMINHIBIT_CLEAR:
		निश्चित(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#अगर_घोषित CONFIG_SMP
		अगर (p->p_misc_flags & MF_SENDA_VM_MISS) {
			काष्ठा priv *privp;
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
#पूर्ण_अगर
		वापस OK;
	हाल VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		वापस OK;
	हाल VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		वापस OK;
  }

  /* Try architecture-specific vmctls. */
  वापस arch_do_vmctl(m_ptr, p);
}
