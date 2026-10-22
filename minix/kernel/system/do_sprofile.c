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

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ "kernel/watchdog.h"

#எனில் SPROFILE

/* user address to write info struct */
நிலையான vir_bytes sprof_info_addr_vir;

நிலையான வெறுமை clean_seen_flag(வெறுமை)
{
	முழு i;

	க்கு (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
முழு do_sprofile(கட்டமைப்பு proc * caller, message * m_ptr)
{
  முழு proc_nr;
  முழு err;

  தேர்வு(m_ptr->m_lsys_krn_sys_sprof.action) {

  நிலை PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	எனில் (sprofiling) {
		printf("SYSTEM: start s-profiling: already started\n");
		திரும்பு EBUSY;
	}

	/* Test endpoint number. */
	எனில்(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		திரும்பு EINVAL;

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

	தேர்வு (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		நிலை PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			நிறுத்து;
		நிலை PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			எனில் (err)
				திரும்பு err;
			நிறுத்து;
		இயல்பு:
			printf("ERROR : unknown profiling interrupt type\n");
			திரும்பு EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	திரும்பு OK;

  நிலை PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	எனில் (!sprofiling) {
		printf("SYSTEM: stop s-profiling: not started\n");
		திரும்பு EBUSY;
	}

	sprofiling = 0;

	தேர்வு (sprofiling_type) {
		நிலை PROF_RTC:
			stop_profile_clock();
			நிறுத்து;
		நிலை PROF_NMI:
			nmi_watchdog_stop_profiling();
			நிறுத்து;
	}

	data_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, அளவு(sprof_info));
	data_copy(KERNEL, (vir_bytes) sprof_sample_buffer,
		sprof_ep, sprof_data_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	திரும்பு OK;

  இயல்பு:
	திரும்பு EINVAL;
  }
}

#endif /* SPROFILE */

