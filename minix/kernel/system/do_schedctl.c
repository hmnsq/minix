#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
صحيح do_schedctl(بنية proc * caller, message * m_ptr)
{
	بنية proc *p;
	uint32_t flags;
	صحيح priority, quantum, cpu;
	صحيح proc_nr;
	صحيح r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	إذا (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		إرجاع EINVAL;
	}

	إذا (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		إرجاع EINVAL;

	p = proc_addr(proc_nr);

	إذا ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		إذا((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			إرجاع r;
		p->p_scheduler = عدم;
	} وإلا {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	إرجاع(OK);
}
