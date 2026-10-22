#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	ຈຳນວນເຕັມ priority, quantum, cpu;
	ຈຳນວນເຕັມ proc_nr;
	ຈຳນວນເຕັມ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ຖ້າ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ສົ່ງຄືນ EINVAL;
	}

	ຖ້າ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ສົ່ງຄືນ EINVAL;

	p = proc_addr(proc_nr);

	ຖ້າ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ຖ້າ((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ສົ່ງຄືນ r;
		p->p_scheduler = NULL;
	} ບໍ່ດັ່ງນັ້ນ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ສົ່ງຄືນ(OK);
}
