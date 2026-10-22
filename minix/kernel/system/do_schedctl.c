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
	ပေႃး (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ပွၵ်ႈ EINVAL;
	}

	ပေႃး (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ပွၵ်ႈ EINVAL;

	p = proc_addr(proc_nr);

	ပေႃး ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ပေႃး((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ပွၵ်ႈ r;
		p->p_scheduler = NULL;
	} ပေႃးဢမ်ႇ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ပွၵ်ႈ(OK);
}
