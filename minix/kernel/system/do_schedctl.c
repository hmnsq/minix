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
	もし (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		戻る EINVAL;
	}

	もし (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		戻る EINVAL;

	p = proc_addr(proc_nr);

	もし ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		もし((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			戻る r;
		p->p_scheduler = NULL;
	} 否则 {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	戻る(OK);
}
