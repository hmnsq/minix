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

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <sys/ptrace.h>

#எனில் USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
முழு do_trace(கட்டமைப்பு proc * caller, message * m_ptr)
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

  பதிவு கட்டமைப்பு proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  முழு tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  முழு tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned எழுத்து ub;
  முழு i;

#define COPYTOPROC(addr, myaddr, length) {		\
	கட்டமைப்பு vir_addr fromaddr, toaddr;		\
	முழு r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	எனில்((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		திரும்பு r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	கட்டமைப்பு vir_addr fromaddr, toaddr;		\
	முழு r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	எனில்((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		திரும்பு r;\
	}  \
}

  எனில்(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) திரும்பு(EINVAL);
  எனில் (iskerneln(tr_proc_nr)) திரும்பு(EPERM);

  rp = proc_addr(tr_proc_nr);
  எனில் (isemptyp(rp)) திரும்பு(EINVAL);
  தேர்வு (tr_request) {
  நிலை T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	திரும்பு(OK);

  நிலை T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, அளவு(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	நிறுத்து;

  நிலை T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, அளவு(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	நிறுத்து;

  நிலை T_GETUSER:		/* return value from process table */
	எனில் ((tr_addr & (அளவு(long) - 1)) != 0) திரும்பு(EFAULT);

	எனில் (tr_addr <= அளவு(கட்டமைப்பு proc) - அளவு(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((எழுத்து *) rp + (முழு) tr_addr);
		நிறுத்து;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = அளவு(long) - 1;
	tr_addr -= (அளவு(கட்டமைப்பு proc) + i) & ~i;

	எனில் (tr_addr > அளவு(கட்டமைப்பு priv) - அளவு(long)) திரும்பு(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((எழுத்து *) rp->p_priv + (முழு) tr_addr);
	நிறுத்து;

  நிலை T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, அளவு(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, அளவு(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_SETUSER:			/* set value in process table */
	எனில் ((tr_addr & (அளவு(reg_t) - 1)) != 0 ||
	     tr_addr > அளவு(கட்டமைப்பு stackframe_s) - அளவு(reg_t))
		திரும்பு(EFAULT);
	i = (முழு) tr_addr;
#எனில் defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	எனில் (i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.cs ||
	    i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.ds ||
	    i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.es ||
	    i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.gs ||
	    i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.fs ||
	    i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.ss)
		திரும்பு(EFAULT);

	எனில் (i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	இல்லையெனில்
		*(reg_t *) ((எழுத்து *) &rp->p_reg + i) = (reg_t) tr_data;
#அல்லதெனில் defined(__arm__)
	எனில் (i == (முழு) &((கட்டமைப்பு proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} இல்லையெனில் {
		*(reg_t *) ((எழுத்து *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  நிலை T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  நிலை T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	நிறுத்து;

  நிலை T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned எழுத்து) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	நிறுத்து;

  இயல்பு:
	திரும்பு(EINVAL);
  }
  திரும்பு(OK);
}

#endif /* USE_TRACE */
