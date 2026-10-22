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

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <sys/ptrace.h>

#എങ്കിൽ USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
പൂർണ്ണസംഖ്യ do_trace(ഘടന proc * caller, message * m_ptr)
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

  രജിസ്റ്റർ ഘടന proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  പൂർണ്ണസംഖ്യ tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  പൂർണ്ണസംഖ്യ tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned അക്ഷരം ub;
  പൂർണ്ണസംഖ്യ i;

#define COPYTOPROC(addr, myaddr, length) {		\
	ഘടന vir_addr fromaddr, toaddr;		\
	പൂർണ്ണസംഖ്യ r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	എങ്കിൽ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		തിരികെ r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	ഘടന vir_addr fromaddr, toaddr;		\
	പൂർണ്ണസംഖ്യ r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	എങ്കിൽ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		തിരികെ r;\
	}  \
}

  എങ്കിൽ(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) തിരികെ(EINVAL);
  എങ്കിൽ (iskerneln(tr_proc_nr)) തിരികെ(EPERM);

  rp = proc_addr(tr_proc_nr);
  എങ്കിൽ (isemptyp(rp)) തിരികെ(EINVAL);
  തിരഞ്ഞെടുപ്പ് (tr_request) {
  അവസ്ഥ T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	തിരികെ(OK);

  അവസ്ഥ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, വലുപ്പം(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	നിർത്തുക;

  അവസ്ഥ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, വലുപ്പം(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	നിർത്തുക;

  അവസ്ഥ T_GETUSER:		/* return value from process table */
	എങ്കിൽ ((tr_addr & (വലുപ്പം(long) - 1)) != 0) തിരികെ(EFAULT);

	എങ്കിൽ (tr_addr <= വലുപ്പം(ഘടന proc) - വലുപ്പം(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((അക്ഷരം *) rp + (പൂർണ്ണസംഖ്യ) tr_addr);
		നിർത്തുക;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = വലുപ്പം(long) - 1;
	tr_addr -= (വലുപ്പം(ഘടന proc) + i) & ~i;

	എങ്കിൽ (tr_addr > വലുപ്പം(ഘടന priv) - വലുപ്പം(long)) തിരികെ(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((അക്ഷരം *) rp->p_priv + (പൂർണ്ണസംഖ്യ) tr_addr);
	നിർത്തുക;

  അവസ്ഥ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, വലുപ്പം(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, വലുപ്പം(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_SETUSER:			/* set value in process table */
	എങ്കിൽ ((tr_addr & (വലുപ്പം(reg_t) - 1)) != 0 ||
	     tr_addr > വലുപ്പം(ഘടന stackframe_s) - വലുപ്പം(reg_t))
		തിരികെ(EFAULT);
	i = (പൂർണ്ണസംഖ്യ) tr_addr;
#എങ്കിൽ defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.cs ||
	    i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.ds ||
	    i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.es ||
	    i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.gs ||
	    i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.fs ||
	    i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.ss)
		തിരികെ(EFAULT);

	എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	അല്ലെങ്കിൽ
		*(reg_t *) ((അക്ഷരം *) &rp->p_reg + i) = (reg_t) tr_data;
#അല്ലെങ്കിലെങ്കിൽ defined(__arm__)
	എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ) &((ഘടന proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} അല്ലെങ്കിൽ {
		*(reg_t *) ((അക്ഷരം *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  അവസ്ഥ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  അവസ്ഥ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	നിർത്തുക;

  അവസ്ഥ T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned അക്ഷരം) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
	തിരികെ(EINVAL);
  }
  തിരികെ(OK);
}

#endif /* USE_TRACE */
