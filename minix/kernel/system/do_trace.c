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

#जर USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
पूर्णांक do_trace(संरचना proc * caller, message * m_ptr)
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

  नोंदणी संरचना proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  दीर्घ tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  पूर्णांक tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  पूर्णांक tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  अचिन्हित अक्षर ub;
  पूर्णांक i;

#define COPYTOPROC(addr, myaddr, length) {		\
	संरचना vir_addr fromaddr, toaddr;		\
	पूर्णांक r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	जर((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		म_लिखो("Can't copy in sys_trace: %d\n", r);\
		परत r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	संरचना vir_addr fromaddr, toaddr;		\
	पूर्णांक r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	जर((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		म_लिखो("Can't copy in sys_trace: %d\n", r);\
		परत r;\
	}  \
}

  जर(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) परत(EINVAL);
  जर (iskerneln(tr_proc_nr)) परत(EPERM);

  rp = proc_addr(tr_proc_nr);
  जर (isemptyp(rp)) परत(EINVAL);
  निवड (tr_request) {
  स्थिती T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	परत(OK);

  स्थिती T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, आकार(दीर्घ));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	थांबा;

  स्थिती T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, आकार(दीर्घ));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	थांबा;

  स्थिती T_GETUSER:		/* return value from process table */
	जर ((tr_addr & (आकार(दीर्घ) - 1)) != 0) परत(EFAULT);

	जर (tr_addr <= आकार(संरचना proc) - आकार(दीर्घ)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(दीर्घ *) ((अक्षर *) rp + (पूर्णांक) tr_addr);
		थांबा;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = आकार(दीर्घ) - 1;
	tr_addr -= (आकार(संरचना proc) + i) & ~i;

	जर (tr_addr > आकार(संरचना priv) - आकार(दीर्घ)) परत(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(दीर्घ *) ((अक्षर *) rp->p_priv + (पूर्णांक) tr_addr);
	थांबा;

  स्थिती T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, आकार(दीर्घ));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, आकार(दीर्घ));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_SETUSER:			/* set value in process table */
	जर ((tr_addr & (आकार(reg_t) - 1)) != 0 ||
	     tr_addr > आकार(संरचना stackframe_s) - आकार(reg_t))
		परत(EFAULT);
	i = (पूर्णांक) tr_addr;
#जर defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	जर (i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.cs ||
	    i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.ds ||
	    i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.es ||
	    i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.gs ||
	    i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.fs ||
	    i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.ss)
		परत(EFAULT);

	जर (i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	अन्यथा
		*(reg_t *) ((अक्षर *) &rp->p_reg + i) = (reg_t) tr_data;
#किंवाजर defined(__arm__)
	जर (i == (पूर्णांक) &((संरचना proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} अन्यथा {
		*(reg_t *) ((अक्षर *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  स्थिती T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  स्थिती T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	थांबा;

  स्थिती T_WRITEB_INS:		/* set value in instruction space */
	ub = (अचिन्हित अक्षर) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	थांबा;

  पूर्वनिर्धारित:
	परत(EINVAL);
  }
  परत(OK);
}

#endif /* USE_TRACE */
