#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ཧྲིལ do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	ཧྲིལ priority, quantum, cpu;
	ཧྲིལ proc_nr;
	ཧྲིལ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	གལ་སྲིད (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ལོག EINVAL;
	}

	གལ་སྲིད (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ལོག EINVAL;

	p = proc_addr(proc_nr);

	གལ་སྲིད ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		གལ་སྲིད((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ལོག r;
		p->p_scheduler = NULL;
	} དེ་མིན {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ལོག(OK);
}
