#شامل "kernel/system.h"
#شامل <minix/endpoint.h>
#شامل "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
صحیح do_جدول_وقت(ساخت proc * caller, پیغام * m_ptr)
{
	ساخت proc *p;
	صحیح proc_nr;
	صحیح ترجیح, وقتی_حصہ, cpu, niced;

	اگر (!isokendpt(m_ptr->m_lsys_krn_جدول_وقت.endpoint, &proc_nr))
		واپس EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	اگر (caller != p->p_scheduler)
		واپس(EPERM);

	/* Try to schedule the process. */
	ترجیح = m_ptr->m_lsys_krn_جدول_وقت.ترجیح;
	وقتی_حصہ = m_ptr->m_lsys_krn_جدول_وقت.وقتی_حصہ;
	cpu = m_ptr->m_lsys_krn_جدول_وقت.cpu;
	niced = !!(m_ptr->m_lsys_krn_جدول_وقت.niced);

	واپس sched_proc(p, ترجیح, وقتی_حصہ, cpu, niced);
}
