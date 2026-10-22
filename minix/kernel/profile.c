/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#شامل "kernel/kernel.h"

#اگر SPROFILE

#شامل <string.h>
#شامل "watchdog.h"

نویسه sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
ایستا صحیح profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ایستا irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
تهی init_profile_clock(u32_t freq)
{
  صحیح irq;

  اگر((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
تهی stop_profile_clock(تهی)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ایستا تهی sprof_save_sample(ساختار proc * p, تهی * pc)
{
	ساختار sprof_sample *s;

	s = (ساختار sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += اندازه(ساختار sprof_sample);
}

ایستا تهی sprof_save_proc(ساختار proc * p)
{
	ساختار sprof_proc * s;

	s = (ساختار sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += اندازه(ساختار sprof_proc);
}

ایستا تهی profile_sample(ساختار proc * p, تهی * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  اگر (!sprofiling || sprof_info.mem_used == -1)
	  بازگشت;

  /* Check if enough memory available before writing sample. */
  اگر (sprof_info.mem_used + اندازه(sprof_info) +
		  2*اندازه(ساختار sprof_sample) +
		  2*اندازه(ساختار sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	بازگشت;
  }

  /* Runnable system process? */
  اگر (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  وگرنه اگر (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	اگر (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } وگرنه {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ایستا صحیح profile_clock_handler(irq_hook_t *hook)
{
  ساختار proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (تهی *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  بازگشت(1);                                    /* reenable interrupts */
}

تهی nmi_sprofile_handler(ساختار nmi_frame * frame)
{
	ساختار proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	اگر (nmi_in_kernel(frame)) {
		ساختار proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		اگر (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			بازگشت;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (تهی *) frame->pc);
	}
	وگرنه
		profile_sample(p, (تهی *) frame->pc);
}

#endif /* SPROFILE */
