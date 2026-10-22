#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/endpoint.h>
#അന്തര്ഭാവ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_schedule(ഘടന proc * caller, message * m_ptr)
{
	ഘടന proc *p;
	പൂർണ്ണസംഖ്യ proc_nr;
	പൂർണ്ണസംഖ്യ priority, quantum, cpu, niced;

	എങ്കിൽ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		തിരികെ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	എങ്കിൽ (caller != p->p_scheduler)
		തിരികെ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	തിരികെ sched_proc(p, priority, quantum, cpu, niced);
}
