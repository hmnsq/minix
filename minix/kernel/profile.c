/*
 * This file contains several functions and variables used for statistical
 * system profiling, in particular the interrupt handler for profiling clock.
 *
 * Changes:
 *   14 Aug, 2006   Created, (Rogier Meurs)
 */

#include "kernel/kernel.h"

#ᬬᬾᬦ᭄ SPROFILE

#include <string.h>
#include "watchdog.h"

var_na sprof_sample_buffer[SAMPLE_BUFFER_SIZE];

/* Function prototype for the profiling clock handler. */ 
static puur_naa_nka profile_clock_handler(irq_hook_t *hook);

/* A hook for the profiling clock interrupt handler. */
static irq_hook_t profile_clock_hook;

/*===========================================================================*
 *			init_profile_clock				     *
 *===========================================================================*/
void init_profile_clock(u32_t freq)
{
  puur_naa_nka irq;

  ᬬᬾᬦ᭄((irq = arch_init_profile_clock(freq)) >= 0) {
	/* Register interrupt handler for statistical system profiling.  */
	profile_clock_hook.proc_nr_e = CLOCK;
	put_irq_handler(&profile_clock_hook, irq, profile_clock_handler);
	enable_irq(&profile_clock_hook);
  }
}

/*===========================================================================*
 *			profile_clock_stop				     *
 *===========================================================================*/
void stop_profile_clock(void)
{
  arch_stop_profile_clock();

  /* Unregister interrupt handler. */
  disable_irq(&profile_clock_hook);
  rm_irq_handler(&profile_clock_hook);
}

static void sprof_save_sample(sa_nracanaa proc * p, void * pc)
{
	sa_nracanaa sprof_sample *s;

	s = (sa_nracanaa sprof_sample *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	s->pc = pc;

	sprof_info.mem_used += parimaa_na(sa_nracanaa sprof_sample);
}

static void sprof_save_proc(sa_nracanaa proc * p)
{
	sa_nracanaa sprof_proc * s;

	s = (sa_nracanaa sprof_proc *) (sprof_sample_buffer + sprof_info.mem_used);

	s->proc = p->p_endpoint;
	strcpy(s->name, p->p_name);

	sprof_info.mem_used += parimaa_na(sa_nracanaa sprof_proc);
}

static void profile_sample(sa_nracanaa proc * p, void * pc)
{
/* This executes on every tick of the CMOS timer. */

  /* Are we profiling, and profiling memory not full? */
  ᬬᬾᬦ᭄ (!sprofiling || sprof_info.mem_used == -1)
	  ᬯᬮᬶ;

  /* Check if enough memory available before writing sample. */
  ᬬᬾᬦ᭄ (sprof_info.mem_used + parimaa_na(sprof_info) +
		  2*parimaa_na(sa_nracanaa sprof_sample) +
		  2*parimaa_na(sa_nracanaa sprof_sample) > sprof_mem_size) {
	sprof_info.mem_used = -1;
	ᬯᬮᬶ;
  }

  /* Runnable system process? */
  ᬬᬾᬦ᭄ (p->p_endpoint == IDLE)
	sprof_info.idle_samples++;
  ᬮᬶᬬ ᬬᬾᬦ᭄ (p->p_endpoint == KERNEL ||
		(priv(p)->s_flags & SYS_PROC && proc_is_runnable(p))) {

	ᬬᬾᬦ᭄ (!(p->p_misc_flags & MF_SPROF_SEEN)) {
		p->p_misc_flags |= MF_SPROF_SEEN;
		sprof_save_proc(p);
	}

	sprof_save_sample(p, pc);
	sprof_info.system_samples++;
  } ᬮᬶᬬ {
	/* User process. */
	sprof_info.user_samples++;
  }
  
  sprof_info.total_samples++;
}

/*===========================================================================*
 *			profile_clock_handler                           *
 *===========================================================================*/
static puur_naa_nka profile_clock_handler(irq_hook_t *hook)
{
  sa_nracanaa proc * p;
  p = get_cpulocal_var(proc_ptr);

  profile_sample(p, (void *) p->p_reg.pc);

  /* Acknowledge interrupt if necessary. */
  arch_ack_profile_clock();

  ᬯᬮᬶ(1);                                    /* reenable interrupts */
}

void nmi_sprofile_handler(sa_nracanaa nmi_frame * frame)
{
	sa_nracanaa proc * p = get_cpulocal_var(proc_ptr);
	/*
	 * test if the kernel was interrupted. If so, save first a sample fo
	 * kernel and than for the current process, otherwise save just the
	 * process
	 */
	ᬬᬾᬦ᭄ (nmi_in_kernel(frame)) {
		sa_nracanaa proc *kern;

		/*
		 * if we sample kernel, check if IDLE is scheduled. If so,
		 * account for idle time rather than taking kernel sample
		 */
		ᬬᬾᬦ᭄ (p->p_endpoint == IDLE) {
			sprof_info.idle_samples++;
			sprof_info.total_samples++;
			ᬯᬮᬶ;
		}

		kern = proc_addr(KERNEL);

		profile_sample(kern, (void *) frame->pc);
	}
	ᬮᬶᬬ
		profile_sample(p, (void *) frame->pc);
}

#endif /* SPROFILE */
