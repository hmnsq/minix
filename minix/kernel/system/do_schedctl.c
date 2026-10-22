#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
цео do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	цео priority, quantum, cpu;
	цео proc_nr;
	цео r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ако (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		врати EINVAL;
	}

	ако (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		врати EINVAL;

	p = proc_addr(proc_nr);

	ако ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ако((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			врати r;
		p->p_scheduler = NULL;
	} инако {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	врати(OK);
}
