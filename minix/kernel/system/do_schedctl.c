#شامل "kernel/system.h"
#شامل <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
صحیح do_schedctl(ساختار proc * caller, message * m_ptr)
{
	ساختار proc *p;
	uint32_t flags;
	صحیح priority, quantum, cpu;
	صحیح proc_nr;
	صحیح r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	اگر (flags & ~SCHEDCTL_FLAG_KERNEL) {
		printf("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		بازگشت EINVAL;
	}

	اگر (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		بازگشت EINVAL;

	p = proc_addr(proc_nr);

	اگر ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		priority = m_ptr->m_lsys_krn_schedctl.priority;
		quantum = m_ptr->m_lsys_krn_schedctl.quantum;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		اگر((r = sched_proc(p, priority, quantum, cpu, FALSE)) != OK)
			بازگشت r;
		p->p_scheduler = پوچ;
	} وگرنه {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	بازگشت(OK);
}
