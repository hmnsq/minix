/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#అన్తర్భావ "kernel/kernel.h"

#ఒకవేళ SPROFILE

#అన్తర్భావ <string.h>
#అన్తర్భావ "watchdog.h"

అక్షరం sprof_sample_బఫరు[SAMPLE_బఫరు_కొలత];

/* Function prototype for the profiling clock handler. */ 
స్థిర పూర్ణాంకం profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
స్థిర irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
శూన్య init_profile_clock(u32_t freq)
{
  పూర్ణాంకం irq;

  ఒకవేళ((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
శూన్య stop_profile_clock(శూన్య)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

స్థిర శూన్య sprof_save_sample(నిర్మాణం proc * p, శూన్య * pc)
{
	నిర్మాణం sprof_sample *s;

	s = (నిర్మాణం sprof_sample *) (sprof_sample_బఫరు + sprof_info.mem_వాడిన);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_వాడిన += పరిమాణం(నిర్మాణం sprof_sample);
}

స్థిర శూన్య sprof_save_proc(నిర్మాణం proc * p)
{
	నిర్మాణం sprof_proc * s;

	s = (నిర్మాణం sprof_proc *) (sprof_sample_బఫరు + sprof_info.mem_వాడిన);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_వాడిన += పరిమాణం(నిర్మాణం sprof_proc);
}

స్థిర శూన్య profile_sample(నిర్మాణం proc * p, శూన్య * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ఒకవేళ (!sprofiling || sprof_info.mem_వాడిన == -1)
	  తిరిగి;

  /* Check if enough memory available before writing sample. */
  ఒకవేళ (sprof_info.mem_వాడిన + పరిమాణం(sprof_info) +
		  2*పరిమాణం(నిర్మాణం sprof_sample) +
		  2*పరిమాణం(నిర్మాణం sprof_sample) > sprof_mem_కొలత) {
	sprof_info.mem_వాడిన = -1;
	తిరిగి;
  }

  /* Runnable system process? */
  ఒకవేళ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  లేకపోతే ఒకవేళ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ఒకవేళ (!(p->p_misc_flags & MF_SPROF_చూసిన)) {
		p->p_misc_flags |= MF_SPROF_చూసిన;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } లేకపోతే {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.మొత్తం_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
స్థిర పూర్ణాంకం profile_clock_handler(irq_hook_t *hook)
{
  నిర్మాణం proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (శూన్య *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  తిరిగి(1);                                    /* reenable interrupts */
}

శూన్య nmi_sprofile_handler(నిర్మాణం nmi_చట్రం * చట్రం)
{
	నిర్మాణం proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ఒకవేళ (nmi_లోపల_kernel(చట్రం)) {
		నిర్మాణం proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ఒకవేళ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.మొత్తం_samples++;
			తిరిగి;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (శూన్య *) చట్రం->pc);
	}
	లేకపోతే
		profile_sample(p, (శూన్య *) చట్రం->pc);
}

#అయితే_ముగింపు /* SPROFILE */
