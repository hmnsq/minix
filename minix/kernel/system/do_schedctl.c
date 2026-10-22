#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
पूर्णाङ्क do_schedctl(संरचना proc * caller, message * m_ptr)
{
	संरचना proc *p;
	uint32_t flags;
	पूर्णाङ्क priority, quantum, cpu;
	पूर्णाङ्क proc_nr;
	पूर्णाङ्क r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	यदि (flags & ~SCHEDCTL_FLAG_KERNEL) {
		म_लिखो("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		फर्काऊ EINVAL;
	}

	यदि (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		फर्काऊ EINVAL;

	p = proc_addr(proc_nr);

	यदि ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		यदि((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			फर्काऊ r;
		p->p_scheduler = रिक्त;
	} अन्यथा {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	फर्काऊ(OK);
}
