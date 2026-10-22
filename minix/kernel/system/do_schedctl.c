#include "kernel/system.h"
#include <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ကိန်းပြည့် do_schedctl(sa_nracanaa proc * caller, message * m_ptr)
{
	sa_nracanaa proc *p;
	uint32_t flags;
	ကိန်းပြည့် priority, quantum, cpu;
	ကိန်းပြည့် proc_nr;
	ကိန်းပြည့် r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	အကယ်၍ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ပြန်ပေး EINVAL;
	}

	အကယ်၍ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ပြန်ပေး EINVAL;

	p = proc_addr(proc_nr);

	အကယ်၍ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		အကယ်၍((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ပြန်ပေး r;
		p->p_scheduler = NULL;
	} မဟုတ်ပါက {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ပြန်ပေး(OK);
}
