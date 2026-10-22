#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
จำนวนเต็ม do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	จำนวนเต็ม priority, quantum, cpu;
	จำนวนเต็ม proc_nr;
	จำนวนเต็ม r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ถ้า (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		คืน EINVAL;
	}

	ถ้า (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		คืน EINVAL;

	p = proc_addr(proc_nr);

	ถ้า ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ถ้า((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			คืน r;
		p->p_scheduler = NULL;
	} มิฉะนั้น {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	คืน(OK);
}
