#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
정수 do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	정수 priority, quantum, cpu;
	정수 proc_nr;
	정수 r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	만약 (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		반환 EINVAL;
	}

	만약 (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		반환 EINVAL;

	p = proc_addr(proc_nr);

	만약 ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		만약((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			반환 r;
		p->p_scheduler = NULL;
	} 아니면 {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	반환(OK);
}
