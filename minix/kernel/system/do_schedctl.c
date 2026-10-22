#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
পূর্ণাঙ্ক do_schedctl(সংরচনা proc * caller, message * m_ptr)
{
	সংরচনা proc *p;
	uint32_t flags;
	পূর্ণাঙ্ক priority, quantum, cpu;
	পূর্ণাঙ্ক proc_nr;
	পূর্ণাঙ্ক r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	যদি (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ঘূৰাই EINVAL;
	}

	যদি (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ঘূৰাই EINVAL;

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
			ঘূৰাই r;
		p->p_scheduler = রিক্ত;
	} অন্যথা {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ঘূৰাই(OK);
}
