/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
పూర్ణాంకం do_vmctl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  పూర్ణాంకం proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  నిర్మాణం proc *p, *rp, **rpp, *లక్ష్యం;

  ఒకవేళ(ep == SELF) { ep = caller->p_endpoint; }

  ఒకవేళ(!isokendpt(ep, &proc_nr)) {
	రాయి("do_vmctl: unexpected endpoint %d from VM\n", ep);
	తిరిగి EINVAL;
  }

  p = proc_addr(proc_nr);

  ఎంపిక(m_ptr->SVMCTL_PARAM) {
	స్థితి VMCTL_CLEAR_PAGEFAULT:
		ప్రతిజ్ఞా(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		తిరిగి OK;
	స్థితి VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		కొరకు (rpp = &vmrequest; *rpp != ఖాళీ;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			ప్రతిజ్ఞా(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.లక్ష్యం, &proc_nr);
			లక్ష్యం = proc_addr(proc_nr);

			/* Check against IPC filters. */
			ఒకవేళ (!allow_ipc_filtered_memreq(rp, లక్ష్యం))
				కొనసాగు;

			/* Reply with request fields. */
			ఒకవేళ (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_లక్ష్యం	=
				rp->p_vmrequest.లక్ష్యం;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.ప్రారంభం;
			m_ptr->SVMCTL_MRG_నిడివి	=
				rp->p_vmrequest.params.check.నిడివి;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(శూన్య *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			తిరిగి rp->p_vmrequest.req_type;
		}

		తిరిగి ENOENT;

	స్థితి VMCTL_MEMREQ_జవాబు:
		ప్రతిజ్ఞా(RTS_ISSET(p, RTS_VMREQUEST));
		ప్రతిజ్ఞా(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.లక్ష్యం, &proc_nr);
		లక్ష్యం = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_విలువ;
		ప్రతిజ్ఞా(p->p_vmrequest.vmresult != VMSUSPEND);

		ఎంపిక(p->p_vmrequest.type) {
		స్థితి VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			ఆపు;
		స్థితి VMSTYPE_DELIVERMSG:
			ప్రతిజ్ఞా(p->p_misc_flags & MF_DELIVERMSG);
			ప్రతిజ్ఞా(p == లక్ష్యం);
			ప్రతిజ్ఞా(RTS_ISSET(p, RTS_VMREQUEST));
			ఆపు;
		స్థితి VMSTYPE_MAP:
			ప్రతిజ్ఞా(RTS_ISSET(p, RTS_VMREQUEST));
			ఆపు;
		అప్రమేయ:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		తిరిగి OK;

	స్థితి VMCTL_KERN_PHYSMAP:
	{
		పూర్ణాంకం i = m_ptr->SVMCTL_విలువ;
		తిరిగి arch_phys_map(i,
			(phys_బైట్లు *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_బైట్లు *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	స్థితి VMCTL_KERN_MAP_జవాబు:
	{
		తిరిగి arch_phys_map_జవాబు(m_ptr->SVMCTL_విలువ,
			(vir_బైట్లు) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	స్థితి VMCTL_VMINHIBIT_సమితి:
		/* check if we must stop a process on a different CPU */
#ఒకవేళ CONFIG_SMP
		ఒకవేళ (p->p_cpu != cpuid) {
			smp_కాలపట్టిక_vminhibit(p);
		} లేకపోతే
#అయితే_ముగింపు
			RTS_సమితి(p, RTS_VMINHIBIT);
#ఒకవేళ CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#అయితే_ముగింపు
		తిరిగి OK;
	స్థితి VMCTL_VMINHIBIT_CLEAR:
		ప్రతిజ్ఞా(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#నిర్వచితమైతే CONFIG_SMP
		ఒకవేళ (p->p_misc_flags & MF_SENDA_VM_తప్పిపోవు) {
			నిర్మాణం priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_తప్పిపోవు;
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
		బిట్లు_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#అయితే_ముగింపు
		తిరిగి OK;
	స్థితి VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		తిరిగి OK;
	స్థితి VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		తిరిగి OK;
  }

  /* Try architecture-specific vmctls. */
  తిరిగి arch_do_vmctl(m_ptr, p);
}
