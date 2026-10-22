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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/watchdog.h"

#ਜੇ SPROFILE

/* user address to write info struct */
ਸਥਿਰ vir_ਬਾਈਟਾਂ sprof_info_addr_vir;

ਸਥਿਰ ਖਾਲੀ clean_ਵੇਖਿਆ_flag(ਖਾਲੀ)
{
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_ਵੇਖਿਆ;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
ਪੂਰਨ do_sprofile(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਪੂਰਨ proc_nr;
  ਪੂਰਨ err;

  ਚੋਣ(m_ptr->m_lsys_krn_sys_sprof.action) {

  ਹਾਲਤ PROF_ਸ਼ੁਰੂ:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	ਜੇ (sprofiling) {
		ਲਿਖੋ("SYSTEM: start s-profiling: already started\n");
		ਵਾਪਸ EBUSY;
	}

	/* Test endpoint number. */
	ਜੇ(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		ਵਾਪਸ EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_ਅੰਕੜੇ_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_ਵਰਤਿਆ = 0;
	sprof_info.ਕੁੱਲ_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_ਮਾਪ =
		m_ptr->m_lsys_krn_sys_sprof.mem_ਮਾਪ < SAMPLE_ਬਫਰ_ਮਾਪ ?
		m_ptr->m_lsys_krn_sys_sprof.mem_ਮਾਪ : SAMPLE_ਬਫਰ_ਮਾਪ;

	ਚੋਣ (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		ਹਾਲਤ PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			ਰੋਕੋ;
		ਹਾਲਤ PROF_NMI:
			err = nmi_watchdog_ਸ਼ੁਰੂ_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			ਜੇ (err)
				ਵਾਪਸ err;
			ਰੋਕੋ;
		ਮੂਲ:
			ਲਿਖੋ("ERROR : unknown profiling interrupt type\n");
			ਵਾਪਸ EINVAL;
	}
	
	sprofiling = 1;

	clean_ਵੇਖਿਆ_flag();

  	ਵਾਪਸ OK;

  ਹਾਲਤ PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	ਜੇ (!sprofiling) {
		ਲਿਖੋ("SYSTEM: stop s-profiling: not started\n");
		ਵਾਪਸ EBUSY;
	}

	sprofiling = 0;

	ਚੋਣ (sprofiling_type) {
		ਹਾਲਤ PROF_RTC:
			stop_profile_clock();
			ਰੋਕੋ;
		ਹਾਲਤ PROF_NMI:
			nmi_watchdog_stop_profiling();
			ਰੋਕੋ;
	}

	ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ) &sprof_info,
		sprof_ep, sprof_info_addr_vir, ਆਕਾਰ(sprof_info));
	ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ) sprof_sample_ਬਫਰ,
		sprof_ep, sprof_ਅੰਕੜੇ_addr_vir, sprof_info.mem_ਵਰਤਿਆ);

	clean_ਵੇਖਿਆ_flag();

  	ਵਾਪਸ OK;

  ਮੂਲ:
	ਵਾਪਸ EINVAL;
  }
}

#ਜੇ_ਅੰਤ /* SPROFILE */

