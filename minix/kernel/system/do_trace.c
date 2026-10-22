/* The kernel call implemented in this file:
 *   m_type:	SYS_TRACE
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_trace.endpt		process that is traced
 *   m_lsys_krn_sys_trace.request	trace request
 *   m_lsys_krn_sys_trace.address	address at traced process' space
 *   m_lsys_krn_sys_trace.data		data to be written
 *   m_krn_lsys_sys_trace.data		data to be returned
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <sys/ptrace.h>

#ಆದರೆ USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
ಪೂರ್ಣಾಂಕ do_trace(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle the debugging commands supported by the ptrace system call
 * The commands are:
 * T_STOP	stop the process
 * T_OK		enable tracing by parent for this process
 * T_GETINS	return value from instruction space
 * T_GETDATA	return value from data space
 * T_GETUSER	return value from user process table
 * T_SETINS	set value in instruction space
 * T_SETDATA	set value in data space
 * T_SETUSER	set value in user process table
 * T_RESUME	resume execution
 * T_EXIT	exit
 * T_STEP	set trace bit
 * T_SYSCALL	trace system call
 * T_ATTACH	attach to an existing process
 * T_DETACH	detach from a traced process
 * T_SETOPT	set trace options
 * T_GETRANGE	get range of values
 * T_SETRANGE	set range of values
 *
 * The T_OK, T_ATTACH, T_EXIT, and T_SETOPT commands are handled completely by
 * the process manager. T_GETRANGE and T_SETRANGE use sys_vircopy(). All others
 * come here.
 */

  ನೋಂದಣಿ ರಚನೆ proc *rp;
  vir_ಬೈಟುಗಳು tr_addr = m_ptr->m_lsys_krn_sys_trace.ವಿಳಾಸ;
  ದೀರ್ಘ tr_ದತ್ತಾಂಶ = m_ptr->m_lsys_krn_sys_trace.ದತ್ತಾಂಶ;
  ಪೂರ್ಣಾಂಕ tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  ಪೂರ್ಣಾಂಕ tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ub;
  ಪೂರ್ಣಾಂಕ i;

#ವ್ಯಾಖ್ಯಾನಿಸು COPYTOPROC(addr, myaddr, ಉದ್ದಳತೆ) {		\
	ರಚನೆ vir_addr fromaddr, toaddr;		\
	ಪೂರ್ಣಾಂಕ r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.ಸ್ಥಾನಾಂತರ = (myaddr);			\
	toaddr.ಸ್ಥಾನಾಂತರ = (addr);				\
	ಆದರೆ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, ಉದ್ದಳತೆ)) != OK) {	\
		ಬರೆ("Can't copy in sys_trace: %d\n", r);\
		ಹಿಂತಿರುಗಿ r;\
	}  \
}

