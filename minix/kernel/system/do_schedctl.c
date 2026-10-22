#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_schedctl(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಸಂರಚನಾ proc *p;
	uint32_t flags;
	ಪೂರ್ಣಾಙ್ಕ priority, quantum, cpu;
	ಪೂರ್ಣಾಙ್ಕ proc_nr;
	ಪೂರ್ಣಾಙ್ಕ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ಯದಿ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	}

	ಯದಿ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	p = proc_addr(proc_nr);

	ಯದಿ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ಯದಿ((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ಪ್ರತ್ಯಾಗಮ r;
		p->p_scheduler = ರಿಕ್ತ;
	} ಅನ್ಯಥಾ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ಪ್ರತ್ಯಾಗಮ(OK);
}
