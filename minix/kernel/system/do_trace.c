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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <sys/ptrace.h>

#ఒకవేళ USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
పూర్ణాంకం do_trace(నిర్మాణం proc * caller, సందేశం * m_ptr)
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

  నమోదు నిర్మాణం proc *rp;
  vir_బైట్లు tr_addr = m_ptr->m_lsys_krn_sys_trace.చిరునామా;
  దీర్ఘ tr_దత్తాంశం = m_ptr->m_lsys_krn_sys_trace.దత్తాంశం;
  పూర్ణాంకం tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  పూర్ణాంకం tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  గుర్తులేని అక్షరం ub;
  పూర్ణాంకం i;

#నిర్వచించు COPYTOPROC(addr, myaddr, నిడివి) {		\
	నిర్మాణం vir_addr fromaddr, toaddr;		\
	పూర్ణాంకం r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.స్థానభ్రంశం = (myaddr);			\
	toaddr.స్థానభ్రంశం = (addr);				\
	ఒకవేళ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, నిడివి)) != OK) {	\
		రాయి("Can't copy in sys_trace: %d\n", r);\
		తిరిగి r;\
	}  \
}

#నిర్వచించు COPYFROMPROC(addr, myaddr, నిడివి) {	\
	నిర్మాణం vir_addr fromaddr, toaddr;		\
	పూర్ణాంకం r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.స్థానభ్రంశం = (addr);			\
	toaddr.స్థానభ్రంశం = (myaddr);			\
	ఒకవేళ((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, నిడివి)) != OK) {	\
		రాయి("Can't copy in sys_trace: %d\n", r);\
		తిరిగి r;\
	}  \
}

  ఒకవేళ(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) తిరిగి(EINVAL);
  ఒకవేళ (iskerneln(tr_proc_nr)) తిరిగి(EPERM);

  rp = proc_addr(tr_proc_nr);
  ఒకవేళ (isemptyp(rp)) తిరిగి(EINVAL);
  ఎంపిక (tr_request) {
  స్థితి T_STOP:			/* stop process */
	RTS_సమితి(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_అడుగు);
	తిరిగి(OK);

  స్థితి T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_బైట్లు) &tr_దత్తాంశం, పరిమాణం(దీర్ఘ));
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = tr_దత్తాంశం;
	ఆపు;

  స్థితి T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_బైట్లు) &tr_దత్తాంశం, పరిమాణం(దీర్ఘ));
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం= tr_దత్తాంశం;
	ఆపు;

  స్థితి T_GETUSER:		/* return value from process table */
	ఒకవేళ ((tr_addr & (పరిమాణం(దీర్ఘ) - 1)) != 0) తిరిగి(EFAULT);

	ఒకవేళ (tr_addr <= పరిమాణం(నిర్మాణం proc) - పరిమాణం(దీర్ఘ)) {
		m_ptr->m_krn_lsys_sys_trace.దత్తాంశం =
		    *(దీర్ఘ *) ((అక్షరం *) rp + (పూర్ణాంకం) tr_addr);
		ఆపు;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = పరిమాణం(దీర్ఘ) - 1;
	tr_addr -= (పరిమాణం(నిర్మాణం proc) + i) & ~i;

	ఒకవేళ (tr_addr > పరిమాణం(నిర్మాణం priv) - పరిమాణం(దీర్ఘ)) తిరిగి(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం =
	    *(దీర్ఘ *) ((అక్షరం *) rp->p_priv + (పూర్ణాంకం) tr_addr);
	ఆపు;

  స్థితి T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_బైట్లు) &tr_దత్తాంశం, పరిమాణం(దీర్ఘ));
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_బైట్లు) &tr_దత్తాంశం, పరిమాణం(దీర్ఘ));
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_SETUSER:			/* set value in process table */
	ఒకవేళ ((tr_addr & (పరిమాణం(reg_t) - 1)) != 0 ||
	     tr_addr > పరిమాణం(నిర్మాణం stackframe_s) - పరిమాణం(reg_t))
		తిరిగి(EFAULT);
	i = (పూర్ణాంకం) tr_addr;
#ఒకవేళ defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ఒకవేళ (i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.cs ||
	    i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.ds ||
	    i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.es ||
	    i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.gs ||
	    i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.fs ||
	    i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.ss)
		తిరిగి(EFAULT);

	ఒకవేళ (i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_దత్తాంశం);
	లేకపోతే
		*(reg_t *) ((అక్షరం *) &rp->p_reg + i) = (reg_t) tr_దత్తాంశం;
#లేదావేళ defined(__arm__)
	ఒకవేళ (i == (పూర్ణాంకం) &((నిర్మాణం proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		సమితి_USR_PSR(rp, tr_దత్తాంశం);
	} లేకపోతే {
		*(reg_t *) ((అక్షరం *) &rp->p_reg + i) = (reg_t) tr_దత్తాంశం;
	}
#అయితే_ముగింపు
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_చురుకైన;

	/* fall through */
  స్థితి T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_అడుగు:			/* set trace bit */
	rp->p_misc_flags |= MF_అడుగు;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  స్థితి T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_బైట్లు) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = ub;
	ఆపు;

  స్థితి T_WRITEB_INS:		/* set value in instruction space */
	ub = (గుర్తులేని అక్షరం) (tr_దత్తాంశం & 0xff);
	COPYTOPROC(tr_addr, (vir_బైట్లు) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.దత్తాంశం = 0;
	ఆపు;

  అప్రమేయ:
	తిరిగి(EINVAL);
  }
  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_TRACE */
