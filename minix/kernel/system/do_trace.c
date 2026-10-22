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
صحيح do_trace(بنية proc * caller, message * m_ptr)
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
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  صحيح tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  صحيح tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned حرف ub;
  صحيح i;

#define COPYTOPROC(addr, myaddr, length) {		\
	بنية vir_addr fromaddr, toaddr;		\
	صحيح r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	إذا((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		إرجاع r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	بنية vir_addr fromaddr, toaddr;		\
	صحيح r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	إذا((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		إرجاع r;\
	}  \
}

  إذا(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) إرجاع(EINVAL);
  إذا (iskerneln(tr_proc_nr)) إرجاع(EPERM);

  rp = proc_addr(tr_proc_nr);
  إذا (isemptyp(rp)) إرجاع(EINVAL);
  بدل (tr_request) {
  حالة T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	إرجاع(OK);

  حالة T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, حجم(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	اكسر;

  حالة T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, حجم(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	اكسر;

  حالة T_GETUSER:		/* return value from process table */
	إذا ((tr_addr & (حجم(long) - 1)) != 0) إرجاع(EFAULT);

	إذا (tr_addr <= حجم(بنية proc) - حجم(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((حرف *) rp + (صحيح) tr_addr);
		اكسر;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = حجم(long) - 1;
	tr_addr -= (حجم(بنية proc) + i) & ~i;

	إذا (tr_addr > حجم(بنية priv) - حجم(long)) إرجاع(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((حرف *) rp->p_priv + (صحيح) tr_addr);
	اكسر;

  حالة T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, حجم(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, حجم(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_SETUSER:			/* set value in process table */
	إذا ((tr_addr & (حجم(reg_t) - 1)) != 0 ||
	     tr_addr > حجم(بنية stackframe_s) - حجم(reg_t))
		إرجاع(EFAULT);
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
		إرجاع(EFAULT);

	إذا (i == (صحيح) &((بنية proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	وإلا
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_data;
#وإلاإذا defined(__arm__)
	إذا (i == (صحيح) &((بنية proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} وإلا {
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  حالة T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  حالة T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	اكسر;

  حالة T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned حرف) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	اكسر;

  افتراضي:
	إرجاع(EINVAL);
  }
  إرجاع(OK);
}

#endif /* USE_TRACE */
