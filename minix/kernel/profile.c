/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#समावेश "kernel/kernel.h"

#अगर SPROFILE

#समावेश <string.h>
#समावेश "watchdog.h"

अक्षर sprof_sample_बफर[SAMPLE_बफर_आकार];

/* Function prototype for the profiling clock handler. */ 
अटल पूर्णांक profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
अटल irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
व्योम init_profile_clock(u32_t freq)
{
  पूर्णांक irq;

  अगर((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
व्योम stop_profile_clock(व्योम)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

अटल व्योम sprof_save_sample(काष्ठा proc * p, व्योम * pc)
{
	काष्ठा sprof_sample *s;

	s = (काष्ठा sprof_sample *) (sprof_sample_बफर + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += माप(काष्ठा sprof_sample);
}

अटल व्योम sprof_save_proc(काष्ठा proc * p)
{
	काष्ठा sprof_proc * s;

	s = (काष्ठा sprof_proc *) (sprof_sample_बफर + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	म_नकल(s->name, p->p_name);

	sprof_info.mem_used += माप(काष्ठा sprof_proc);
}

अटल व्योम profile_sample(काष्ठा proc * p, व्योम * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  अगर (!sprofiling || sprof_info.mem_used == -1)
	  वापस;

  /* Check if enough memory available before writing sample. */
  अगर (sprof_info.mem_used + माप(sprof_info) +
		  2*माप(काष्ठा sprof_sample) +
		  2*माप(काष्ठा sprof_sample) > sprof_mem_आकार) {
	sprof_info.mem_used = -1;
	वापस;
  }

  /* Runnable system process? */
  अगर (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  अन्यथा अगर (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	अगर (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } अन्यथा {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.योग_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
अटल पूर्णांक profile_clock_handler(irq_hook_t *hook)
{
  काष्ठा proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (व्योम *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  वापस(1);                                    /* reenable interrupts */
}

व्योम nmi_sprofile_handler(काष्ठा nmi_frame * frame)
{
	काष्ठा proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	अगर (nmi_in_kernel(frame)) {
		काष्ठा proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		अगर (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.योग_samples++;
			वापस;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (व्योम *) frame->pc);
	}
	अन्यथा
		profile_sample(p, (व्योम *) frame->pc);
}

#पूर्ण_अगर /* SPROFILE */
