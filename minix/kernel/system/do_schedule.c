#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
पूर्णाङ्क do_कालसारणी(संरचना proc * caller, सन्देशः * m_ptr)
{
	संरचना proc *p;
	पूर्णाङ्क proc_nr;
	पूर्णाङ्क प्राथमिकता, कालांशः, cpu, niced;

	यदि (!isokendpt(m_ptr->m_lsys_krn_कालसारणी.endpoint, &proc_nr))
		प्रत्यागम EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	यदि (caller != p->p_scheduler)
		प्रत्यागम(EPERM);

	/* Try to schedule the process. */
	प्राथमिकता = m_ptr->m_lsys_krn_कालसारणी.प्राथमिकता;
	कालांशः = m_ptr->m_lsys_krn_कालसारणी.कालांशः;
	cpu = m_ptr->m_lsys_krn_कालसारणी.cpu;
	niced = !!(m_ptr->m_lsys_krn_कालसारणी.niced);

	प्रत्यागम sched_proc(p, प्राथमिकता, कालांशः, cpu, niced);
}
