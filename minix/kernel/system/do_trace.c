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
صحیح do_trace(ساخت proc * caller, پیغام * m_ptr)
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

  اندراج ساخت proc *rp;
  vir_بائٹیں tr_addr = m_ptr->m_lsys_krn_sys_trace.پتہ;
  طویل tr_ڈیٹا = m_ptr->m_lsys_krn_sys_trace.ڈیٹا;
  صحیح tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  صحیح tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  غیرعلامتی حرف ub;
  صحیح i;

#تعریف_کرو COPYTOPROC(addr, myaddr, لمبائی) {		\
	ساخت vir_addr fromaddr, toaddr;		\
	صحیح r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.انحراف = (myaddr);			\
	toaddr.انحراف = (addr);				\
	اگر((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, لمبائی)) != OK) {	\
		لکھو("Can't copy in sys_trace: %d\n", r);\
		واپس r;\
	}  \
}

#تعریف_کرو COPYFROMPROC(addr, myaddr, لمبائی) {	\
	ساخت vir_addr fromaddr, toaddr;		\
	صحیح r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.انحراف = (addr);			\
	toaddr.انحراف = (myaddr);			\
	اگر((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, لمبائی)) != OK) {	\
		لکھو("Can't copy in sys_trace: %d\n", r);\
		واپس r;\
	}  \
}

  اگر(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) واپس(EINVAL);
  اگر (iskerneln(tr_proc_nr)) واپس(EPERM);

  rp = proc_addr(tr_proc_nr);
  اگر (isemptyp(rp)) واپس(EINVAL);
  انتخاب (tr_request) {
  حالت T_STOP:			/* stop process */
	RTS_سیٹ(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_قدم);
	واپس(OK);

  حالت T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_بائٹیں) &tr_ڈیٹا, حجم(طویل));
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = tr_ڈیٹا;
	توڑو;

  حالت T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_بائٹیں) &tr_ڈیٹا, حجم(طویل));
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا= tr_ڈیٹا;
	توڑو;

  حالت T_GETUSER:		/* return value from process table */
	اگر ((tr_addr & (حجم(طویل) - 1)) != 0) واپس(EFAULT);

	اگر (tr_addr <= حجم(ساخت proc) - حجم(طویل)) {
		m_ptr->m_krn_lsys_sys_trace.ڈیٹا =
		    *(طویل *) ((حرف *) rp + (صحیح) tr_addr);
		توڑو;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = حجم(طویل) - 1;
	tr_addr -= (حجم(ساخت proc) + i) & ~i;

	اگر (tr_addr > حجم(ساخت priv) - حجم(طویل)) واپس(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.ڈیٹا =
	    *(طویل *) ((حرف *) rp->p_priv + (صحیح) tr_addr);
	توڑو;

  حالت T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_بائٹیں) &tr_ڈیٹا, حجم(طویل));
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_بائٹیں) &tr_ڈیٹا, حجم(طویل));
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_SETUSER:			/* set value in process table */
	اگر ((tr_addr & (حجم(reg_t) - 1)) != 0 ||
	     tr_addr > حجم(ساخت stackframe_s) - حجم(reg_t))
		واپس(EFAULT);
	i = (صحیح) tr_addr;
#اگر defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	اگر (i == (صحیح) &((ساخت proc *) 0)->p_reg.cs ||
	    i == (صحیح) &((ساخت proc *) 0)->p_reg.ds ||
	    i == (صحیح) &((ساخت proc *) 0)->p_reg.es ||
	    i == (صحیح) &((ساخت proc *) 0)->p_reg.gs ||
	    i == (صحیح) &((ساخت proc *) 0)->p_reg.fs ||
	    i == (صحیح) &((ساخت proc *) 0)->p_reg.ss)
		واپس(EFAULT);

	اگر (i == (صحیح) &((ساخت proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_ڈیٹا);
	ورنہ
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_ڈیٹا;
#وگرنہ_اگر defined(__arm__)
	اگر (i == (صحیح) &((ساخت proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		سیٹ_USR_PSR(rp, tr_ڈیٹا);
	} ورنہ {
		*(reg_t *) ((حرف *) &rp->p_reg + i) = (reg_t) tr_ڈیٹا;
	}
#اگر_ختم
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_فعال;

	/* fall through */
  حالت T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_قدم:			/* set trace bit */
	rp->p_misc_flags |= MF_قدم;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  حالت T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_بائٹیں) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = ub;
	توڑو;

  حالت T_WRITEB_INS:		/* set value in instruction space */
	ub = (غیرعلامتی حرف) (tr_ڈیٹا & 0xff);
	COPYTOPROC(tr_addr, (vir_بائٹیں) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.ڈیٹا = 0;
	توڑو;

  طےشدہ:
	واپس(EINVAL);
  }
  واپس(OK);
}

#اگر_ختم /* USE_TRACE */