#ವ್ಯಾಖ್ಯಾನಿಸು COPYFROMPROC(addr, myaddr, ಉದ್ದಳತೆ) {	\
	ರಚನೆ vir_addr fromaddr, toaddr;		\
	ಪೂರ್ಣಾಂಕ r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.ಸ್ಥಾನಾಂತರ = (addr);			\
	toaddr.ಸ್ಥಾನಾಂತರ = (myaddr);			\
	ಆದರೆ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, ಉದ್ದಳತೆ)) != OK) {	\
		ಬರೆ("Can't copy in sys_trace: %d\n", r);\
		ಹಿಂತಿರುಗಿ r;\
	}  \
}

  ಆದರೆ(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ಹಿಂತಿರುಗಿ(EINVAL);
  ಆದರೆ (iskerneln(tr_proc_nr)) ಹಿಂತಿರುಗಿ(EPERM);

  rp = proc_addr(tr_proc_nr);
  ಆದರೆ (isemptyp(rp)) ಹಿಂತಿರುಗಿ(EINVAL);
  ಆಯ್ಕೆ (tr_request) {
  ಸ್ಥಿತಿ T_STOP:			/* stop process */
	RTS_ಗಣ(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_ಹೆಜ್ಜೆ);
	ಹಿಂತಿರುಗಿ(OK);

  ಸ್ಥಿತಿ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_ಬೈಟುಗಳು) &tr_ದತ್ತಾಂಶ, ಗಾತ್ರ(ದೀರ್ಘ));
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = tr_ದತ್ತಾಂಶ;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_ಬೈಟುಗಳು) &tr_ದತ್ತಾಂಶ, ಗಾತ್ರ(ದೀರ್ಘ));
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ= tr_ದತ್ತಾಂಶ;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_GETUSER:		/* return value from process table */
	ಆದರೆ ((tr_addr & (ಗಾತ್ರ(ದೀರ್ಘ) - 1)) != 0) ಹಿಂತಿರುಗಿ(EFAULT);

	ಆದರೆ (tr_addr <= ಗಾತ್ರ(ರಚನೆ proc) - ಗಾತ್ರ(ದೀರ್ಘ)) {
		m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ =
		    *(ದೀರ್ಘ *) ((ಅಕ್ಷರ *) rp + (ಪೂರ್ಣಾಂಕ) tr_addr);
		ನಿಲ್ಲಿಸು;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = ಗಾತ್ರ(ದೀರ್ಘ) - 1;
	tr_addr -= (ಗಾತ್ರ(ರಚನೆ proc) + i) & ~i;

	ಆದರೆ (tr_addr > ಗಾತ್ರ(ರಚನೆ priv) - ಗಾತ್ರ(ದೀರ್ಘ)) ಹಿಂತಿರುಗಿ(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ =
	    *(ದೀರ್ಘ *) ((ಅಕ್ಷರ *) rp->p_priv + (ಪೂರ್ಣಾಂಕ) tr_addr);
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_ಬೈಟುಗಳು) &tr_ದತ್ತಾಂಶ, ಗಾತ್ರ(ದೀರ್ಘ));
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_ಬೈಟುಗಳು) &tr_ದತ್ತಾಂಶ, ಗಾತ್ರ(ದೀರ್ಘ));
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_SETUSER:			/* set value in process table */
	ಆದರೆ ((tr_addr & (ಗಾತ್ರ(reg_t) - 1)) != 0 ||
	     tr_addr > ಗಾತ್ರ(ರಚನೆ stackframe_s) - ಗಾತ್ರ(reg_t))
		ಹಿಂತಿರುಗಿ(EFAULT);
	i = (ಪೂರ್ಣಾಂಕ) tr_addr;
#ಆದರೆ defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.cs ||
	    i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.ds ||
	    i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.es ||
	    i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.gs ||
	    i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.fs ||
	    i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.ss)
		ಹಿಂತಿರುಗಿ(EFAULT);

	ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_ದತ್ತಾಂಶ);
	ಇಲ್ಲದಿದ್ದರೆ
		*(reg_t *) ((ಅಕ್ಷರ *) &rp->p_reg + i) = (reg_t) tr_ದತ್ತಾಂಶ;
#ಅಥವಾಆದರೆ defined(__arm__)
	ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ) &((ರಚನೆ proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		ಗಣ_USR_PSR(rp, tr_ದತ್ತಾಂಶ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		*(reg_t *) ((ಅಕ್ಷರ *) &rp->p_reg + i) = (reg_t) tr_ದತ್ತಾಂಶ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ಸಕ್ರಿಯ;

	/* fall through */
  ಸ್ಥಿತಿ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_ಹೆಜ್ಜೆ:			/* set trace bit */
	rp->p_misc_flags |= MF_ಹೆಜ್ಜೆ;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_ಬೈಟುಗಳು) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = ub;
	ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_WRITEB_INS:		/* set value in instruction space */
	ub = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) (tr_ದತ್ತಾಂಶ & 0xff);
	COPYTOPROC(tr_addr, (vir_ಬೈಟುಗಳು) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ದತ್ತಾಂಶ = 0;
	ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
	ಹಿಂತಿರುಗಿ(EINVAL);
  }
  ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_TRACE */
