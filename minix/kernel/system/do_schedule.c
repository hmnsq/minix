#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_schedule(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
	ಸಂರಚನಾ proc *p;
	ಪೂರ್ಣಾಙ್ಕ proc_nr;
	ಪೂರ್ಣಾಙ್ಕ priority, quantum, cpu, niced;

	ಯದಿ (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ಯದಿ (caller != p->p_scheduler)
		ಪ್ರತ್ಯಾಗಮ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ಪ್ರತ್ಯಾಗಮ sched_proc(p, priority, quantum, cpu, niced);
}
