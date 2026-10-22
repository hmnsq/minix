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

#यदि SPROFILE

/* user address to write info struct */
स्थिर vir_अष्टकानि sprof_info_addr_vir;

स्थिर शून्य clean_आलोकितम्_flag(शून्य)
{
	पूर्णाङ्क i;

	प्रति (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_आलोकितम्;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
पूर्णाङ्क do_sprofile(संरचना proc * caller, सन्देशः * m_ptr)
{
  पूर्णाङ्क proc_nr;
  पूर्णाङ्क err;

  विकल्प(m_ptr->m_lsys_krn_sys_sprof.action) {

  पक्ष PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	यदि (sprofiling) {
		प्रारूप_लिख("SYSTEM: start s-profiling: already started\n");
		प्रत्यागम EBUSY;
	}

	/* Test endpoint number. */
	यदि(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		प्रत्यागम EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_दत्त_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_प्रयुक्तम् = 0;
	sprof_info.योग_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_आकार =
		m_ptr->m_lsys_krn_sys_sprof.mem_आकार < SAMPLE_बफर_आकार ?
		m_ptr->m_lsys_krn_sys_sprof.mem_आकार : SAMPLE_बफर_आकार;

	विकल्प (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		पक्ष PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			भङ्ग;
		पक्ष PROF_NMI:
			err = nmi_watchdog_start_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			यदि (err)
				प्रत्यागम err;
			भङ्ग;
		स्वभाव:
			प्रारूप_लिख("ERROR : unknown profiling interrupt type\n");
			प्रत्यागम EINVAL;
	}
	
	sprofiling = 1;

	clean_आलोकितम्_flag();

  	प्रत्यागम OK;

  पक्ष PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	यदि (!sprofiling) {
		प्रारूप_लिख("SYSTEM: stop s-profiling: not started\n");
		प्रत्यागम EBUSY;
	}

	sprofiling = 0;

	विकल्प (sprofiling_type) {
		पक्ष PROF_RTC:
			stop_profile_clock();
			भङ्ग;
		पक्ष PROF_NMI:
			nmi_watchdog_stop_profiling();
			भङ्ग;
	}

	दत्त_copy(KERNEL, (vir_अष्टकानि) &sprof_info,
		sprof_ep, sprof_info_addr_vir, परिमाण(sprof_info));
	दत्त_copy(KERNEL, (vir_अष्टकानि) sprof_sample_बफर,
		sprof_ep, sprof_दत्त_addr_vir, sprof_info.mem_प्रयुक्तम्);

	clean_आलोकितम्_flag();

  	प्रत्यागम OK;

  स्वभाव:
	प्रत्यागम EINVAL;
  }
}

#यदि_अन्तः /* SPROFILE */

