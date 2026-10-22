#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
整数 do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	整数 priority, quantum, cpu;
	整数 proc_nr;
	整数 r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	如果 (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		返回 EINVAL;
	}

	如果 (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		返回 EINVAL;

	p = proc_addr(proc_nr);

	如果 ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		如果((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			返回 r;
		p->p_scheduler = NULL;
	} 否则 {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	返回(OK);
}
