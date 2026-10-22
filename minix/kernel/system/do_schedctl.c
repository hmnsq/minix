#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
पूर्णाङ्क do_schedctl(संरचना proc * caller, message * m_ptr)
{
	संरचना proc *p;
	uint32_t flags;
	पूर्णाङ्क priority, quantum, cpu;
	पूर्णाङ्क proc_nr;
	पूर्णाङ्क r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	जइ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		प्रत्यागम EINVAL;
	}

	जइ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		प्रत्यागम EINVAL;

	p = proc_addr(proc_nr);

	जइ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		जइ((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			प्रत्यागम r;
		p->p_scheduler = रिक्त;
	} अण्णहा {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	प्रत्यागम(OK);
}
