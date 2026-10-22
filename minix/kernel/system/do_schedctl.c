#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
పూర్ణాంకం do_schedctl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
	నిర్మాణం proc *p;
	uint32_t flags;
	పూర్ణాంకం ప్రాధాన్యత, కాలాంశం, cpu;
	పూర్ణాంకం proc_nr;
	పూర్ణాంకం r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ఒకవేళ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		రాయి("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		తిరిగి EINVAL;
	}

	ఒకవేళ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		తిరిగి EINVAL;

	p = proc_addr(proc_nr);

	ఒకవేళ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		ప్రాధాన్యత = m_ptr->m_lsys_krn_schedctl.ప్రాధాన్యత;
		కాలాంశం = m_ptr->m_lsys_krn_schedctl.కాలాంశం;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ఒకవేళ((r = sched_proc(p, ప్రాధాన్యత, కాలాంశం, cpu, FALSE)) != OK)
			తిరిగి r;
		p->p_scheduler = ఖాళీ;
	} లేకపోతే {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	తిరిగి(OK);
}
