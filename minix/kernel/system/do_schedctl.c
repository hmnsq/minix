#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
पूर्णाङ्क do_schedctl(संरचना proc * caller, सन्देशः * m_ptr)
{
	संरचना proc *p;
	uint32_t flags;
	पूर्णाङ्क प्राथमिकता, कालांशः, cpu;
	पूर्णाङ्क proc_nr;
	पूर्णाङ्क r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	यदि (flags & ~SCHEDCTL_FLAG_KERNEL) {
		प्रारूप_लिख("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		प्रत्यागम EINVAL;
	}

	यदि (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		प्रत्यागम EINVAL;

	p = proc_addr(proc_nr);

	यदि ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		प्राथमिकता = m_ptr->m_lsys_krn_schedctl.प्राथमिकता;
		कालांशः = m_ptr->m_lsys_krn_schedctl.कालांशः;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		यदि((r = sched_proc(p, प्राथमिकता, कालांशः, cpu, FALSE)) != OK)
			प्रत्यागम r;
		p->p_scheduler = रिक्त;
	} अन्यथा {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	प्रत्यागम(OK);
}
