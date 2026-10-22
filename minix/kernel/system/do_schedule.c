#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
పూర్ణాంకం do_కాలపట్టిక(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	నిర్మాణం proc *p;
	పూర్ణాంకం proc_nr;
	పూర్ణాంకం ప్రాధాన్యత, కాలాంశం, cpu, niced;

	ఒకవేళ (!isokendpt(m_ptr->m_lsys_krn_కాలపట్టిక.endpoint, &proc_nr))
		తిరిగి EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ఒకవేళ (caller != p->p_scheduler)
		తిరిగి(EPERM);

	/* Try to schedule the process. */
	ప్రాధాన్యత = m_ptr->m_lsys_krn_కాలపట్టిక.ప్రాధాన్యత;
	కాలాంశం = m_ptr->m_lsys_krn_కాలపట్టిక.కాలాంశం;
	cpu = m_ptr->m_lsys_krn_కాలపట్టిక.cpu;
	niced = !!(m_ptr->m_lsys_krn_కాలపట్టిక.niced);

	తిరిగి sched_proc(p, ప్రాధాన్యత, కాలాంశం, cpu, niced);
}
