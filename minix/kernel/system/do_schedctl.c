#شامل "kernel/system.h"
#شامل <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
صحیح do_schedctl(ساخت proc * caller, پیغام * m_ptr)
{
	ساخت proc *p;
	uint32_t flags;
	صحیح ترجیح, وقتی_حصہ, cpu;
	صحیح proc_nr;
	صحیح r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	اگر (flags & ~SCHEDCTL_FLAG_KERNEL) {
		لکھو("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		واپس EINVAL;
	}

	اگر (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		واپس EINVAL;

	p = proc_addr(proc_nr);

	اگر ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		ترجیح = m_ptr->m_lsys_krn_schedctl.ترجیح;
		وقتی_حصہ = m_ptr->m_lsys_krn_schedctl.وقتی_حصہ;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		اگر((r = sched_proc(p, ترجیح, وقتی_حصہ, cpu, FALSE)) != OK)
			واپس r;
		p->p_scheduler = تہی;
	} ورنہ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	واپس(OK);
}
