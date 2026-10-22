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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"

#ಯದಿ SPROFILE

/* user address to write info struct */
ಸ್ಥಿರ vir_bytes sprof_info_addr_vir;

ಸ್ಥಿರ ಶೂನ್ಯ clean_seen_flag(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_sprofile(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  ಪೂರ್ಣಾಙ್ಕ proc_nr;
  ಪೂರ್ಣಾಙ್ಕ err;

  ವಿಕಲ್ಪ(m_ptr->m_lsys_krn_sys_sprof.action) {

  ಪಕ್ಷ PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	ಯದಿ (sprofiling) {
		printf("SYSTEM: start s-profiling: already started\n");
		ಪ್ರತ್ಯಾಗಮ EBUSY;
	}

	/* Test endpoint number. */
	ಯದಿ(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_data_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_used = 0;
	sprof_info.total_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_size =
		m_ptr->m_lsys_krn_sys_sprof.mem_size < SAMPLE_BUFFER_SIZE ?
		m_ptr->m_lsys_krn_sys_sprof.mem_size : SAMPLE_BUFFER_SIZE;

	ವಿಕಲ್ಪ (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		ಪಕ್ಷ PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			ಭಙ್ಗ;
		ಪಕ್ಷ PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			ಯದಿ (err)
				ಪ್ರತ್ಯಾಗಮ err;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			printf("ERROR : unknown profiling interrupt type\n");
			ಪ್ರತ್ಯಾಗಮ EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	ಪ್ರತ್ಯಾಗಮ OK;

  ಪಕ್ಷ PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	ಯದಿ (!sprofiling) {
		printf("SYSTEM: stop s-profiling: not started\n");
		ಪ್ರತ್ಯಾಗಮ EBUSY;
	}

	sprofiling = 0;

	ವಿಕಲ್ಪ (sprofiling_type) {
		ಪಕ್ಷ PROF_RTC:
			stop_profile_clock();
			ಭಙ್ಗ;
		ಪಕ್ಷ PROF_NMI:
			nmi_watchdog_stop_profiling();
			ಭಙ್ಗ;
	}

	data_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, ಪರಿಮಾಣ(sprof_info));
	data_copy(KERNEL, (vir_bytes) sprof_sample_buffer,
		sprof_ep, sprof_data_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	ಪ್ರತ್ಯಾಗಮ OK;

  ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ EINVAL;
  }
}

#endif /* SPROFILE */

