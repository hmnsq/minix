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

#شامل "kernel/system.h"
#شامل <sys/ptrace.h>

#اگر USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
صحیح do_trace(ساختار proc * caller, message * m_ptr)
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

  ثبات ساختار proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  صحیح tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  صحیح tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned نویسه ub;
  صحیح i;

#define COPYTOPROC(addr, myaddr, length) {		\
	ساختار vir_addr fromaddr, toaddr;		\
	صحیح r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	اگر((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		بازگشت r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	ساختار vir_addr fromaddr, toaddr;		\
	صحیح r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	اگر((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		بازگشت r;\
	}  \
}

  اگر(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) بازگشت(EINVAL);
  اگر (iskerneln(tr_proc_nr)) بازگشت(EPERM);

  rp = proc_addr(tr_proc_nr);
  اگر (isemptyp(rp)) بازگشت(EINVAL);
  انتخاب (tr_request) {
  حالت T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	بازگشت(OK);

  حالت T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, اندازه(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	بشکن;

  حالت T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, اندازه(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	بشکن;

  حالت T_GETUSER:		/* return value from process table */
	اگر ((tr_addr & (اندازه(long) - 1)) != 0) بازگشت(EFAULT);

	اگر (tr_addr <= اندازه(ساختار proc) - اندازه(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((نویسه *) rp + (صحیح) tr_addr);
		بشکن;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = اندازه(long) - 1;
	tr_addr -= (اندازه(ساختار proc) + i) & ~i;

	اگر (tr_addr > اندازه(ساختار priv) - اندازه(long)) بازگشت(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((نویسه *) rp->p_priv + (صحیح) tr_addr);
	بشکن;

  حالت T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, اندازه(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, اندازه(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_SETUSER:			/* set value in process table */
	اگر ((tr_addr & (اندازه(reg_t) - 1)) != 0 ||
	     tr_addr > اندازه(ساختار stackframe_s) - اندازه(reg_t))
		بازگشت(EFAULT);
	i = (صحیح) tr_addr;
#اگر defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	اگر (i == (صحیح) &((ساختار proc *) 0)->p_reg.cs ||
	    i == (صحیح) &((ساختار proc *) 0)->p_reg.ds ||
	    i == (صحیح) &((ساختار proc *) 0)->p_reg.es ||
	    i == (صحیح) &((ساختار proc *) 0)->p_reg.gs ||
	    i == (صحیح) &((ساختار proc *) 0)->p_reg.fs ||
	    i == (صحیح) &((ساختار proc *) 0)->p_reg.ss)
		بازگشت(EFAULT);

	اگر (i == (صحیح) &((ساختار proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	وگرنه
		*(reg_t *) ((نویسه *) &rp->p_reg + i) = (reg_t) tr_data;
#وگرنهاگر defined(__arm__)
	اگر (i == (صحیح) &((ساختار proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} وگرنه {
		*(reg_t *) ((نویسه *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  حالت T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  حالت T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	بشکن;

  حالت T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned نویسه) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	بشکن;

  پیشفرض:
	بازگشت(EINVAL);
  }
  بازگشت(OK);
}

#endif /* USE_TRACE */
