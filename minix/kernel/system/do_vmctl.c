/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব "kernel/vm.h"
#অন্তর্ভাব <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_vmctl(কাঠামো proc * caller, message * m_ptr)
{
  পূর্ণসংখ্যা proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  কাঠামো proc *p, *rp, **rpp, *target;

  যদি(ep == SELF) { ep = caller->p_endpoint; }

  যদি(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	ফেরত EINVAL;
  }

  p = proc_addr(proc_nr);

  নির্বাচন(m_ptr->SVMCTL_PARAM) {
	ক্ষেত্র VMCTL_CLEAR_PAGEFAULT:
		প্রতিজ্ঞা(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		ফেরত OK;
	ক্ষেত্র VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		জন্য (rpp = &vmrequest; *rpp != ফাঁকা;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			প্রতিজ্ঞা(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			যদি (!allow_ipc_filtered_memreq(rp, target))
				চালু;

			/* Reply with request fields. */
			যদি (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_TARGET	=
				rp->p_vmrequest.target;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.start;
			m_ptr->SVMCTL_MRG_দৈর্ঘ্য	=
				rp->p_vmrequest.params.check.দৈর্ঘ্য;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(শূন্য *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			ফেরত rp->p_vmrequest.req_type;
		}

		ফেরত ENOENT;

	ক্ষেত্র VMCTL_MEMREQ_REPLY:
		প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
		প্রতিজ্ঞা(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_মান;
		প্রতিজ্ঞা(p->p_vmrequest.vmresult != VMSUSPEND);

		নির্বাচন(p->p_vmrequest.type) {
		ক্ষেত্র VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			বিরতি;
		ক্ষেত্র VMSTYPE_DELIVERMSG:
			প্রতিজ্ঞা(p->p_misc_flags & MF_DELIVERMSG);
			প্রতিজ্ঞা(p == target);
			প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
			বিরতি;
		ক্ষেত্র VMSTYPE_MAP:
			প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
			বিরতি;
		পূর্বনির্ধারিত:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		ফেরত OK;

	ক্ষেত্র VMCTL_KERN_PHYSMAP:
	{
		পূর্ণসংখ্যা i = m_ptr->SVMCTL_মান;
		ফেরত arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	ক্ষেত্র VMCTL_KERN_MAP_REPLY:
	{
		ফেরত arch_phys_map_reply(m_ptr->SVMCTL_মান,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	ক্ষেত্র VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#যদি CONFIG_SMP
		যদি (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} অন্যথা
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#যদি CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		ফেরত OK;
	ক্ষেত্র VMCTL_VMINHIBIT_CLEAR:
		প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		যদি (p->p_misc_flags & MF_SENDA_VM_MISS) {
			কাঠামো priv *privp;
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
		ফেরত OK;
	ক্ষেত্র VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		ফেরত OK;
	ক্ষেত্র VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		ফেরত OK;
  }

  /* Try architecture-specific vmctls. */
  ফেরত arch_do_vmctl(m_ptr, p);
}
