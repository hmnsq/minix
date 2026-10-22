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
পূর্ণাঙ্ক do_vmctl(সংরচনা proc * caller, message * m_ptr)
{
  পূর্ণাঙ্ক proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  সংরচনা proc *p, *rp, **rpp, *target;

  যদি(ep == SELF) { ep = caller->p_endpoint; }

  যদি(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	প্রত্যাগম EINVAL;
  }

  p = proc_addr(proc_nr);

  বিকল্প(m_ptr->SVMCTL_PARAM) {
	পক্ষ VMCTL_CLEAR_PAGEFAULT:
		প্রতিজ্ঞা(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		প্রত্যাগম OK;
	পক্ষ VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		প্রতি (rpp = &vmrequest; *rpp != রিক্ত;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			প্রতিজ্ঞা(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			যদি (!allow_ipc_filtered_memreq(rp, target))
				অনুবর্ত;

			/* Reply with request fields. */
			যদি (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(শূন্য *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			প্রত্যাগম rp->p_vmrequest.req_type;
		}

		প্রত্যাগম ENOENT;

	পক্ষ VMCTL_MEMREQ_REPLY:
		প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
		প্রতিজ্ঞা(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_মান;
		প্রতিজ্ঞা(p->p_vmrequest.vmresult != VMSUSPEND);

		বিকল্প(p->p_vmrequest.type) {
		পক্ষ VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ভঙ্গ;
		পক্ষ VMSTYPE_DELIVERMSG:
			প্রতিজ্ঞা(p->p_misc_flags & MF_DELIVERMSG);
			প্রতিজ্ঞা(p == target);
			প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
			ভঙ্গ;
		পক্ষ VMSTYPE_MAP:
			প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMREQUEST));
			ভঙ্গ;
		স্বভাব:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		প্রত্যাগম OK;

	পক্ষ VMCTL_KERN_PHYSMAP:
	{
		পূর্ণাঙ্ক i = m_ptr->SVMCTL_মান;
		প্রত্যাগম arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	পক্ষ VMCTL_KERN_MAP_REPLY:
	{
		প্রত্যাগম arch_phys_map_reply(m_ptr->SVMCTL_মান,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	পক্ষ VMCTL_VMINHIBIT_SET:
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
		প্রত্যাগম OK;
	পক্ষ VMCTL_VMINHIBIT_CLEAR:
		প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		যদি (p->p_misc_flags & MF_SENDA_VM_MISS) {
			সংরচনা priv *privp;
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
		প্রত্যাগম OK;
	পক্ষ VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		প্রত্যাগম OK;
	পক্ষ VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		প্রত্যাগম OK;
  }

  /* Try architecture-specific vmctls. */
  প্রত্যাগম arch_do_vmctl(m_ptr, p);
}
