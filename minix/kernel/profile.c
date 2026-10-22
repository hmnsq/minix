/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಯದಿ SPROFILE

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ "watchdog.h"

ವರ್ಣ sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ಸ್ಥಿರ irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
ಶೂನ್ಯ init_profile_clock(u32_t freq)
{
  ಪೂರ್ಣಾಙ್ಕ irq;

  ಯದಿ((irq = arch_init_profile_clock(freq)) >= 0) {
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

ಸ್ಥಿರ ಶೂನ್ಯ sprof_save_sample(ಸಂರಚನಾ proc * p, ಶೂನ್ಯ * pc)
{
	ಸಂರಚನಾ sprof_sample *s;

	s = (ಸಂರಚನಾ sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += ಪರಿಮಾಣ(ಸಂರಚನಾ sprof_sample);
}

ಸ್ಥಿರ ಶೂನ್ಯ sprof_save_proc(ಸಂರಚನಾ proc * p)
{
	ಸಂರಚನಾ sprof_proc * s;

	s = (ಸಂರಚನಾ sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += ಪರಿಮಾಣ(ಸಂರಚನಾ sprof_proc);
}

ಸ್ಥಿರ ಶೂನ್ಯ profile_sample(ಸಂರಚನಾ proc * p, ಶೂನ್ಯ * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ಯದಿ (!sprofiling || sprof_info.mem_used == -1)
	  ಪ್ರತ್ಯಾಗಮ;

  /* Check if enough memory available before writing sample. */
  ಯದಿ (sprof_info.mem_used + ಪರಿಮಾಣ(sprof_info) +
		  2*ಪರಿಮಾಣ(ಸಂರಚನಾ sprof_sample) +
		  2*ಪರಿಮಾಣ(ಸಂರಚನಾ sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	ಪ್ರತ್ಯಾಗಮ;
  }

  /* Runnable system process? */
  ಯದಿ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ಅನ್ಯಥಾ ಯದಿ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ಯದಿ (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ಅನ್ಯಥಾ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ profile_clock_handler(irq_hook_t *hook)
{
  ಸಂರಚನಾ proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (ಶೂನ್ಯ *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ಪ್ರತ್ಯಾಗಮ(1);                                    /* reenable interrupts */
}

ಶೂನ್ಯ nmi_sprofile_handler(ಸಂರಚನಾ nmi_frame * frame)
{
	ಸಂರಚನಾ proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ಯದಿ (nmi_in_kernel(frame)) {
		ಸಂರಚನಾ proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ಯದಿ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			ಪ್ರತ್ಯಾಗಮ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (ಶೂನ್ಯ *) frame->pc);
	}
	ಅನ್ಯಥಾ
		profile_sample(p, (ಶೂನ್ಯ *) frame->pc);
}

#endif /* SPROFILE */
