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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <sys/ptrace.h>

#सचे USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
पूर्णाङ्क do_trace(संरचना proc * caller, message * m_ptr)
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

  पञ्जिका संरचना proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  पूर्णाङ्क tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  पूर्णाङ्क tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned वर्ण ub;
  पूर्णाङ्क i;

#define COPYTOPROC(addr, myaddr, length) {		\
	संरचना vir_addr fromaddr, toaddr;		\
	पूर्णाङ्क r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	सचे((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		पटिनिवत्त r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	संरचना vir_addr fromaddr, toaddr;		\
	पूर्णाङ्क r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	सचे((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		पटिनिवत्त r;\
	}  \
}

  सचे(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) पटिनिवत्त(EINVAL);
  सचे (iskerneln(tr_proc_nr)) पटिनिवत्त(EPERM);

  rp = proc_addr(tr_proc_nr);
  सचे (isemptyp(rp)) पटिनिवत्त(EINVAL);
  विकल्प (tr_request) {
  पक्ष T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	पटिनिवत्त(OK);

  पक्ष T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, परिमाण(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	भङ्ग;

  पक्ष T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, परिमाण(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	भङ्ग;

  पक्ष T_GETUSER:		/* return value from process table */
	सचे ((tr_addr & (परिमाण(long) - 1)) != 0) पटिनिवत्त(EFAULT);

	सचे (tr_addr <= परिमाण(संरचना proc) - परिमाण(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((वर्ण *) rp + (पूर्णाङ्क) tr_addr);
		भङ्ग;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = परिमाण(long) - 1;
	tr_addr -= (परिमाण(संरचना proc) + i) & ~i;

	सचे (tr_addr > परिमाण(संरचना priv) - परिमाण(long)) पटिनिवत्त(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((वर्ण *) rp->p_priv + (पूर्णाङ्क) tr_addr);
	भङ्ग;

  पक्ष T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, परिमाण(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, परिमाण(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_SETUSER:			/* set value in process table */
	सचे ((tr_addr & (परिमाण(reg_t) - 1)) != 0 ||
	     tr_addr > परिमाण(संरचना stackframe_s) - परिमाण(reg_t))
		पटिनिवत्त(EFAULT);
	i = (पूर्णाङ्क) tr_addr;
#सचे defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	सचे (i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.cs ||
	    i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.ds ||
	    i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.es ||
	    i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.gs ||
	    i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.fs ||
	    i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.ss)
		पटिनिवत्त(EFAULT);

	सचे (i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	अञ्ञथा
		*(reg_t *) ((वर्ण *) &rp->p_reg + i) = (reg_t) tr_data;
#नोचेद्यदि defined(__arm__)
	सचे (i == (पूर्णाङ्क) &((संरचना proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} अञ्ञथा {
		*(reg_t *) ((वर्ण *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  पक्ष T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  पक्ष T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	भङ्ग;

  पक्ष T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned वर्ण) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	भङ्ग;

  स्वभाव:
	पटिनिवत्त(EINVAL);
  }
  पटिनिवत्त(OK);
}

#endif /* USE_TRACE */
