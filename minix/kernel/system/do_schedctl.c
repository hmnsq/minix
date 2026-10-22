#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
පූර්ණ_සංඛ්‍යා do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	පූර්ණ_සංඛ්‍යා priority, quantum, cpu;
	පූර්ණ_සංඛ්‍යා proc_nr;
	පූර්ණ_සංඛ්‍යා r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	නම් (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ආපසු EINVAL;
	}

	නම් (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ආපසු EINVAL;

	p = proc_addr(proc_nr);

	නම් ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		නම්((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ආපසු r;
		p->p_scheduler = NULL;
	} නැතහොත් {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ආපසු(OK);
}
