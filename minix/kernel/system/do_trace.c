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

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <sys/ptrace.h>

#যদি USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
পূর্ণসংখ্যা do_trace(কাঠামো proc * caller, message * m_ptr)
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

  নিবন্ধ কাঠামো proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_তথ্য = m_ptr->m_lsys_krn_sys_trace.তথ্য;
  পূর্ণসংখ্যা tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  পূর্ণসংখ্যা tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned অক্ষর ub;
  পূর্ণসংখ্যা i;

#define COPYTOPROC(addr, myaddr, দৈর্ঘ্য) {		\
	কাঠামো vir_addr fromaddr, toaddr;		\
	পূর্ণসংখ্যা r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	যদি((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, দৈর্ঘ্য)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ফেরত r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, দৈর্ঘ্য) {	\
	কাঠামো vir_addr fromaddr, toaddr;		\
	পূর্ণসংখ্যা r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	যদি((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, দৈর্ঘ্য)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ফেরত r;\
	}  \
}

  যদি(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ফেরত(EINVAL);
  যদি (iskerneln(tr_proc_nr)) ফেরত(EPERM);

  rp = proc_addr(tr_proc_nr);
  যদি (isemptyp(rp)) ফেরত(EINVAL);
  নির্বাচন (tr_request) {
  ক্ষেত্র T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	ফেরত(OK);

  ক্ষেত্র T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_তথ্য, আকার(long));
	m_ptr->m_krn_lsys_sys_trace.তথ্য = tr_তথ্য;
	বিরতি;

  ক্ষেত্র T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_তথ্য, আকার(long));
	m_ptr->m_krn_lsys_sys_trace.তথ্য= tr_তথ্য;
	বিরতি;

  ক্ষেত্র T_GETUSER:		/* return value from process table */
	যদি ((tr_addr & (আকার(long) - 1)) != 0) ফেরত(EFAULT);

	যদি (tr_addr <= আকার(কাঠামো proc) - আকার(long)) {
		m_ptr->m_krn_lsys_sys_trace.তথ্য =
		    *(long *) ((অক্ষর *) rp + (পূর্ণসংখ্যা) tr_addr);
		বিরতি;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = আকার(long) - 1;
	tr_addr -= (আকার(কাঠামো proc) + i) & ~i;

	যদি (tr_addr > আকার(কাঠামো priv) - আকার(long)) ফেরত(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.তথ্য =
	    *(long *) ((অক্ষর *) rp->p_priv + (পূর্ণসংখ্যা) tr_addr);
	বিরতি;

  ক্ষেত্র T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_তথ্য, আকার(long));
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_তথ্য, আকার(long));
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_SETUSER:			/* set value in process table */
	যদি ((tr_addr & (আকার(reg_t) - 1)) != 0 ||
	     tr_addr > আকার(কাঠামো stackframe_s) - আকার(reg_t))
		ফেরত(EFAULT);
	i = (পূর্ণসংখ্যা) tr_addr;
#যদি defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	যদি (i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.cs ||
	    i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.ds ||
	    i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.es ||
	    i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.gs ||
	    i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.fs ||
	    i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.ss)
		ফেরত(EFAULT);

	যদি (i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_তথ্য);
	অন্যথা
		*(reg_t *) ((অক্ষর *) &rp->p_reg + i) = (reg_t) tr_তথ্য;
#নতুবা_যদি defined(__arm__)
	যদি (i == (পূর্ণসংখ্যা) &((কাঠামো proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_তথ্য);
	} অন্যথা {
		*(reg_t *) ((অক্ষর *) &rp->p_reg + i) = (reg_t) tr_তথ্য;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  ক্ষেত্র T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  ক্ষেত্র T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.তথ্য = ub;
	বিরতি;

  ক্ষেত্র T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned অক্ষর) (tr_তথ্য & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.তথ্য = 0;
	বিরতি;

  পূর্বনির্ধারিত:
	ফেরত(EINVAL);
  }
  ফেরত(OK);
}

#endif /* USE_TRACE */
