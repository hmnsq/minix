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

#ಯದಿ USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_trace(ಸಂರಚನಾ proc * caller, message * m_ptr)
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

  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  ಪೂರ್ಣಾಙ್ಕ tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  ಪೂರ್ಣಾಙ್ಕ tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned ವರ್ಣ ub;
  ಪೂರ್ಣಾಙ್ಕ i;

#define COPYTOPROC(addr, myaddr, length) {		\
	ಸಂರಚನಾ vir_addr fromaddr, toaddr;		\
	ಪೂರ್ಣಾಙ್ಕ r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	ಯದಿ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ಪ್ರತ್ಯಾಗಮ r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	ಸಂರಚನಾ vir_addr fromaddr, toaddr;		\
	ಪೂರ್ಣಾಙ್ಕ r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	ಯದಿ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ಪ್ರತ್ಯಾಗಮ r;\
	}  \
}

  ಯದಿ(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ಯದಿ (iskerneln(tr_proc_nr)) ಪ್ರತ್ಯಾಗಮ(EPERM);

  rp = proc_addr(tr_proc_nr);
  ಯದಿ (isemptyp(rp)) ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ವಿಕಲ್ಪ (tr_request) {
  ಪಕ್ಷ T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	ಪ್ರತ್ಯಾಗಮ(OK);

  ಪಕ್ಷ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, ಪರಿಮಾಣ(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, ಪರಿಮಾಣ(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_GETUSER:		/* return value from process table */
	ಯದಿ ((tr_addr & (ಪರಿಮಾಣ(long) - 1)) != 0) ಪ್ರತ್ಯಾಗಮ(EFAULT);

	ಯದಿ (tr_addr <= ಪರಿಮಾಣ(ಸಂರಚನಾ proc) - ಪರಿಮಾಣ(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((ವರ್ಣ *) rp + (ಪೂರ್ಣಾಙ್ಕ) tr_addr);
		ಭಙ್ಗ;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = ಪರಿಮಾಣ(long) - 1;
	tr_addr -= (ಪರಿಮಾಣ(ಸಂರಚನಾ proc) + i) & ~i;

	ಯದಿ (tr_addr > ಪರಿಮಾಣ(ಸಂರಚನಾ priv) - ಪರಿಮಾಣ(long)) ಪ್ರತ್ಯಾಗಮ(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((ವರ್ಣ *) rp->p_priv + (ಪೂರ್ಣಾಙ್ಕ) tr_addr);
	ಭಙ್ಗ;

  ಪಕ್ಷ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, ಪರಿಮಾಣ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, ಪರಿಮಾಣ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_SETUSER:			/* set value in process table */
	ಯದಿ ((tr_addr & (ಪರಿಮಾಣ(reg_t) - 1)) != 0 ||
	     tr_addr > ಪರಿಮಾಣ(ಸಂರಚನಾ stackframe_s) - ಪರಿಮಾಣ(reg_t))
		ಪ್ರತ್ಯಾಗಮ(EFAULT);
	i = (ಪೂರ್ಣಾಙ್ಕ) tr_addr;
#ಯದಿ defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.cs ||
	    i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.ds ||
	    i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.es ||
	    i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.gs ||
	    i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.fs ||
	    i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.ss)
		ಪ್ರತ್ಯಾಗಮ(EFAULT);

	ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	ಅನ್ಯಥಾ
		*(reg_t *) ((ವರ್ಣ *) &rp->p_reg + i) = (reg_t) tr_data;
#ನೋಚೇದ್ಯದಿ defined(__arm__)
	ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ) &((ಸಂರಚನಾ proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} ಅನ್ಯಥಾ {
		*(reg_t *) ((ವರ್ಣ *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  ಪಕ್ಷ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	ಭಙ್ಗ;

  ಪಕ್ಷ T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned ವರ್ಣ) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ಭಙ್ಗ;

  ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ(EINVAL);
  }
  ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif /* USE_TRACE */
