#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_schedctl(ഘടന proc * caller, message * m_ptr)
{
	ഘടന proc *p;
	uint32_t flags;
	പൂർണ്ണസംഖ്യ priority, quantum, cpu;
	പൂർണ്ണസംഖ്യ proc_nr;
	പൂർണ്ണസംഖ്യ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	എങ്കിൽ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		തിരികെ EINVAL;
	}

	എങ്കിൽ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		തിരികെ EINVAL;

	p = proc_addr(proc_nr);

	എങ്കിൽ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		എങ്കിൽ((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			തിരികെ r;
		p->p_scheduler = ശൂന്യ;
	} അല്ലെങ്കിൽ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	തിരികെ(OK);
}
