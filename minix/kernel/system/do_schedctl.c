#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

/*===========================================================================*
 *			          do_schedctl			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_schedctl(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
	ರಚನೆ proc *p;
	uint32_t flags;
	ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಕಾಲಾಂಶ, cpu;
	ಪೂರ್ಣಾಂಕ proc_nr;
	ಪೂರ್ಣಾಂಕ r;

	/* check parameter validity */
	flags = m_ptr->m_lsys_krn_schedctl.flags;
	ಆದರೆ (flags & ~SCHEDCTL_FLAG_KERNEL) {
		ಬರೆ("do_schedctl: flags 0x%x invalid, caller=%d\n", 
			flags, caller - proc);
		ಹಿಂತಿರುಗಿ EINVAL;
	}

	ಆದರೆ (!isokendpt(m_ptr->m_lsys_krn_schedctl.endpoint, &proc_nr))
		ಹಿಂತಿರುಗಿ EINVAL;

	p = proc_addr(proc_nr);

	ಆದರೆ ((flags & SCHEDCTL_FLAG_KERNEL) == SCHEDCTL_FLAG_KERNEL) {
		/* the kernel becomes the scheduler and starts 
		 * scheduling the process.
		 */
		ಆದ್ಯತೆ = m_ptr->m_lsys_krn_schedctl.ಆದ್ಯತೆ;
		ಕಾಲಾಂಶ = m_ptr->m_lsys_krn_schedctl.ಕಾಲಾಂಶ;
		cpu = m_ptr->m_lsys_krn_schedctl.cpu;

		/* Try to schedule the process. */
		ಆದರೆ((r = sched_proc(p, ಆದ್ಯತೆ, ಕಾಲಾಂಶ, cpu, FALSE)) != OK)
			ಹಿಂತಿರುಗಿ r;
		p->p_scheduler = ಖಾಲಿ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		/* the caller becomes the scheduler */
		p->p_scheduler = caller;
	}

	ಹಿಂತಿರುಗಿ(OK);
}
