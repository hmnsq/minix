/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#include "kernel/system.h"
#include "kernel/vm.h"
#include <assert.h>

/*===========================================================================*
 *				do_vmctl				     *
 *===========================================================================*/
정수 do_vmctl(struct proc * caller, message * m_ptr)
{
  정수 proc_nr;
  endpoint_t ep = m_ptr->SVMCTL_WHO;
  struct proc *p, *rp, **rpp, *target;

  만약(ep == SELF) { ep = caller->p_endpoint; }

  만약(!isokendpt(ep, &proc_nr)) {
	printf("do_vmctl: unexpected endpoint %d from VM\n", ep);
	반환 EINVAL;
  }

  p = proc_addr(proc_nr);

  switch(m_ptr->SVMCTL_PARAM) {
	case VMCTL_CLEAR_PAGEFAULT:
		assert(RTS_ISSET(p,RTS_PAGEFAULT));
		RTS_UNSET(p, RTS_PAGEFAULT);
		반환 OK;
	case VMCTL_MEMREQ_GET:
		/* Send VM the information about the memory request. We can
		 * not simply send the first request on the list, because IPC
		 * filters may forbid VM from getting requests for particular
		 * sources. However, IPC filters are used only in rare cases.
		 */
		위해 (rpp = &vmrequest; *rpp != NULL;
		    rpp = &(*rpp)->p_vmrequest.nextrequestor) {
			rp = *rpp;

			assert(RTS_ISSET(rp, RTS_VMREQUEST));

			okendpt(rp->p_vmrequest.target, &proc_nr);
			target = proc_addr(proc_nr);

			/* Check against IPC filters. */
			만약 (!allow_ipc_filtered_memreq(rp, target))
				continue;

			/* Reply with request fields. */
			만약 (rp->p_vmrequest.req_type != VMPTYPE_CHECK)
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
				(void *) rp->p_endpoint;

			rp->p_vmrequest.vmresult = VMSUSPEND;

			/* Remove from request chain. */
			*rpp = rp->p_vmrequest.nextrequestor;

			반환 rp->p_vmrequest.req_type;
		}

		반환 ENOENT;

	case VMCTL_MEMREQ_REPLY:
		assert(RTS_ISSET(p, RTS_VMREQUEST));
		assert(p->p_vmrequest.vmresult == VMSUSPEND);
  		okendpt(p->p_vmrequest.target, &proc_nr);
		target = proc_addr(proc_nr);
		p->p_vmrequest.vmresult = m_ptr->SVMCTL_VALUE;
		assert(p->p_vmrequest.vmresult != VMSUSPEND);

		switch(p->p_vmrequest.type) {
		case VMSTYPE_KERNELCALL:
			/*
			 * we will have to resume execution of the kernel call
			 * as soon the scheduler picks up this process again
			 */
			p->p_misc_flags |= MF_KCALL_RESUME;
			break;
		case VMSTYPE_DELIVERMSG:
			assert(p->p_misc_flags & MF_DELIVERMSG);
			assert(p == target);
			assert(RTS_ISSET(p, RTS_VMREQUEST));
			break;
		case VMSTYPE_MAP:
			assert(RTS_ISSET(p, RTS_VMREQUEST));
			break;
		default:
			panic("strange request type: %d",p->p_vmrequest.type);
		}

		RTS_UNSET(p, RTS_VMREQUEST);
		반환 OK;

	case VMCTL_KERN_PHYSMAP:
	{
		정수 i = m_ptr->SVMCTL_VALUE;
		반환 arch_phys_map(i,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_ADDR,
			(phys_bytes *) &m_ptr->SVMCTL_MAP_PHYS_LEN,
			&m_ptr->SVMCTL_MAP_FLAGS);
	}
	case VMCTL_KERN_MAP_REPLY:
	{
		반환 arch_phys_map_reply(m_ptr->SVMCTL_VALUE,
			(vir_bytes) m_ptr->SVMCTL_MAP_VIR_ADDR);
	}
	case VMCTL_VMINHIBIT_SET:
		/* check if we must stop a process on a different CPU */
#만약 CONFIG_SMP
		만약 (p->p_cpu != cpuid) {
			smp_schedule_vminhibit(p);
		} 아니면
#endif
			RTS_SET(p, RTS_VMINHIBIT);
#만약 CONFIG_SMP
		p->p_misc_flags |= MF_FLUSH_TLB;
#endif
		반환 OK;
	case VMCTL_VMINHIBIT_CLEAR:
		assert(RTS_ISSET(p, RTS_VMINHIBIT));
		/*
		 * the processes is certainly not runnable, no need to tell its
		 * cpu
		 */
		RTS_UNSET(p, RTS_VMINHIBIT);
#ifdef CONFIG_SMP
		만약 (p->p_misc_flags & MF_SENDA_VM_MISS) {
			struct priv *privp;
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
		반환 OK;
	case VMCTL_CLEARMAPCACHE:
		/* VM says: forget about old mappings we have cached. */
		mem_clear_mapcache();
		반환 OK;
	case VMCTL_BOOTINHIBIT_CLEAR:
		RTS_UNSET(p, RTS_BOOTINHIBIT);
		반환 OK;
  }

  /* Try architecture-specific vmctls. */
  반환 arch_do_vmctl(m_ptr, p);
}
