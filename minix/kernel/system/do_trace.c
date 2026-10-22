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

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <sys/ptrace.h>

#যদি USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
পূর্ণাঙ্ক do_trace(সংরচনা proc * caller, message * m_ptr)
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

  পঞ্জিকা সংরচনা proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  পূর্ণাঙ্ক tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  পূর্ণাঙ্ক tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned ৱর্ণ ub;
  পূর্ণাঙ্ক i;

#define COPYTOPROC(addr, myaddr, length) {		\
	সংরচনা vir_addr fromaddr, toaddr;		\
	পূর্ণাঙ্ক r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	যদি((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ঘূৰাই r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	সংরচনা vir_addr fromaddr, toaddr;		\
	পূর্ণাঙ্ক r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	যদি((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ঘূৰাই r;\
	}  \
}

  যদি(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ঘূৰাই(EINVAL);
  যদি (iskerneln(tr_proc_nr)) ঘূৰাই(EPERM);

  rp = proc_addr(tr_proc_nr);
  যদি (isemptyp(rp)) ঘূৰাই(EINVAL);
  ৱিকল্প (tr_request) {
  পক্ষ T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	ঘূৰাই(OK);

  পক্ষ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, পরিমাণ(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	ভঙ্গ;

  পক্ষ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, পরিমাণ(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	ভঙ্গ;

  পক্ষ T_GETUSER:		/* return value from process table */
	যদি ((tr_addr & (পরিমাণ(long) - 1)) != 0) ঘূৰাই(EFAULT);

	যদি (tr_addr <= পরিমাণ(সংরচনা proc) - পরিমাণ(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((ৱর্ণ *) rp + (পূর্ণাঙ্ক) tr_addr);
		ভঙ্গ;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = পরিমাণ(long) - 1;
	tr_addr -= (পরিমাণ(সংরচনা proc) + i) & ~i;

	যদি (tr_addr > পরিমাণ(সংরচনা priv) - পরিমাণ(long)) ঘূৰাই(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((ৱর্ণ *) rp->p_priv + (পূর্ণাঙ্ক) tr_addr);
	ভঙ্গ;

  পক্ষ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, পরিমাণ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, পরিমাণ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_SETUSER:			/* set value in process table */
	যদি ((tr_addr & (পরিমাণ(reg_t) - 1)) != 0 ||
	     tr_addr > পরিমাণ(সংরচনা stackframe_s) - পরিমাণ(reg_t))
		ঘূৰাই(EFAULT);
	i = (পূর্ণাঙ্ক) tr_addr;
#যদি defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	যদি (i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.cs ||
	    i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.ds ||
	    i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.es ||
	    i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.gs ||
	    i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.fs ||
	    i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.ss)
		ঘূৰাই(EFAULT);

	যদি (i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	অন্যথা
		*(reg_t *) ((ৱর্ণ *) &rp->p_reg + i) = (reg_t) tr_data;
#নোচেদ্যদি defined(__arm__)
	যদি (i == (পূর্ণাঙ্ক) &((সংরচনা proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} অন্যথা {
		*(reg_t *) ((ৱর্ণ *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  পক্ষ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  পক্ষ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	ভঙ্গ;

  পক্ষ T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned ৱর্ণ) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ভঙ্গ;

  স্ৱভাৱ:
	ঘূৰাই(EINVAL);
  }
  ঘূৰাই(OK);
}

#endif /* USE_TRACE */
