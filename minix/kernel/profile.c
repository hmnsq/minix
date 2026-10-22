/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#അന്തര്ഭാവ "kernel/kernel.h"

#എങ്കിൽ SPROFILE

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ "watchdog.h"

അക്ഷരം sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
സ്ഥിതം പൂർണ്ണസംഖ്യ profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
സ്ഥിതം irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
ശൂന്യം init_profile_clock(u32_t freq)
{
  പൂർണ്ണസംഖ്യ irq;

  എങ്കിൽ((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
ശൂന്യം stop_profile_clock(ശൂന്യം)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

സ്ഥിതം ശൂന്യം sprof_save_sample(ഘടന proc * p, ശൂന്യം * pc)
{
	ഘടന sprof_sample *s;

	s = (ഘടന sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += വലുപ്പം(ഘടന sprof_sample);
}

സ്ഥിതം ശൂന്യം sprof_save_proc(ഘടന proc * p)
{
	ഘടന sprof_proc * s;

	s = (ഘടന sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += വലുപ്പം(ഘടന sprof_proc);
}

സ്ഥിതം ശൂന്യം profile_sample(ഘടന proc * p, ശൂന്യം * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  എങ്കിൽ (!sprofiling || sprof_info.mem_used == -1)
	  തിരികെ;

  /* Check if enough memory available before writing sample. */
  എങ്കിൽ (sprof_info.mem_used + വലുപ്പം(sprof_info) +
		  2*വലുപ്പം(ഘടന sprof_sample) +
		  2*വലുപ്പം(ഘടന sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	തിരികെ;
  }

  /* Runnable system process? */
  എങ്കിൽ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  അല്ലെങ്കിൽ എങ്കിൽ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	എങ്കിൽ (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } അല്ലെങ്കിൽ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
സ്ഥിതം പൂർണ്ണസംഖ്യ profile_clock_handler(irq_hook_t *hook)
{
  ഘടന proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (ശൂന്യം *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  തിരികെ(1);                                    /* reenable interrupts */
}

ശൂന്യം nmi_sprofile_handler(ഘടന nmi_frame * frame)
{
	ഘടന proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	എങ്കിൽ (nmi_in_kernel(frame)) {
		ഘടന proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		എങ്കിൽ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			തിരികെ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (ശൂന്യം *) frame->pc);
	}
	അല്ലെങ്കിൽ
		profile_sample(p, (ശൂന്യം *) frame->pc);
}

#endif /* SPROFILE */
