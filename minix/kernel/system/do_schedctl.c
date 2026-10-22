#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ціле do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	ціле priority, quantum, cpu;
	ціле proc_nr;
	ціле r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	якщо (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		повернути EINVAL;
	}

	якщо (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		повернути EINVAL;

	p = proc_addr(proc_nr);

	якщо ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		якщо((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			повернути r;
		p->p_scheduler = NULL;
	} інакше {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	повернути(OK);
}
