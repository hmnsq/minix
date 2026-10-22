#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>
#ଅନ୍ତର୍ଭାଵ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_schedule(ସଂରଚନା proc * caller, message * m_ptr)
{
	ସଂରଚନା proc *p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, quantum, cpu, niced;

	ଯଦି (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr))
		ଫେରନ୍ତୁ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ଯଦି (caller != p->p_scheduler)
		ଫେରନ୍ତୁ(EPERM);

	/* Try to schedule the process. */
	priority = m_ptr->m_lsys_krn_schedule.priority;
	quantum = m_ptr->m_lsys_krn_schedule.quantum;
	cpu = m_ptr->m_lsys_krn_schedule.cpu;
	niced = !!(m_ptr->m_lsys_krn_schedule.niced);

	ଫେରନ୍ତୁ sched_proc(p, priority, quantum, cpu, niced);
}
