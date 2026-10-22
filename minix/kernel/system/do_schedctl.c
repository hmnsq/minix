#समावेश "kernel/system.h"
#समावेश <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
पूर्णांक do_schedctl(काष्ठा proc * caller, message * m_ptr)
{
	काष्ठा proc *p;
	uint32_t flags;
	पूर्णांक priority, quantum, cpu;
	पूर्णांक proc_nr;
	पूर्णांक r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	अगर (flags & ~SCHEDCTL_FLAG_KERNEL) {
		म_लिखो("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		वापस EINVAL;
	}

	अगर (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		वापस EINVAL;

	p = proc_addr(proc_nr);

	अगर ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		अगर((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			वापस r;
		p->p_scheduler = शून्य;
	} अन्यथा {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	वापस(OK);
}
