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

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <sys/ptrace.h>

#જો USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
પૂર્ણાંક do_trace(સંરચના proc * caller, message * m_ptr)
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

  નોંધણી સંરચના proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  પૂર્ણાંક tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  પૂર્ણાંક tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned અક્ષર ub;
  પૂર્ણાંક i;

#define COPYTOPROC(addr, myaddr, length) {		\
	સંરચના vir_addr fromaddr, toaddr;		\
	પૂર્ણાંક r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	જો((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		પાછા r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	સંરચના vir_addr fromaddr, toaddr;		\
	પૂર્ણાંક r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	જો((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		પાછા r;\
	}  \
}

  જો(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) પાછા(EINVAL);
  જો (iskerneln(tr_proc_nr)) પાછા(EPERM);

  rp = proc_addr(tr_proc_nr);
  જો (isemptyp(rp)) પાછા(EINVAL);
  પસંદગી (tr_request) {
  સ્થિતિ T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	પાછા(OK);

  સ્થિતિ T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, કદ(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	તોડો;

  સ્થિતિ T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, કદ(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	તોડો;

  સ્થિતિ T_GETUSER:		/* return value from process table */
	જો ((tr_addr & (કદ(long) - 1)) != 0) પાછા(EFAULT);

	જો (tr_addr <= કદ(સંરચના proc) - કદ(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((અક્ષર *) rp + (પૂર્ણાંક) tr_addr);
		તોડો;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = કદ(long) - 1;
	tr_addr -= (કદ(સંરચના proc) + i) & ~i;

	જો (tr_addr > કદ(સંરચના priv) - કદ(long)) પાછા(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((અક્ષર *) rp->p_priv + (પૂર્ણાંક) tr_addr);
	તોડો;

  સ્થિતિ T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, કદ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, કદ(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_SETUSER:			/* set value in process table */
	જો ((tr_addr & (કદ(reg_t) - 1)) != 0 ||
	     tr_addr > કદ(સંરચના stackframe_s) - કદ(reg_t))
		પાછા(EFAULT);
	i = (પૂર્ણાંક) tr_addr;
#જો defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	જો (i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.cs ||
	    i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.ds ||
	    i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.es ||
	    i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.gs ||
	    i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.fs ||
	    i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.ss)
		પાછા(EFAULT);

	જો (i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	નહીંતર
		*(reg_t *) ((અક્ષર *) &rp->p_reg + i) = (reg_t) tr_data;
#અથવાજો defined(__arm__)
	જો (i == (પૂર્ણાંક) &((સંરચના proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} નહીંતર {
		*(reg_t *) ((અક્ષર *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  સ્થિતિ T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  સ્થિતિ T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	તોડો;

  સ્થિતિ T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned અક્ષર) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	તોડો;

  મૂળભૂત:
	પાછા(EINVAL);
  }
  પાછા(OK);
}

#endif /* USE_TRACE */
