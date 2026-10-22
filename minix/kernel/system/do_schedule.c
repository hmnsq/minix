#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"

/*===========================================================================*
 *				do_schedule				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_ವೇಳಾಪಟ್ಟಿ(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ರಚನೆ proc *p;
	ಪೂರ್ಣಾಂಕ proc_nr;
	ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಕಾಲಾಂಶ, cpu, niced;

	ಆದರೆ (!isokendpt(m_ptr->m_lsys_krn_ವೇಳಾಪಟ್ಟಿ.endpoint, &proc_nr))
		ಹಿಂತಿರುಗಿ EINVAL;

	p = proc_addr(proc_nr);

	/* Only this process' scheduler can schedule it */
	ಆದರೆ (caller != p->p_scheduler)
		ಹಿಂತಿರುಗಿ(EPERM);

	/* Try to schedule the process. */
	ಆದ್ಯತೆ = m_ptr->m_lsys_krn_ವೇಳಾಪಟ್ಟಿ.ಆದ್ಯತೆ;
	ಕಾಲಾಂಶ = m_ptr->m_lsys_krn_ವೇಳಾಪಟ್ಟಿ.ಕಾಲಾಂಶ;
	cpu = m_ptr->m_lsys_krn_ವೇಳಾಪಟ್ಟಿ.cpu;
	niced = !!(m_ptr->m_lsys_krn_ವೇಳಾಪಟ್ಟಿ.niced);

	ಹಿಂತಿರುಗಿ sched_proc(p, ಆದ್ಯತೆ, ಕಾಲಾಂಶ, cpu, niced);
}
