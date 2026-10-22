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

#समावेश "kernel/system.h"
#समावेश "kernel/watchdog.h"

#अगर SPROFILE

/* user address to write info struct */
अटल vir_bytes sprof_info_addr_vir;

अटल व्योम clean_seen_flag(व्योम)
{
	पूर्णांक i;

	क्रम (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_SEEN;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
पूर्णांक do_sprofile(काष्ठा proc * caller, message * m_ptr)
{
  पूर्णांक proc_nr;
  पूर्णांक err;

  चयन(m_ptr->m_lsys_krn_sys_sprof.action) {

  हाल PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	अगर (sprofiling) {
		म_लिखो("SYSTEM: start s-profiling: already started\n");
		वापस EBUSY;
	}

	/* Test endpoint number. */
	अगर(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		वापस EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_तथ्य_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_used = 0;
	sprof_info.योग_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_आकार =
		m_ptr->m_lsys_krn_sys_sprof.mem_आकार < SAMPLE_बफर_आकार ?
		m_ptr->m_lsys_krn_sys_sprof.mem_आकार : SAMPLE_बफर_आकार;

	चयन (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		हाल PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			अवरोध;
		हाल PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			अगर (err)
				वापस err;
			अवरोध;
		शेष:
			म_लिखो("ERROR : unknown profiling interrupt type\n");
			वापस EINVAL;
	}
	
	sprofiling = 1;

	clean_seen_flag();

  	वापस OK;

  हाल PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	अगर (!sprofiling) {
		म_लिखो("SYSTEM: stop s-profiling: not started\n");
		वापस EBUSY;
	}

	sprofiling = 0;

	चयन (sprofiling_type) {
		हाल PROF_RTC:
			stop_profile_clock();
			अवरोध;
		हाल PROF_NMI:
			nmi_watchdog_stop_profiling();
			अवरोध;
	}

	तथ्य_copy(KERNEL, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, माप(sprof_info));
	तथ्य_copy(KERNEL, (vir_bytes) sprof_sample_बफर,
		sprof_ep, sprof_तथ्य_addr_vir, sprof_info.mem_used);

	clean_seen_flag();

  	वापस OK;

  शेष:
	वापस EINVAL;
  }
}

#पूर्ण_अगर /* SPROFILE */

