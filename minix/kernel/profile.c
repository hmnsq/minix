/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#अन्तर्भाव "kernel/kernel.h"

#जर SPROFILE

#अन्तर्भाव <string.h>
#अन्तर्भाव "watchdog.h"

अक्षर sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
स्थिर पूर्णांक profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
स्थिर irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
शून्य init_profile_clock(u32_t freq)
{
  पूर्णांक irq;

  जर((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
शून्य stop_profile_clock(शून्य)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

स्थिर शून्य sprof_save_sample(संरचना proc * p, शून्य * pc)
{
	संरचना sprof_sample *s;

	s = (संरचना sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += आकार(संरचना sprof_sample);
}

स्थिर शून्य sprof_save_proc(संरचना proc * p)
{
	संरचना sprof_proc * s;

	s = (संरचना sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	म_नकल(s->name, p->p_name);

	sprof_info.mem_used += आकार(संरचना sprof_proc);
}

स्थिर शून्य profile_sample(संरचना proc * p, शून्य * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  जर (!sprofiling || sprof_info.mem_used == -1)
	  परत;

  /* Check if enough memory available before writing sample. */
  जर (sprof_info.mem_used + आकार(sprof_info) +
		  2*आकार(संरचना sprof_sample) +
		  2*आकार(संरचना sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	परत;
  }

  /* Runnable system process? */
  जर (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  अन्यथा जर (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	जर (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } अन्यथा {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
स्थिर पूर्णांक profile_clock_handler(irq_hook_t *hook)
{
  संरचना proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (शून्य *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  परत(1);                                    /* reenable interrupts */
}

शून्य nmi_sprofile_handler(संरचना nmi_frame * frame)
{
	संरचना proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	जर (nmi_in_kernel(frame)) {
		संरचना proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		जर (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			परत;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (शून्य *) frame->pc);
	}
	अन्यथा
		profile_sample(p, (शून्य *) frame->pc);
}

#endif /* SPROFILE */
