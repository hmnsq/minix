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

#شامل "kernel/system.h"
#شامل "kernel/watchdog.h"

#اگر SPROFILE

/* user address to write info struct */
ساکن vir_بائٹیں sprof_info_addr_vir;

ساکن خالی clean_نظر_آیا_flag(خالی)
{
	صحیح i;

	برائے (i = 0; i < NR_TASKS + NR_PROCS; i++)
		proc[i].p_misc_flags &= ~MF_SPROF_نظر_آیا;
}

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
صحیح do_sprofile(ساخت proc * caller, پیغام * m_ptr)
{
  صحیح proc_nr;
  صحیح err;

  انتخاب(m_ptr->m_lsys_krn_sys_sprof.action) {

  حالت PROF_آغاز:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	اگر (sprofiling) {
		لکھو("SYSTEM: start s-profiling: already started\n");
		واپس EBUSY;
	}

	/* Test endpoint number. */
	اگر(!isokendpt(m_ptr->m_lsys_krn_sys_sprof.endpt, &proc_nr))
		واپس EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->m_lsys_krn_sys_sprof.endpt;
	sprof_info_addr_vir = m_ptr->m_lsys_krn_sys_sprof.ctl_ptr;
	sprof_ڈیٹا_addr_vir = m_ptr->m_lsys_krn_sys_sprof.mem_ptr;

	sprof_info.mem_مستعمل = 0;
	sprof_info.کل_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_سائز =
		m_ptr->m_lsys_krn_sys_sprof.mem_سائز < SAMPLE_بفر_سائز ?
		m_ptr->m_lsys_krn_sys_sprof.mem_سائز : SAMPLE_بفر_سائز;

	انتخاب (sprofiling_type = m_ptr->m_lsys_krn_sys_sprof.intr_type) {
		حالت PROF_RTC:
			init_profile_clock(m_ptr->m_lsys_krn_sys_sprof.freq);
			توڑو;
		حالت PROF_NMI:
			err = nmi_watchdog_آغاز_profiling(
				m_ptr->m_lsys_krn_sys_sprof.freq);
			اگر (err)
				واپس err;
			توڑو;
		طےشدہ:
			لکھو("ERROR : unknown profiling interrupt type\n");
			واپس EINVAL;
	}
	
	sprofiling = 1;

	clean_نظر_آیا_flag();

  	واپس OK;

  حالت PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	اگر (!sprofiling) {
		لکھو("SYSTEM: stop s-profiling: not started\n");
		واپس EBUSY;
	}

	sprofiling = 0;

	انتخاب (sprofiling_type) {
		حالت PROF_RTC:
			stop_profile_clock();
			توڑو;
		حالت PROF_NMI:
			nmi_watchdog_stop_profiling();
			توڑو;
	}

	ڈیٹا_copy(KERNEL, (vir_بائٹیں) &sprof_info,
		sprof_ep, sprof_info_addr_vir, حجم(sprof_info));
	ڈیٹا_copy(KERNEL, (vir_بائٹیں) sprof_sample_بفر,
		sprof_ep, sprof_ڈیٹا_addr_vir, sprof_info.mem_مستعمل);

	clean_نظر_آیا_flag();

  	واپس OK;

  طےشدہ:
	واپس EINVAL;
  }
}

#اگر_ختم /* SPROFILE */

