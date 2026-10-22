/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಆದರೆ SPROFILE

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ "watchdog.h"

ಅಕ್ಷರ sprof_sample_ಬಫರ್[SAMPLE_ಬಫರ್_ಗಾತ್ರ];

/* Function prototype for the profiling clock handler. */ 
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ಸ್ಥಿರ irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
ಶೂನ್ಯ init_profile_clock(u32_t freq)
{
  ಪೂರ್ಣಾಂಕ irq;

  ಆದರೆ((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
ಶೂನ್ಯ stop_profile_clock(ಶೂನ್ಯ)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ಸ್ಥಿರ ಶೂನ್ಯ sprof_save_sample(ರಚನೆ proc * p, ಶೂನ್ಯ * pc)
{
	ರಚನೆ sprof_sample *s;

	s = (ರಚನೆ sprof_sample *) (sprof_sample_ಬಫರ್ + sprof_info.mem_ಬಳಸಿದ);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_ಬಳಸಿದ += ಗಾತ್ರ(ರಚನೆ sprof_sample);
}

ಸ್ಥಿರ ಶೂನ್ಯ sprof_save_proc(ರಚನೆ proc * p)
{
	ರಚನೆ sprof_proc * s;

	s = (ರಚನೆ sprof_proc *) (sprof_sample_ಬಫರ್ + sprof_info.mem_ಬಳಸಿದ);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_ಬಳಸಿದ += ಗಾತ್ರ(ರಚನೆ sprof_proc);
}

ಸ್ಥಿರ ಶೂನ್ಯ profile_sample(ರಚನೆ proc * p, ಶೂನ್ಯ * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ಆದರೆ (!sprofiling || sprof_info.mem_ಬಳಸಿದ == -1)
	  ಹಿಂತಿರುಗಿ;

  /* Check if enough memory available before writing sample. */
  ಆದರೆ (sprof_info.mem_ಬಳಸಿದ + ಗಾತ್ರ(sprof_info) +
		  2*ಗಾತ್ರ(ರಚನೆ sprof_sample) +
		  2*ಗಾತ್ರ(ರಚನೆ sprof_sample) > sprof_mem_ಗಾತ್ರ) {
	sprof_info.mem_ಬಳಸಿದ = -1;
	ಹಿಂತಿರುಗಿ;
  }

  /* Runnable system process? */
  ಆದರೆ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ಆದರೆ (!(p->p_misc_flags & MF_SPROF_ನೋಡಿದ)) {
		p->p_misc_flags |= MF_SPROF_ನೋಡಿದ;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ಇಲ್ಲದಿದ್ದರೆ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.ಒಟ್ಟು_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ profile_clock_handler(irq_hook_t *hook)
{
  ರಚನೆ proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (ಶೂನ್ಯ *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ಹಿಂತಿರುಗಿ(1);                                    /* reenable interrupts */
}

ಶೂನ್ಯ nmi_sprofile_handler(ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು)
{
	ರಚನೆ proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ಆದರೆ (nmi_ಒಳ_kernel(ಚೌಕಟ್ಟು)) {
		ರಚನೆ proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ಆದರೆ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.ಒಟ್ಟು_samples++;
			ಹಿಂತಿರುಗಿ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (ಶೂನ್ಯ *) ಚೌಕಟ್ಟು->pc);
	}
	ಇಲ್ಲದಿದ್ದರೆ
		profile_sample(p, (ಶೂನ್ಯ *) ಚೌಕಟ್ಟು->pc);
}

#ಆದರೆ_ಅಂತ್ಯ /* SPROFILE */
