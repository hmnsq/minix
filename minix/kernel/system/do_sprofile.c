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

#ضمن "kernel/system.h"
#ضمن "kernel/watchdog.h"

#إذا SPROFILE

/* user address to write info struct */
ساكن vir_بایتها sprof_info_addr_vir;

ساكن فراغ clean_دیده_flag(فراغ)
{
	صحيح i;

	لكل (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_دیده;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
صحيح do_sprofile(بنية proc * caller, پیام * m_ptr)
{
  صحيح proc_nr;
  صحيح err;

  بدل(m_ptr->m_lsys_krn_sys_sprof.action) {

  حالة PROF_شروع:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	إذا (sprofiling) {
		چاپ_کن("SYSTEM: start s-profiling: already started\n");
		أرجع EBUSY;
	}

	/* Test endpoint number. */
	إذا(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		أرجع EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_داده_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_استفاده_شده = 0;
	sprof_info.مجموع_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_اندازه =
		m_ptr->m_lsys_krn_sys_sprof.mem_اندازه < SAMPLE_میانگیر_اندازه ?
		m_ptr->m_lsys_krn_sys_sprof.mem_اندازه : SAMPLE_میانگیر_اندازه;

	بدل (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		حالة PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			اكسر;
		حالة PROF_NMI:
			err = nmi_watchdog_شروع_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			إذا (err)
				أرجع err;
			اكسر;
		افتراضي:
			چاپ_کن("ERROR : unknown profiling interrupt type\n");
			أرجع EINVAL;
	}
	
	sprofiling = 1;

	clean_دیده_flag();

  	أرجع OK;

  حالة PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	إذا (!sprofiling) {
		چاپ_کن("SYSTEM: stop s-profiling: not started\n");
		أرجع EBUSY;
	}

	sprofiling = 0;

	بدل (sprofiling_type) {
		حالة PROF_RTC:
			stop_profile_clock();
			اكسر;
		حالة PROF_NMI:
			nmi_watchdog_stop_profiling();
			اكسر;
	}

	داده_copy(KERNEL, (vir_بایتها) &sprof_info,
		sprof_ep, sprof_info_addr_vir, حجم(sprof_info));
	داده_copy(KERNEL, (vir_بایتها) sprof_sample_میانگیر,
		sprof_ep, sprof_داده_addr_vir, sprof_info.mem_استفاده_شده);

	clean_دیده_flag();

  	أرجع OK;

  افتراضي:
	أرجع EINVAL;
  }
}

#پایان_اگر /* SPROFILE */

