#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_schedctl(ସଂରଚନା proc * caller, message * m_ptr)
{
	ସଂରଚନା proc *p;
	uint32_t flags;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, quantum, cpu;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ଯଦି (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ଫେରନ୍ତୁ EINVAL;
	}

	ଯଦି (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ଫେରନ୍ତୁ EINVAL;

	p = proc_addr(proc_nr);

	ଯଦି ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ଯଦି((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			ଫେରନ୍ତୁ r;
		p->p_scheduler = ଖାଲି;
	} ଅନ୍ୟଥା {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ଫେରନ୍ତୁ(OK);
}
