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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <sys/ptrace.h>

#ଯଦି USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_trace(ସଂରଚନା proc * caller, message * m_ptr)
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

  ପଞ୍ଜିକରଣ ସଂରଚନା proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned ଅକ୍ଷର ub;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

#define COPYTOPROC(addr, myaddr, length) {		\
	ସଂରଚନା vir_addr fromaddr, toaddr;		\
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	ଯଦି((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ଫେରନ୍ତୁ r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	ସଂରଚନା vir_addr fromaddr, toaddr;		\
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	ଯଦି((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		ଫେରନ୍ତୁ r;\
	}  \
}

  ଯଦି(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) ଫେରନ୍ତୁ(EINVAL);
  ଯଦି (iskerneln(tr_proc_nr)) ଫେରନ୍ତୁ(EPERM);

  rp = proc_addr(tr_proc_nr);
  ଯଦି (isemptyp(rp)) ଫେରନ୍ତୁ(EINVAL);
  ବାଛ (tr_request) {
  ସ୍ଥିତି T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	ଫେରନ୍ତୁ(OK);

  ସ୍ଥିତି T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, ଆକାର(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, ଆକାର(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_GETUSER:		/* return value from process table */
	ଯଦି ((tr_addr & (ଆକାର(long) - 1)) != 0) ଫେରନ୍ତୁ(EFAULT);

	ଯଦି (tr_addr <= ଆକାର(ସଂରଚନା proc) - ଆକାର(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((ଅକ୍ଷର *) rp + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) tr_addr);
		ଭାଙ୍ଗ;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = ଆକାର(long) - 1;
	tr_addr -= (ଆକାର(ସଂରଚନା proc) + i) & ~i;

	ଯଦି (tr_addr > ଆକାର(ସଂରଚନା priv) - ଆକାର(long)) ଫେରନ୍ତୁ(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((ଅକ୍ଷର *) rp->p_priv + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) tr_addr);
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, ଆକାର(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, ଆକାର(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_SETUSER:			/* set value in process table */
	ଯଦି ((tr_addr & (ଆକାର(reg_t) - 1)) != 0 ||
	     tr_addr > ଆକାର(ସଂରଚନା stackframe_s) - ଆକାର(reg_t))
		ଫେରନ୍ତୁ(EFAULT);
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) tr_addr;
#ଯଦି defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.cs ||
	    i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.ds ||
	    i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.es ||
	    i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.gs ||
	    i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.fs ||
	    i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.ss)
		ଫେରନ୍ତୁ(EFAULT);

	ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	ଅନ୍ୟଥା
		*(reg_t *) ((ଅକ୍ଷର *) &rp->p_reg + i) = (reg_t) tr_data;
#ନଚେତଯଦି defined(__arm__)
	ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) &((ସଂରଚନା proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} ଅନ୍ୟଥା {
		*(reg_t *) ((ଅକ୍ଷର *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  ସ୍ଥିତି T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	ଭାଙ୍ଗ;

  ସ୍ଥିତି T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned ଅକ୍ଷର) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ(EINVAL);
  }
  ଫେରନ୍ତୁ(OK);
}

#endif /* USE_TRACE */
