#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
puur_naa_nka do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	puur_naa_nka priority, quantum, cpu;
	puur_naa_nka proc_nr;
	puur_naa_nka r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ᮚᮦᮔ᮪ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ᮝᮜᮤ EINVAL;
	}

	ᮚᮦᮔ᮪ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ᮝᮜᮤ EINVAL;

	p = proc_addr(proc_nr);

	ᮚᮦᮔ᮪ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ᮚᮦᮔ᮪((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ᮝᮜᮤ r;
		p->p_scheduler = NULL;
	} ᮜᮤᮚ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ᮝᮜᮤ(OK);
}
