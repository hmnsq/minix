#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
પૂર્ણાંક do_schedctl(સંરચના proc * caller, message * m_ptr)
{
	સંરચના proc *p;
	uint32_t flags;
	પૂર્ણાંક priority, quantum, cpu;
	પૂર્ણાંક proc_nr;
	પૂર્ણાંક r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	જો (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		પાછા EINVAL;
	}

	જો (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		પાછા EINVAL;

	p = proc_addr(proc_nr);

	જો ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		જો((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			પાછા r;
		p->p_scheduler = ખાલી;
	} નહીંતર {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	પાછા(OK);
}
