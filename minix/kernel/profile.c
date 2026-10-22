/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#ضمن "kernel/kernel.h"

#𐤀𐤌 SPROFILE

#ضمن <string.h>
#ضمن "watchdog.h"

حرف sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
ساكن صحيح profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
ساكن irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
فراغ init_profile_clock(u32_t freq)
{
  صحيح irq;

  𐤀𐤌((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
فراغ stop_profile_clock(فراغ)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

ساكن فراغ sprof_save_sample(بنية proc * p, فراغ * pc)
{
	بنية sprof_sample *s;

	s = (بنية sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += حجم(بنية sprof_sample);
}

ساكن فراغ sprof_save_proc(بنية proc * p)
{
	بنية sprof_proc * s;

	s = (بنية sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += حجم(بنية sprof_proc);
}

ساكن فراغ profile_sample(بنية proc * p, فراغ * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  𐤀𐤌 (!sprofiling || sprof_info.mem_used == -1)
	  أرجع;

  /* Check if enough memory available before writing sample. */
  𐤀𐤌 (sprof_info.mem_used + حجم(sprof_info) +
		  2*حجم(بنية sprof_sample) +
		  2*حجم(بنية sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	أرجع;
  }

  /* Runnable system process? */
  𐤀𐤌 (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  وإلا 𐤀𐤌 (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	𐤀𐤌 (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } وإلا {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
ساكن صحيح profile_clock_handler(irq_hook_t *hook)
{
  بنية proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (فراغ *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  أرجع(1);                                    /* reenable interrupts */
}

فراغ nmi_sprofile_handler(بنية nmi_frame * frame)
{
	بنية proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	𐤀𐤌 (nmi_in_kernel(frame)) {
		بنية proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		𐤀𐤌 (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			أرجع;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (فراغ *) frame->pc);
	}
	وإلا
		profile_sample(p, (فراغ *) frame->pc);
}

#endif /* SPROFILE */
