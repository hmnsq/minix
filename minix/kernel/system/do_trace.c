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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ptrace.h>

#ਜੇ USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
ਪੂਰਨ do_trace(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
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

  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;
  vir_ਬਾਈਟਾਂ tr_addr = m_ptr->m_lsys_krn_sys_trace.ਪਤਾ;
  ਲੰਮਾ tr_ਅੰਕੜੇ = m_ptr->m_lsys_krn_sys_trace.ਅੰਕੜੇ;
  ਪੂਰਨ tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  ਪੂਰਨ tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ub;
  ਪੂਰਨ i;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPYTOPROC(addr, myaddr, ਲੰਮਾਈ) {		\
	ਬਣਤਰ vir_addr fromaddr, toaddr;		\
	ਪੂਰਨ r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.ਵਿੱਥ = (myaddr);			\
	toaddr.ਵਿੱਥ = (addr);				\
	ਜੇ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, ਲੰਮਾਈ)) != OK) {	\
		ਲਿਖੋ("Can't copy in sys_trace: %d\n", r);\
		ਵਾਪਸ r;\
	}  \
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPYFROMPROC(addr, myaddr, ਲੰਮਾਈ) {	\
	ਬਣਤਰ vir_addr fromaddr, toaddr;		\
	ਪੂਰਨ r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.ਵਿੱਥ = (addr);			\
	toaddr.ਵਿੱਥ = (myaddr);			\
	ਜੇ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, ਲੰਮਾਈ)) != OK) {	\
		ਲਿਖੋ("Can't copy in sys_trace: %d\n", r);\
		ਵਾਪਸ r;\
	}  \
}

  ਜੇ(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ਵਾਪਸ(EINVAL);
  ਜੇ (iskerneln(tr_proc_nr)) ਵਾਪਸ(EPERM);

  rp = proc_addr(tr_proc_nr);
  ਜੇ (isemptyp(rp)) ਵਾਪਸ(EINVAL);
  ਚੋਣ (tr_request) {
  ਹਾਲਤ T_STOP:			/* stop process */
	RTS_ਸੈੱਟ(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_ਕਦਮ);
	ਵਾਪਸ(OK);

  ਹਾਲਤ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &tr_ਅੰਕੜੇ, ਆਕਾਰ(ਲੰਮਾ));
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = tr_ਅੰਕੜੇ;
	ਰੋਕੋ;

  ਹਾਲਤ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &tr_ਅੰਕੜੇ, ਆਕਾਰ(ਲੰਮਾ));
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ= tr_ਅੰਕੜੇ;
	ਰੋਕੋ;

  ਹਾਲਤ T_GETUSER:		/* return value from process table */
	ਜੇ ((tr_addr & (ਆਕਾਰ(ਲੰਮਾ) - 1)) != 0) ਵਾਪਸ(EFAULT);

	ਜੇ (tr_addr <= ਆਕਾਰ(ਬਣਤਰ proc) - ਆਕਾਰ(ਲੰਮਾ)) {
		m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ =
		    *(ਲੰਮਾ *) ((ਅੱਖਰ *) rp + (ਪੂਰਨ) tr_addr);
		ਰੋਕੋ;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = ਆਕਾਰ(ਲੰਮਾ) - 1;
	tr_addr -= (ਆਕਾਰ(ਬਣਤਰ proc) + i) & ~i;

	ਜੇ (tr_addr > ਆਕਾਰ(ਬਣਤਰ priv) - ਆਕਾਰ(ਲੰਮਾ)) ਵਾਪਸ(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ =
	    *(ਲੰਮਾ *) ((ਅੱਖਰ *) rp->p_priv + (ਪੂਰਨ) tr_addr);
	ਰੋਕੋ;

  ਹਾਲਤ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &tr_ਅੰਕੜੇ, ਆਕਾਰ(ਲੰਮਾ));
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &tr_ਅੰਕੜੇ, ਆਕਾਰ(ਲੰਮਾ));
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_SETUSER:			/* set value in process table */
	ਜੇ ((tr_addr & (ਆਕਾਰ(reg_t) - 1)) != 0 ||
	     tr_addr > ਆਕਾਰ(ਬਣਤਰ stackframe_s) - ਆਕਾਰ(reg_t))
		ਵਾਪਸ(EFAULT);
	i = (ਪੂਰਨ) tr_addr;
#ਜੇ defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ਜੇ (i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.cs ||
	    i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.ds ||
	    i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.es ||
	    i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.gs ||
	    i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.fs ||
	    i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.ss)
		ਵਾਪਸ(EFAULT);

	ਜੇ (i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_ਅੰਕੜੇ);
	ਨਹੀਂਤਾਂ
		*(reg_t *) ((ਅੱਖਰ *) &rp->p_reg + i) = (reg_t) tr_ਅੰਕੜੇ;
#ਜਾਂਜੇ defined(__arm__)
	ਜੇ (i == (ਪੂਰਨ) &((ਬਣਤਰ proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		ਸੈੱਟ_USR_PSR(rp, tr_ਅੰਕੜੇ);
	} ਨਹੀਂਤਾਂ {
		*(reg_t *) ((ਅੱਖਰ *) &rp->p_reg + i) = (reg_t) tr_ਅੰਕੜੇ;
	}
#ਜੇ_ਅੰਤ
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ਸਰਗਰਮ;

	/* fall through */
  ਹਾਲਤ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_ਕਦਮ:			/* set trace bit */
	rp->p_misc_flags |= MF_ਕਦਮ;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਹਾਲਤ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = ub;
	ਰੋਕੋ;

  ਹਾਲਤ T_WRITEB_INS:		/* set value in instruction space */
	ub = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) (tr_ਅੰਕੜੇ & 0xff);
	COPYTOPROC(tr_addr, (vir_ਬਾਈਟਾਂ) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ਅੰਕੜੇ = 0;
	ਰੋਕੋ;

  ਮੂਲ:
	ਵਾਪਸ(EINVAL);
  }
  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_TRACE */
