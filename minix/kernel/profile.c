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

حرف sprof_sample_بفر[SAMPLE_بفر_سائز];

/* Function prototype for the profiling clock handler. */ 
ساکن صحیح profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ساکن irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
خالی init_profile_clock(u32_t freq)
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
خالی stop_profile_clock(خالی)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ساکن خالی sprof_save_sample(ساخت proc * p, خالی * pc)
{
	ساخت sprof_sample *s;

	s = (ساخت sprof_sample *) (sprof_sample_بفر + sprof_info.mem_مستعمل);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_مستعمل += حجم(ساخت sprof_sample);
}

ساکن خالی sprof_save_proc(ساخت proc * p)
{
	ساخت sprof_proc * s;

	s = (ساخت sprof_proc *) (sprof_sample_بفر + sprof_info.mem_مستعمل);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_مستعمل += حجم(ساخت sprof_proc);
}

ساکن خالی profile_sample(ساخت proc * p, خالی * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  اگر (!sprofiling || sprof_info.mem_مستعمل == -1)
	  واپس;

  /* Check if enough memory available before writing sample. */
  اگر (sprof_info.mem_مستعمل + حجم(sprof_info) +
		  2*حجم(ساخت sprof_sample) +
		  2*حجم(ساخت sprof_sample) > sprof_mem_سائز) {
	sprof_info.mem_مستعمل = -1;
	واپس;
  }

  /* Runnable system process? */
  اگر (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ورنہ اگر (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	اگر (!(p->p_misc_flags & MF_SPROF_نظر_آیا)) {
		p->p_misc_flags |= MF_SPROF_نظر_آیا;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ورنہ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.کل_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ساکن صحیح profile_clock_handler(irq_hook_t *hook)
{
  ساخت proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (خالی *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  واپس(1);                                    /* reenable interrupts */
}

خالی nmi_sprofile_handler(ساخت nmi_چوکھٹا * چوکھٹا)
{
	ساخت proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	اگر (nmi_اندر_kernel(چوکھٹا)) {
		ساخت proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		اگر (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.کل_samples++;
			واپس;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (خالی *) چوکھٹا->pc);
	}
	ورنہ
		profile_sample(p, (خالی *) چوکھٹا->pc);
}

#اگر_ختم /* SPROFILE */
