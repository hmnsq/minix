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
पूर्णाङ्क do_vmctl(संरचना proc * caller, सन्देशः * m_ptr)
{
  पूर्णाङ्क proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  संरचना proc *p, *rp, **rpp, *target;

  यदि(ep == SELF) { ep = caller->p_endpoint; }

  यदि(!isokendpt(ep, &proc_nr)) {
	प्रारूप_लिख("do_vmctl: unexpected endpoint %d from VM\n", ep);
	प्रत्यागम EINVAL;
  }

  p = proc_addr(proc_nr);

  विकल्प(m_ptr->SVMCTL_PARAM) {
	पक्ष VMCTL_CLEAR_PAGEFAULT:
		प्रतिज्ञा(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		प्रत्यागम OK;
	पक्ष VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		प्रति (rpp = &vmrequest; *rpp != रिक्त;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			प्रतिज्ञा(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			यदि (!allow_ipc_filtered_memreq(rp, target))
				अनुवर्त;

			/* Reply with request fields. */
			यदि (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_TARGET	=
				rp->p_vmrequest.target;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.start;
			m_ptr->SVMCTL_MRG_दीर्घता	=
				rp->p_vmrequest.params.check.दीर्घता;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(शून्य *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			प्रत्यागम rp->p_vmrequest.req_type;
		}

		प्रत्यागम ENOENT;

	पक्ष VMCTL_MEMREQ_प्रत्युत्तरम्:
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
		प्रत्यागम OK;

	पक्ष VMCTL_KERN_PHYSMAP:
	{
		पूर्णाङ्क i = m_ptr->SVMCTL_मानम्;
		प्रत्यागम arch_phys_map(i,
			(phys_अष्टकानि *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_अष्टकानि *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	पक्ष VMCTL_KERN_MAP_प्रत्युत्तरम्:
	{
		प्रत्यागम arch_phys_map_प्रत्युत्तरम्(m_ptr->SVMCTL_मानम्,
			(vir_अष्टकानि) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	पक्ष VMCTL_VMINHIBIT_समुच्चयः:
		/* check if we must stop a process on a different CPU */
#यदि CONFIG_SMP
		यदि (p->p_cpu != cpuid) {
			smp_कालसारणी_vminhibit(p);
		} अन्यथा
#यदि_अन्तः
			RTS_समुच्चयः(p, RTS_VMINHIBIT);
#यदि CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#यदि_अन्तः
		प्रत्यागम OK;
	पक्ष VMCTL_VMINHIBIT_CLEAR:
		प्रतिज्ञा(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#यदि_परिभाषितम् CONFIG_SMP
		यदि (p->p_misc_flags & MF_SENDA_VM_अलब्धिः) {
			संरचना priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_अलब्धिः;
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
		द्वयङ्काः_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#यदि_अन्तः
		प्रत्यागम OK;
	पक्ष VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		प्रत्यागम OK;
	पक्ष VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		प्रत्यागम OK;
  }

  /* Try architecture-specific vmctls. */
  प्रत्यागम arch_do_vmctl(m_ptr, p);
}
