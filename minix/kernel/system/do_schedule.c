#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>
#ضمن "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحيح do_ویلا_سارنی(بنية proc * caller, سنیہا * m_ptr)
{
	بنية proc *p;
	صحيح proc_nr;
	صحيح ترجیح, ویلا_حصہ, cpu, niced;

	إذا (!isokendpt(m_ptr->m_lsys_krn_ویلا_سارنی.endpoint, &proc_nr))
		أرجع EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	إذا (caller != p->p_scheduler)
		أرجع(EPERM);

	/* Try to schedule the process. */
	ترجیح = m_ptr->m_lsys_krn_ویلا_سارنی.ترجیح;
	ویلا_حصہ = m_ptr->m_lsys_krn_ویلا_سارنی.ویلا_حصہ;
	cpu = m_ptr->m_lsys_krn_ویلا_سارنی.cpu;
	niced = !!(m_ptr->m_lsys_krn_ویلا_سارنی.niced);

	أرجع sched_proc(p, ترجیح, ویلا_حصہ, cpu, niced);
}
