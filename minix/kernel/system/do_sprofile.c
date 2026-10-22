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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ "kernel/watchdog.h"

#ଯଦି SPROFILE

/* user address to write info struct */
ସ୍ଥିର vir_bytes sprof_info_addr_vir;

ସ୍ଥିର ଶୂନ୍ୟ clean_seen_flag(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sprofile(ସଂରଚନା proc * caller, message * m_ptr)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

  ବାଛ(m_ptr->m_lsys_krn_sys_sprof.action) {

  ସ୍ଥିତି PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	ଯଦି (sprofiling) {
		printf("SYSTEM: start s-profiling: already started\n");
		ଫେରନ୍ତୁ EBUSY;
	}

	/* Test endpoint number. */
	ଯଦି(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		ଫେରନ୍ତୁ EINVAL;

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

	ବାଛ (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		ସ୍ଥିତି PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			ଭାଙ୍ଗ;
		ସ୍ଥିତି PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			ଯଦି (err)
				ଫେରନ୍ତୁ err;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			printf("ERROR : unknown profiling interrupt type\n");
			ଫେରନ୍ତୁ EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	ଫେରନ୍ତୁ OK;

  ସ୍ଥିତି PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	ଯଦି (!sprofiling) {
		printf("SYSTEM: stop s-profiling: not started\n");
		ଫେରନ୍ତୁ EBUSY;
	}

	sprofiling = 0;

	ବାଛ (sprofiling_type) {
		ସ୍ଥିତି PROF_RTC:
			stop_profile_clock();
			ଭାଙ୍ଗ;
		ସ୍ଥିତି PROF_NMI:
			nmi_watchdog_stop_profiling();
			ଭାଙ୍ଗ;
	}

	data_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, ଆକାର(sprof_info));
	data_copy(KERNEL, (vir_bytes) sprof_sample_buffer,
		sprof_ep, sprof_data_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	ଫେରନ୍ତୁ OK;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ EINVAL;
  }
}

#endif /* SPROFILE */

