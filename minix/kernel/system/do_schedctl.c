#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
целое do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	целое priority, quantum, cpu;
	целое proc_nr;
	целое r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	если (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		вернуть EINVAL;
	}

	если (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		вернуть EINVAL;

	p = proc_addr(proc_nr);

	если ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		если((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			вернуть r;
		p->p_scheduler = NULL;
	} иначе {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	вернуть(OK);
}
