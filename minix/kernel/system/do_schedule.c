#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>
#ضمن "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحيح do_مهالوېش(بنية proc * caller, پیغام * m_ptr)
{
	بنية proc *p;
	صحيح proc_nr;
	صحيح لومړیتوب, وخت_ټوټه, cpu, niced;

	إذا (!isokendpt(m_ptr->m_lsys_krn_مهالوېش.endpoint, &proc_nr))
		أرجع EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	إذا (caller != p->p_scheduler)
		أرجع(EPERM);

	/* Try to schedule the process. */
	لومړیتوب = m_ptr->m_lsys_krn_مهالوېش.لومړیتوب;
	وخت_ټوټه = m_ptr->m_lsys_krn_مهالوېش.وخت_ټوټه;
	cpu = m_ptr->m_lsys_krn_مهالوېش.cpu;
	niced = !!(m_ptr->m_lsys_krn_مهالوېش.niced);

	أرجع sched_proc(p, لومړیتوب, وخت_ټوټه, cpu, niced);
}
