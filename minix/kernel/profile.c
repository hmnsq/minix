/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#અન્તર્ભાવ "kernel/kernel.h"

#જો SPROFILE

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ "watchdog.h"

અક્ષર sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
સ્થિર પૂર્ણાંક profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
સ્થિર irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
શૂન્ય init_profile_clock(u32_t freq)
{
  પૂર્ણાંક irq;

  જો((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
શૂન્ય stop_profile_clock(શૂન્ય)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

સ્થિર શૂન્ય sprof_save_sample(સંરચના proc * p, શૂન્ય * pc)
{
	સંરચના sprof_sample *s;

	s = (સંરચના sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += કદ(સંરચના sprof_sample);
}

સ્થિર શૂન્ય sprof_save_proc(સંરચના proc * p)
{
	સંરચના sprof_proc * s;

	s = (સંરચના sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += કદ(સંરચના sprof_proc);
}

સ્થિર શૂન્ય profile_sample(સંરચના proc * p, શૂન્ય * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  જો (!sprofiling || sprof_info.mem_used == -1)
	  પાછા;

  /* Check if enough memory available before writing sample. */
  જો (sprof_info.mem_used + કદ(sprof_info) +
		  2*કદ(સંરચના sprof_sample) +
		  2*કદ(સંરચના sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	પાછા;
  }

  /* Runnable system process? */
  જો (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  નહીંતર જો (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	જો (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } નહીંતર {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
સ્થિર પૂર્ણાંક profile_clock_handler(irq_hook_t *hook)
{
  સંરચના proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (શૂન્ય *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  પાછા(1);                                    /* reenable interrupts */
}

શૂન્ય nmi_sprofile_handler(સંરચના nmi_frame * frame)
{
	સંરચના proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	જો (nmi_in_kernel(frame)) {
		સંરચના proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		જો (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			પાછા;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (શૂન્ય *) frame->pc);
	}
	નહીંતર
		profile_sample(p, (શૂન્ય *) frame->pc);
}

#endif /* SPROFILE */
