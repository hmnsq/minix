#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
पूर्णांक do_schedctl(संरचना proc * caller, message * m_ptr)
{
	संरचना proc *p;
	uint32_t flags;
	पूर्णांक priority, quantum, cpu;
	पूर्णांक proc_nr;
	पूर्णांक r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	जर (flags & ~SCHEDCTL_FLAG_KERNEL) {
		म_लिखो("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		परत EINVAL;
	}

	जर (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		परत EINVAL;

	p = proc_addr(proc_nr);

	जर ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		जर((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			परत r;
		p->p_scheduler = रिक्त;
	} अन्यथा {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	परत(OK);
}
