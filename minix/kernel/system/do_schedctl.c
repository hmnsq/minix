#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
முழு do_schedctl(கட்டமைப்பு proc * caller, message * m_ptr)
{
	கட்டமைப்பு proc *p;
	uint32_t flags;
	முழு priority, quantum, cpu;
	முழு proc_nr;
	முழு r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	எனில் (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		திரும்பு EINVAL;
	}

	எனில் (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		திரும்பு EINVAL;

	p = proc_addr(proc_nr);

	எனில் ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		எனில்((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			திரும்பு r;
		p->p_scheduler = வெற்று;
	} இல்லையெனில் {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	திரும்பு(OK);
}
