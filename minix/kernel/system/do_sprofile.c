/* The kernel call that is implemented in this file:
 *   m_type:    SYS_SPROF
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_sys_sprof.action	(start/stop profiling)
 *	m_lsys_krn_sys_sprof.mem_size	(available memory for data)
 *	m_lsys_krn_sys_sprof.freq	(requested sample frequency)
 *	m_lsys_krn_sys_sprof.endpt	(endpoint of caller)
 *	m_lsys_krn_sys_sprof.ctl_ptr	(location of info struct)
 *	m_lsys_krn_sys_sprof.mem_ptr	(location of memory for data)
 *	m_lsys_krn_sys_sprof.intr_type	(interrupt source: RTC/NMI)
 *
 * Changes:
 *   14 Aug, 2006   Created (Rogier Meurs)
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ "kernel/watchdog.h"

#ఒకవేళ SPROFILE

/* user address to write info struct */
స్థిర vir_బైట్లు sprof_info_addr_vir;

స్థిర శూన్య clean_చూసిన_flag(శూన్య)
{
	పూర్ణాంకం i;

	కొరకు (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_చూసిన;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
పూర్ణాంకం do_sprofile(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  పూర్ణాంకం proc_nr;
  పూర్ణాంకం err;

  ఎంపిక(m_ptr->m_lsys_krn_sys_sprof.action) {

  స్థితి PROF_ప్రారంభం:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	ఒకవేళ (sprofiling) {
		రాయి("SYSTEM: start s-profiling: already started\n");
		తిరిగి EBUSY;
	}

	/* Test endpoint number. */
	ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		తిరిగి EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_దత్తాంశం_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_వాడిన = 0;
	sprof_info.మొత్తం_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_కొలత =
		m_ptr->m_lsys_krn_sys_sprof.mem_కొలత < SAMPLE_బఫరు_కొలత ?
		m_ptr->m_lsys_krn_sys_sprof.mem_కొలత : SAMPLE_బఫరు_కొలత;

	ఎంపిక (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		స్థితి PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			ఆపు;
		స్థితి PROF_NMI:
			err = nmi_watchdog_ప్రారంభం_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			ఒకవేళ (err)
				తిరిగి err;
			ఆపు;
		అప్రమేయ:
			రాయి("ERROR : unknown profiling interrupt type\n");
			తిరిగి EINVAL;
	}
	
	sprofiling = 1;

	clean_చూసిన_flag();

  	తిరిగి OK;

  స్థితి PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	ఒకవేళ (!sprofiling) {
		రాయి("SYSTEM: stop s-profiling: not started\n");
		తిరిగి EBUSY;
	}

	sprofiling = 0;

	ఎంపిక (sprofiling_type) {
		స్థితి PROF_RTC:
			stop_profile_clock();
			ఆపు;
		స్థితి PROF_NMI:
			nmi_watchdog_stop_profiling();
			ఆపు;
	}

	దత్తాంశం_copy(KERNEL, (vir_బైట్లు) &sprof_info,
		sprof_ep, sprof_info_addr_vir, పరిమాణం(sprof_info));
	దత్తాంశం_copy(KERNEL, (vir_బైట్లు) sprof_sample_బఫరు,
		sprof_ep, sprof_దత్తాంశం_addr_vir, sprof_info.mem_వాడిన);

	clean_చూసిన_flag();

  	తిరిగి OK;

  అప్రమేయ:
	తిరిగి EINVAL;
  }
}

#అయితే_ముగింపు /* SPROFILE */

