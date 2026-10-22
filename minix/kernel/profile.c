/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଯଦି SPROFILE

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ "watchdog.h"

ଅକ୍ଷର sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ସ୍ଥିର irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
ଶୂନ୍ୟ init_profile_clock(u32_t freq)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq;

  ଯଦି((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
ଶୂନ୍ୟ stop_profile_clock(ଶୂନ୍ୟ)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ସ୍ଥିର ଶୂନ୍ୟ sprof_save_sample(ସଂରଚନା proc * p, ଶୂନ୍ୟ * pc)
{
	ସଂରଚନା sprof_sample *s;

	s = (ସଂରଚନା sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += ଆକାର(ସଂରଚନା sprof_sample);
}

ସ୍ଥିର ଶୂନ୍ୟ sprof_save_proc(ସଂରଚନା proc * p)
{
	ସଂରଚନା sprof_proc * s;

	s = (ସଂରଚନା sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += ଆକାର(ସଂରଚନା sprof_proc);
}

ସ୍ଥିର ଶୂନ୍ୟ profile_sample(ସଂରଚନା proc * p, ଶୂନ୍ୟ * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ଯଦି (!sprofiling || sprof_info.mem_used == -1)
	  ଫେରନ୍ତୁ;

  /* Check if enough memory available before writing sample. */
  ଯଦି (sprof_info.mem_used + ଆକାର(sprof_info) +
		  2*ଆକାର(ସଂରଚନା sprof_sample) +
		  2*ଆକାର(ସଂରଚନା sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	ଫେରନ୍ତୁ;
  }

  /* Runnable system process? */
  ଯଦି (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ଅନ୍ୟଥା ଯଦି (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ଯଦି (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ଅନ୍ୟଥା {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା profile_clock_handler(irq_hook_t *hook)
{
  ସଂରଚନା proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (ଶୂନ୍ୟ *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ଫେରନ୍ତୁ(1);                                    /* reenable interrupts */
}

ଶୂନ୍ୟ nmi_sprofile_handler(ସଂରଚନା nmi_frame * frame)
{
	ସଂରଚନା proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ଯଦି (nmi_in_kernel(frame)) {
		ସଂରଚନା proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ଯଦି (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			ଫେରନ୍ତୁ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (ଶୂନ୍ୟ *) frame->pc);
	}
	ଅନ୍ୟଥା
		profile_sample(p, (ଶୂନ୍ୟ *) frame->pc);
}

#endif /* SPROFILE */
