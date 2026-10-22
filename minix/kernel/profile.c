/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#அந்தர்பாவ "kernel/kernel.h"

#எனில் SPROFILE

#அந்தர்பாவ <string.h>
#அந்தர்பாவ "watchdog.h"

எழுத்து sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
நிலையான முழு profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
நிலையான irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
வெறுமை init_profile_clock(u32_t freq)
{
  முழு irq;

  எனில்((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
வெறுமை stop_profile_clock(வெறுமை)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

நிலையான வெறுமை sprof_save_sample(கட்டமைப்பு proc * p, வெறுமை * pc)
{
	கட்டமைப்பு sprof_sample *s;

	s = (கட்டமைப்பு sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += அளவு(கட்டமைப்பு sprof_sample);
}

நிலையான வெறுமை sprof_save_proc(கட்டமைப்பு proc * p)
{
	கட்டமைப்பு sprof_proc * s;

	s = (கட்டமைப்பு sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += அளவு(கட்டமைப்பு sprof_proc);
}

நிலையான வெறுமை profile_sample(கட்டமைப்பு proc * p, வெறுமை * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  எனில் (!sprofiling || sprof_info.mem_used == -1)
	  திரும்பு;

  /* Check if enough memory available before writing sample. */
  எனில் (sprof_info.mem_used + அளவு(sprof_info) +
		  2*அளவு(கட்டமைப்பு sprof_sample) +
		  2*அளவு(கட்டமைப்பு sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	திரும்பு;
  }

  /* Runnable system process? */
  எனில் (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  இல்லையெனில் எனில் (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	எனில் (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } இல்லையெனில் {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
நிலையான முழு profile_clock_handler(irq_hook_t *hook)
{
  கட்டமைப்பு proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (வெறுமை *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  திரும்பு(1);                                    /* reenable interrupts */
}

வெறுமை nmi_sprofile_handler(கட்டமைப்பு nmi_frame * frame)
{
	கட்டமைப்பு proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	எனில் (nmi_in_kernel(frame)) {
		கட்டமைப்பு proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		எனில் (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			திரும்பு;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (வெறுமை *) frame->pc);
	}
	இல்லையெனில்
		profile_sample(p, (வெறுமை *) frame->pc);
}

#endif /* SPROFILE */
