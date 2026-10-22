#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ចំនួនគត់ do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	ចំនួនគត់ priority, quantum, cpu;
	ចំនួនគត់ proc_nr;
	ចំនួនគត់ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ប្រសិនបើ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ត្រឡប់ EINVAL;
	}

	ប្រសិនបើ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ត្រឡប់ EINVAL;

	p = proc_addr(proc_nr);

	ប្រសិនបើ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ប្រសិនបើ((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ត្រឡប់ r;
		p->p_scheduler = NULL;
	} បើមិនដូច្នេះ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ត្រឡប់(OK);
}
