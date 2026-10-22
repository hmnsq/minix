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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव "kernel/watchdog.h"

#जर SPROFILE

/* user address to write info struct */
स्थिर vir_bytes sprof_info_addr_vir;

स्थिर शून्य clean_seen_flag(शून्य)
{
	पूर्णांक i;

	साठी (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
पूर्णांक do_sprofile(संरचना proc * caller, message * m_ptr)
{
  पूर्णांक proc_nr;
  पूर्णांक err;

  निवड(m_ptr->m_lsys_krn_sys_sprof.action) {

  स्थिती PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	जर (sprofiling) {
		म_लिखो("SYSTEM: start s-profiling: already started\n");
		परत EBUSY;
	}

	/* Test endpoint number. */
	जर(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		परत EINVAL;

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

	निवड (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		स्थिती PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			थांबा;
		स्थिती PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			जर (err)
				परत err;
			थांबा;
		पूर्वनिर्धारित:
			म_लिखो("ERROR : unknown profiling interrupt type\n");
			परत EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	परत OK;

  स्थिती PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	जर (!sprofiling) {
		म_लिखो("SYSTEM: stop s-profiling: not started\n");
		परत EBUSY;
	}

	sprofiling = 0;

	निवड (sprofiling_type) {
		स्थिती PROF_RTC:
			stop_profile_clock();
			थांबा;
		स्थिती PROF_NMI:
			nmi_watchdog_stop_profiling();
			थांबा;
	}

	data_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, आकार(sprof_info));
	data_copy(KERNEL, (vir_bytes) sprof_sample_buffer,
		sprof_ep, sprof_data_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	परत OK;

  पूर्वनिर्धारित:
	परत EINVAL;
  }
}

#endif /* SPROFILE */

