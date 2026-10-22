/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ "kernel/vm.h"
#ଅନ୍ତର୍ଭାଵ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vmctl(ସଂରଚନା proc * caller, message * m_ptr)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  ସଂରଚନା proc *p, *rp, **rpp, *target;

  ଯଦି(ep == SELF) { ep = caller->p_endpoint; }

  ଯଦି(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	ଫେରନ୍ତୁ EINVAL;
  }

  p = proc_addr(proc_nr);

  ବାଛ(m_ptr->SVMCTL_PARAM) {
	ସ୍ଥିତି VMCTL_CLEAR_PAGEFAULT:
		ପ୍ରତିଜ୍ଞା(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		ପାଇଁ (rpp = &vmrequest; *rpp != ଖାଲି;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ପ୍ରତିଜ୍ଞା(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			ଯଦି (!allow_ipc_filtered_memreq(rp, target))
				ଜାରି;

			/* Reply with request fields. */
			ଯଦି (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(ଶୂନ୍ୟ *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			ଫେରନ୍ତୁ rp->p_vmrequest.req_type;
		}

		ଫେରନ୍ତୁ ENOENT;

	ସ୍ଥିତି VMCTL_MEMREQ_REPLY:
		ପ୍ରତିଜ୍ଞା(RTS_ISSET(p, RTS_VMREQUEST));
		ପ୍ରତିଜ୍ଞା(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_ମୂଲ୍ୟ;
		ପ୍ରତିଜ୍ଞା(p->p_vmrequest.vmresult != VMSUSPEND);

		ବାଛ(p->p_vmrequest.type) {
		ସ୍ଥିତି VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି VMSTYPE_DELIVERMSG:
			ପ୍ରତିଜ୍ଞା(p->p_misc_flags & MF_DELIVERMSG);
			ପ୍ରତିଜ୍ଞା(p == target);
			ପ୍ରତିଜ୍ଞା(RTS_ISSET(p, RTS_VMREQUEST));
			ଭାଙ୍ଗ;
		ସ୍ଥିତି VMSTYPE_MAP:
			ପ୍ରତିଜ୍ଞା(RTS_ISSET(p, RTS_VMREQUEST));
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		ଫେରନ୍ତୁ OK;

	ସ୍ଥିତି VMCTL_KERN_PHYSMAP:
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = m_ptr->SVMCTL_ମୂଲ୍ୟ;
		ଫେରନ୍ତୁ arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	ସ୍ଥିତି VMCTL_KERN_MAP_REPLY:
	{
		ଫେରନ୍ତୁ arch_phys_map_reply(m_ptr->SVMCTL_ମୂଲ୍ୟ,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	ସ୍ଥିତି VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#ଯଦି CONFIG_SMP
		ଯଦି (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} ଅନ୍ୟଥା
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#ଯଦି CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_VMINHIBIT_CLEAR:
		ପ୍ରତିଜ୍ଞା(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		ଯଦି (p->p_misc_flags & MF_SENDA_VM_MISS) {
			ସଂରଚନା priv *privp;
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
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		ଫେରନ୍ତୁ OK;
  }

  /* Try architecture-specific vmctls. */
  ଫେରନ୍ତୁ arch_do_vmctl(m_ptr, p);
}
