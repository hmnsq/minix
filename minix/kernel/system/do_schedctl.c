#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
số_nguyên do_schedctl(struct proc * caller, message * m_ptr)
{
	struct proc *p;
	uint32_t flags;
	số_nguyên priority, quantum, cpu;
	số_nguyên proc_nr;
	số_nguyên r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	nếu (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		trả_về EINVAL;
	}

	nếu (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		trả_về EINVAL;

	p = proc_addr(proc_nr);

	nếu ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		nếu((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			trả_về r;
		p->p_scheduler = NULL;
	} không_thì {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	trả_về(OK);
}
