#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>
#ضمن "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحيح do_وقت_جدول(بنية proc * caller, پيغام * m_ptr)
{
	بنية proc *p;
	صحيح proc_nr;
	صحيح ترجيح, وقتي_حصو, cpu, niced;

	إذا (!isokendpt(m_ptr->m_lsys_krn_وقت_جدول.endpoint, &proc_nr))
		أرجع EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	إذا (caller != p->p_scheduler)
		أرجع(EPERM);

	/* Try to schedule the process. */
	ترجيح = m_ptr->m_lsys_krn_وقت_جدول.ترجيح;
	وقتي_حصو = m_ptr->m_lsys_krn_وقت_جدول.وقتي_حصو;
	cpu = m_ptr->m_lsys_krn_وقت_جدول.cpu;
	niced = !!(m_ptr->m_lsys_krn_وقت_جدول.niced);

	أرجع sched_proc(p, ترجيح, وقتي_حصو, cpu, niced);
}
