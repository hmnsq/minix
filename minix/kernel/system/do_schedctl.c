#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
صحيح do_schedctl(بنية proc * caller, پيغام * m_ptr)
{
	بنية proc *p;
	uint32_t flags;
	صحيح ترجيح, وقتي_حصو, cpu;
	صحيح proc_nr;
	صحيح r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	إذا (flags & ~SCHEDCTL_FLAG_KERNEL) {
		لکو("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		أرجع EINVAL;
	}

	إذا (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		أرجع EINVAL;

	p = proc_addr(proc_nr);

	إذا ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		ترجيح = m_ptr->m_lsys_krn_schedctl.ترجيح;
		وقتي_حصو = m_ptr->m_lsys_krn_schedctl.وقتي_حصو;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		إذا((r = sched_proc(p, ترجيح, وقتي_حصو, cpu, FALSE)) != OK)
			أرجع r;
		p->p_scheduler = عدم;
	} وإلا {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	أرجع(OK);
}
