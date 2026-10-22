/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#অন্তর্ভাব "kernel/kernel.h"

#যদি SPROFILE

#অন্তর্ভাব <string.h>
#অন্তর্ভাব "watchdog.h"

অক্ষর sprof_sample_বাফার[SAMPLE_বাফার_আকার];

/* Function prototype for the profiling clock handler. */ 
স্থির পূর্ণসংখ্যা profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
স্থির irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
শূন্য init_profile_clock(u32_t freq)
{
  পূর্ণসংখ্যা irq;

  যদি((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
শূন্য stop_profile_clock(শূন্য)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

স্থির শূন্য sprof_save_sample(কাঠামো proc * p, শূন্য * pc)
{
	কাঠামো sprof_sample *s;

	s = (কাঠামো sprof_sample *) (sprof_sample_বাফার + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += আকার(কাঠামো sprof_sample);
}

স্থির শূন্য sprof_save_proc(কাঠামো proc * p)
{
	কাঠামো sprof_proc * s;

	s = (কাঠামো sprof_proc *) (sprof_sample_বাফার + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += আকার(কাঠামো sprof_proc);
}

স্থির শূন্য profile_sample(কাঠামো proc * p, শূন্য * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  যদি (!sprofiling || sprof_info.mem_used == -1)
	  ফেরত;

  /* Check if enough memory available before writing sample. */
  যদি (sprof_info.mem_used + আকার(sprof_info) +
		  2*আকার(কাঠামো sprof_sample) +
		  2*আকার(কাঠামো sprof_sample) > sprof_mem_আকার) {
	sprof_info.mem_used = -1;
	ফেরত;
  }

  /* Runnable system process? */
  যদি (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  অন্যথা যদি (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	যদি (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } অন্যথা {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.যোগ_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
স্থির পূর্ণসংখ্যা profile_clock_handler(irq_hook_t *hook)
{
  কাঠামো proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (শূন্য *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ফেরত(1);                                    /* reenable interrupts */
}

শূন্য nmi_sprofile_handler(কাঠামো nmi_frame * frame)
{
	কাঠামো proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	যদি (nmi_in_kernel(frame)) {
		কাঠামো proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		যদি (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.যোগ_samples++;
			ফেরত;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (শূন্য *) frame->pc);
	}
	অন্যথা
		profile_sample(p, (শূন্য *) frame->pc);
}

#endif /* SPROFILE */
