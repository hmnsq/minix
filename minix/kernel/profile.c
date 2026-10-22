/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਜੇ SPROFILE

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "watchdog.h"

ਅੱਖਰ sprof_sample_ਬਫਰ[SAMPLE_ਬਫਰ_ਮਾਪ];

/* Function prototype for the profiling clock handler. */ 
ਸਥਿਰ ਪੂਰਨ profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ਸਥਿਰ irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
ਖਾਲੀ init_profile_clock(u32_t freq)
{
  ਪੂਰਨ irq;

  ਜੇ((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
ਖਾਲੀ stop_profile_clock(ਖਾਲੀ)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ਸਥਿਰ ਖਾਲੀ sprof_save_sample(ਬਣਤਰ proc * p, ਖਾਲੀ * pc)
{
	ਬਣਤਰ sprof_sample *s;

	s = (ਬਣਤਰ sprof_sample *) (sprof_sample_ਬਫਰ + sprof_info.mem_ਵਰਤਿਆ);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_ਵਰਤਿਆ += ਆਕਾਰ(ਬਣਤਰ sprof_sample);
}

ਸਥਿਰ ਖਾਲੀ sprof_save_proc(ਬਣਤਰ proc * p)
{
	ਬਣਤਰ sprof_proc * s;

	s = (ਬਣਤਰ sprof_proc *) (sprof_sample_ਬਫਰ + sprof_info.mem_ਵਰਤਿਆ);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_ਵਰਤਿਆ += ਆਕਾਰ(ਬਣਤਰ sprof_proc);
}

ਸਥਿਰ ਖਾਲੀ profile_sample(ਬਣਤਰ proc * p, ਖਾਲੀ * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ਜੇ (!sprofiling || sprof_info.mem_ਵਰਤਿਆ == -1)
	  ਵਾਪਸ;

  /* Check if enough memory available before writing sample. */
  ਜੇ (sprof_info.mem_ਵਰਤਿਆ + ਆਕਾਰ(sprof_info) +
		  2*ਆਕਾਰ(ਬਣਤਰ sprof_sample) +
		  2*ਆਕਾਰ(ਬਣਤਰ sprof_sample) > sprof_mem_ਮਾਪ) {
	sprof_info.mem_ਵਰਤਿਆ = -1;
	ਵਾਪਸ;
  }

  /* Runnable system process? */
  ਜੇ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ਨਹੀਂਤਾਂ ਜੇ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ਜੇ (!(p->p_misc_flags & MF_SPROF_ਵੇਖਿਆ)) {
		p->p_misc_flags |= MF_SPROF_ਵੇਖਿਆ;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ਨਹੀਂਤਾਂ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.ਕੁੱਲ_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ profile_clock_handler(irq_hook_t *hook)
{
  ਬਣਤਰ proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (ਖਾਲੀ *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ਵਾਪਸ(1);                                    /* reenable interrupts */
}

ਖਾਲੀ nmi_sprofile_handler(ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ)
{
	ਬਣਤਰ proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ਜੇ (nmi_ਵਿੱਚ_kernel(ਚੌਖਟਾ)) {
		ਬਣਤਰ proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ਜੇ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.ਕੁੱਲ_samples++;
			ਵਾਪਸ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (ਖਾਲੀ *) ਚੌਖਟਾ->pc);
	}
	ਨਹੀਂਤਾਂ
		profile_sample(p, (ਖਾਲੀ *) ਚੌਖਟਾ->pc);
}

#ਜੇ_ਅੰਤ /* SPROFILE */
