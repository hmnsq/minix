#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
পূর্ণসংখ্যা do_schedctl(কাঠামো proc * caller, message * m_ptr)
{
	কাঠামো proc *p;
	uint32_t flags;
	পূর্ণসংখ্যা priority, quantum, cpu;
	পূর্ণসংখ্যা proc_nr;
	পূর্ণসংখ্যা r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	যদি (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ফেরত EINVAL;
	}

	যদি (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ফেরত EINVAL;

	p = proc_addr(proc_nr);

	যদি ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		যদি((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ফেরত r;
		p->p_scheduler = ফাঁকা;
	} অন্যথা {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ফেরত(OK);
}
