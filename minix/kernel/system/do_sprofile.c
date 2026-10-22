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
	পূর্ণসংখ্যা i;

	জন্য (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_sprofile(কাঠামো proc * caller, message * m_ptr)
{
  পূর্ণসংখ্যা proc_nr;
  পূর্ণসংখ্যা err;

  নির্বাচন(m_ptr->m_lsys_krn_sys_sprof.action) {

  ক্ষেত্র PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	যদি (sprofiling) {
		printf("SYSTEM: start s-profiling: already started\n");
		ফেরত EBUSY;
	}

	/* Test endpoint number. */
	যদি(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		ফেরত EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_তথ্য_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_used = 0;
	sprof_info.যোগ_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_আকার =
		m_ptr->m_lsys_krn_sys_sprof.mem_আকার < SAMPLE_বাফার_আকার ?
		m_ptr->m_lsys_krn_sys_sprof.mem_আকার : SAMPLE_বাফার_আকার;

	নির্বাচন (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		ক্ষেত্র PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			বিরতি;
		ক্ষেত্র PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			যদি (err)
				ফেরত err;
			বিরতি;
		পূর্বনির্ধারিত:
			printf("ERROR : unknown profiling interrupt type\n");
			ফেরত EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	ফেরত OK;

  ক্ষেত্র PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	যদি (!sprofiling) {
		printf("SYSTEM: stop s-profiling: not started\n");
		ফেরত EBUSY;
	}

	sprofiling = 0;

	নির্বাচন (sprofiling_type) {
		ক্ষেত্র PROF_RTC:
			stop_profile_clock();
			বিরতি;
		ক্ষেত্র PROF_NMI:
			nmi_watchdog_stop_profiling();
			বিরতি;
	}

	তথ্য_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, আকার(sprof_info));
	তথ্য_copy(KERNEL, (vir_bytes) sprof_sample_বাফার,
		sprof_ep, sprof_তথ্য_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	ফেরত OK;

  পূর্বনির্ধারিত:
	ফেরত EINVAL;
  }
}

#endif /* SPROFILE */

