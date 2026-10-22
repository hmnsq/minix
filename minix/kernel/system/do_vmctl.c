/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ضمن "kernel/system.h"
#ضمن "kernel/vm.h"
#ضمن <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
صحيح do_vmctl(بنية proc * caller, سنیہا * m_ptr)
{
  صحيح proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  بنية proc *p, *rp, **rpp, *نشانہ;

  إذا(ep == SELF) { ep = caller->p_endpoint; }

  إذا(!isokendpt(ep, &proc_nr)) {
	لکھو("do_vmctl: unexpected endpoint %d from VM\n", ep);
	أرجع EINVAL;
  }

  p = proc_addr(proc_nr);

  بدل(m_ptr->SVMCTL_PARAM) {
	حالة VMCTL_CLEAR_PAGEFAULT:
		أكد(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		أرجع OK;
	حالة VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		لكل (rpp = &vmrequest; *rpp != عدم;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			أكد(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.نشانہ, &proc_nr);
			نشانہ = proc_addr(proc_nr);

			/* Check against IPC filters. */
			إذا (!allow_ipc_filtered_memreq(rp, نشانہ))
				استمر;

			/* Reply with request fields. */
			إذا (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
				panic("VMREQUEST wrong type");

			m_ptr->SVMCTL_MRG_نشانہ	=
				rp->p_vmrequest.نشانہ;
			m_ptr->SVMCTL_MRG_ADDR		=
				rp->p_vmrequest.params.check.شروع;
			m_ptr->SVMCTL_MRG_لمائی	=
				rp->p_vmrequest.params.check.لمائی;
			m_ptr->SVMCTL_MRG_FLAG		=
				rp->p_vmrequest.params.check.writeflag;
			m_ptr->SVMCTL_MRG_REQUESTOR	=
				(فراغ *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			أرجع rp->p_vmrequest.req_type;
		}

		أرجع ENOENT;

	حالة VMCTL_MEMREQ_جواب:
		أكد(RTS_ISSET(p, RTS_VMREQUEST));
		أكد(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.نشانہ, &proc_nr);
		نشانہ = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_قدر;
		أكد(p->p_vmrequest.vmresult != VMSUSPEND);

		بدل(p->p_vmrequest.type) {
		حالة VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			اكسر;
		حالة VMSTYPE_DELIVERMSG:
			أكد(p->p_misc_flags & MF_DELIVERMSG);
			أكد(p == نشانہ);
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			اكسر;
		حالة VMSTYPE_MAP:
			أكد(RTS_ISSET(p, RTS_VMREQUEST));
			اكسر;
		افتراضي:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		أرجع OK;

	حالة VMCTL_KERN_PHYSMAP:
	{
		صحيح i = m_ptr->SVMCTL_قدر;
		أرجع arch_phys_map(i,
			(phys_بائٹاں *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_بائٹاں *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	حالة VMCTL_KERN_MAP_جواب:
	{
		أرجع arch_phys_map_جواب(m_ptr->SVMCTL_قدر,
			(vir_بائٹاں) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	حالة VMCTL_VMINHIBIT_سیٹ:
		/* check if we must stop a process on a different CPU */
#إذا CONFIG_SMP
		إذا (p->p_cpu != cpuid) {
			smp_ویلا_سارنی_vminhibit(p);
		} وإلا
#جے_انت
			RTS_سیٹ(p, RTS_VMINHIBIT);
#إذا CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#جے_انت
		أرجع OK;
	حالة VMCTL_VMINHIBIT_CLEAR:
		أكد(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#جے_تعریف_شدہ CONFIG_SMP
		إذا (p->p_misc_flags & MF_SENDA_VM_کھنج) {
			بنية priv *privp;
			p->p_misc_flags &= ~MF_SENDA_VM_کھنج;
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
		بٹ_fill(p->p_stale_tlb, CONFIG_MAX_CPUS);
#جے_انت
		أرجع OK;
	حالة VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		أرجع OK;
	حالة VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		أرجع OK;
  }

  /* Try architecture-specific vmctls. */
  أرجع arch_do_vmctl(m_ptr, p);
}
