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

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব "kernel/watchdog.h"

#যদি SPROFILE

/* user address to write info struct */
স্থির vir_bytes sprof_info_addr_vir;

স্থির শূন্য clean_seen_flag(শূন্য)
{
	পূর্ণাঙ্ক i;

	প্রতি (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_sprofile(সংরচনা proc * caller, message * m_ptr)
{
  পূর্ণাঙ্ক proc_nr;
  পূর্ণাঙ্ক err;

  বিকল্প(m_ptr->m_lsys_krn_sys_sprof.action) {

  পক্ষ PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	যদি (sprofiling) {
		printf("SYSTEM: start s-profiling: already started\n");
		প্রত্যাগম EBUSY;
	}

	/* Test endpoint number. */
	যদি(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		প্রত্যাগম EINVAL;

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

	বিকল্প (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		পক্ষ PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			ভঙ্গ;
		পক্ষ PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			যদি (err)
				প্রত্যাগম err;
			ভঙ্গ;
		স্বভাব:
			printf("ERROR : unknown profiling interrupt type\n");
			প্রত্যাগম EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	প্রত্যাগম OK;

  পক্ষ PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	যদি (!sprofiling) {
		printf("SYSTEM: stop s-profiling: not started\n");
		প্রত্যাগম EBUSY;
	}

	sprofiling = 0;

	বিকল্প (sprofiling_type) {
		পক্ষ PROF_RTC:
			stop_profile_clock();
			ভঙ্গ;
		পক্ষ PROF_NMI:
			nmi_watchdog_stop_profiling();
			ভঙ্গ;
	}

	data_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, পরিমাণ(sprof_info));
	data_copy(KERNEL, (vir_bytes) sprof_sample_buffer,
		sprof_ep, sprof_data_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	প্রত্যাগম OK;

  স্বভাব:
	প্রত্যাগম EINVAL;
  }
}

#endif /* SPROFILE */

