#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>
#ضمن "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحيح do_زمانبندی(بنية proc * caller, پیام * m_ptr)
{
	بنية proc *p;
	صحيح proc_nr;
	صحيح اولویت, برش_زمانی, cpu, niced;

	إذا (!isokendpt(m_ptr->m_lsys_krn_زمانبندی.endpoint, &proc_nr))
		أرجع EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	إذا (caller != p->p_scheduler)
		أرجع(EPERM);

	/* Try to schedule the process. */
	اولویت = m_ptr->m_lsys_krn_زمانبندی.اولویت;
	برش_زمانی = m_ptr->m_lsys_krn_زمانبندی.برش_زمانی;
	cpu = m_ptr->m_lsys_krn_زمانبندی.cpu;
	niced = !!(m_ptr->m_lsys_krn_زمانبندی.niced);

	أرجع sched_proc(p, اولویت, برش_زمانی, cpu, niced);
}
