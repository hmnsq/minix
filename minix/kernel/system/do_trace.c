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

#include "kernel/system.h"
#include <sys/ptrace.h>

#만약 USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
정수 do_trace(struct proc * caller, message * m_ptr)
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

  register struct proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  정수 tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  정수 tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned char ub;
  정수 i;

#define COPYTOPROC(addr, myaddr, length) {		\
	struct vir_addr fromaddr, toaddr;		\
	정수 r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	만약((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		반환 r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	struct vir_addr fromaddr, toaddr;		\
	정수 r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	만약((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		반환 r;\
	}  \
}

  만약(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) 반환(EINVAL);
  만약 (iskerneln(tr_proc_nr)) 반환(EPERM);

  rp = proc_addr(tr_proc_nr);
  만약 (isemptyp(rp)) 반환(EINVAL);
  switch (tr_request) {
  case T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	반환(OK);

  case T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, sizeof(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	break;

  case T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, sizeof(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	break;

  case T_GETUSER:		/* return value from process table */
	만약 ((tr_addr & (sizeof(long) - 1)) != 0) 반환(EFAULT);

	만약 (tr_addr <= sizeof(struct proc) - sizeof(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((char *) rp + (정수) tr_addr);
		break;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = sizeof(long) - 1;
	tr_addr -= (sizeof(struct proc) + i) & ~i;

	만약 (tr_addr > sizeof(struct priv) - sizeof(long)) 반환(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((char *) rp->p_priv + (정수) tr_addr);
	break;

  case T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, sizeof(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, sizeof(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_SETUSER:			/* set value in process table */
	만약 ((tr_addr & (sizeof(reg_t) - 1)) != 0 ||
	     tr_addr > sizeof(struct stackframe_s) - sizeof(reg_t))
		반환(EFAULT);
	i = (정수) tr_addr;
#만약 defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	만약 (i == (정수) &((struct proc *) 0)->p_reg.cs ||
	    i == (정수) &((struct proc *) 0)->p_reg.ds ||
	    i == (정수) &((struct proc *) 0)->p_reg.es ||
	    i == (정수) &((struct proc *) 0)->p_reg.gs ||
	    i == (정수) &((struct proc *) 0)->p_reg.fs ||
	    i == (정수) &((struct proc *) 0)->p_reg.ss)
		반환(EFAULT);

	만약 (i == (정수) &((struct proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	아니면
		*(reg_t *) ((char *) &rp->p_reg + i) = (reg_t) tr_data;
#elif defined(__arm__)
	만약 (i == (정수) &((struct proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} 아니면 {
		*(reg_t *) ((char *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  case T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  case T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	break;

  case T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned char) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	break;

  default:
	반환(EINVAL);
  }
  반환(OK);
}

#endif /* USE_TRACE */
