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

#ضمن "kernel/system.h"
#ضمن <sys/ptrace.h>

#إذا USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
صحيح do_trace(بنية proc * caller, پیغام * m_ptr)
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

  سجل بنية proc *rp;
  vir_بایټونه tr_addr = m_ptr->m_lsys_krn_sys_trace.پته;
  اوږد tr_معلومات = m_ptr->m_lsys_krn_sys_trace.معلومات;
  صحيح tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  صحيح tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  بینښه حرف ub;
  صحيح i;

#تعریف_کړه COPYTOPROC(addr, myaddr, اوږدتیا) {		\
	بنية vir_addr fromaddr, toaddr;		\
	صحيح r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.دننه_واټن = (myaddr);			\
	toaddr.دننه_واټن = (addr);				\
	إذا((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, اوږدتیا)) != OK) {	\
		چاپ_کړه("Can't copy in sys_trace: %d\n", r);\
		أرجع r;\
	}  \
}

#تعریف_کړه COPYFROMPROC(addr, myaddr, اوږدتیا) {	\
	بنية vir_addr fromaddr, toaddr;		\
	صحيح r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.دننه_واټن = (addr);			\
	toaddr.دننه_واټن = (myaddr);			\
	إذا((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, اوږدتیا)) != OK) {	\
		چاپ_کړه("Can't copy in sys_trace: %d\n", r);\
		أرجع r;\
	}  \
}

  إذا(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) أرجع(EINVAL);
  إذا (iskerneln(tr_proc_nr)) أرجع(EPERM);

  rp = proc_addr(tr_proc_nr);
  إذا (isemptyp(rp)) أرجع(EINVAL);
  بدل (tr_request) {
  حالة T_STOP:			/* stop process */
	RTS_سېټ(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_ګام);
	أرجع(OK);

  حالة T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_بایټونه) &tr_معلومات, حجم(اوږد));
	m_ptr->m_krn_lsys_sys_trace.معلومات = tr_معلومات;
	اكسر;

  حالة T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_بایټونه) &tr_معلومات, حجم(اوږد));
	m_ptr->m_krn_lsys_sys_trace.معلومات= tr_معلومات;
	اكسر;

  حالة T_GETUSER:		/* return value from process table */
	إذا ((tr_addr & (حجم(اوږد) - 1)) != 0) أرجع(EFAULT);

	إذا (tr_addr <= حجم(بنية proc) - حجم(اوږد)) {
		m_ptr->m_krn_lsys_sys_trace.معلومات =
		    *(اوږد *) ((حرف *) rp + (صحيح) tr_addr);
		اكسر;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = حجم(اوږد) - 1;
	tr_addr -= (حجم(بنية proc) + i) & ~i;

	إذا (tr_addr > حجم(بنية priv) - حجم(اوږد)) أرجع(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.معلومات =
	    *(اوږد *) ((حرف *) rp->p_priv + (صحيح) tr_addr);
	اكسر;

  حالة T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_بایټونه) &tr_معلومات, حجم(اوږد));
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_بایټونه) &tr_معلومات, حجم(اوږد));
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_SETUSER:			/* set value in process table */
	إذا ((tr_addr & (حجم(reg_t) - 1)) != 0 ||
	     tr_addr > حجم(بنية stackframe_s) - حجم(reg_t))
		أرجع(EFAULT);
	i = (صحيح) tr_addr;
#إذا defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	إذا (i == (صحيح) &((بنية proc *) 0)->p_reg.cs ||
	    i == (صحيح) &((بنية proc *) 0)->p_reg.ds ||
	    i == (صحيح) &((بنية proc *) 0)->p_reg.es ||
	    i == (صحيح) &((بنية proc *) 0)->p_reg.gs ||
	    i == (صحيح) &((بنية proc *) 0)->p_reg.fs ||
	    i == (صحيح) &((بنية proc *) 0)->p_reg.ss)
		أرجع(EFAULT);

	إذا (i == (صحيح) &((بنية proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_معلومات);
	وإلا
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_معلومات;
#وإلاإذا defined(__arm__)
	إذا (i == (صحيح) &((بنية proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		سېټ_USR_PSR(rp, tr_معلومات);
	} وإلا {
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_معلومات;
	}
#د_که_پای
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_فعال;

	/* fall through */
  حالة T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_ګام:			/* set trace bit */
	rp->p_misc_flags |= MF_ګام;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  حالة T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_بایټونه) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.معلومات = ub;
	اكسر;

  حالة T_WRITEB_INS:		/* set value in instruction space */
	ub = (بینښه حرف) (tr_معلومات & 0xff);
	COPYTOPROC(tr_addr, (vir_بایټونه) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.معلومات = 0;
	اكسر;

  افتراضي:
	أرجع(EINVAL);
  }
  أرجع(OK);
}

#د_که_پای /* USE_TRACE */
