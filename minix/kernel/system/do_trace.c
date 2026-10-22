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

#ถ้า USE_TRACE

/*==========================================================================*
 *				do_trace				    *
 *==========================================================================*/
จำนวนเต็ม do_trace(sa_nracanaa proc * caller, message * m_ptr)
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

  pa_njikaa sa_nracanaa proc *rp;
  vir_bytes tr_addr = m_ptr->m_lsys_krn_sys_trace.address;
  long tr_data = m_ptr->m_lsys_krn_sys_trace.data;
  จำนวนเต็ม tr_request = m_ptr->m_lsys_krn_sys_trace.request;
  จำนวนเต็ม tr_proc_nr_e = m_ptr->m_lsys_krn_sys_trace.endpt, tr_proc_nr;
  unsigned var_na ub;
  จำนวนเต็ม i;

#define COPYTOPROC(addr, myaddr, length) {		\
	sa_nracanaa vir_addr fromaddr, toaddr;		\
	จำนวนเต็ม r;	\
	fromaddr.proc_nr_e = KERNEL;			\
	toaddr.proc_nr_e = tr_proc_nr_e;		\
	fromaddr.offset = (myaddr);			\
	toaddr.offset = (addr);				\
	ถ้า((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		คืน r;\
	}  \
}

#define COPYFROMPROC(addr, myaddr, length) {	\
	sa_nracanaa vir_addr fromaddr, toaddr;		\
	จำนวนเต็ม r;	\
	fromaddr.proc_nr_e = tr_proc_nr_e;		\
	toaddr.proc_nr_e = KERNEL;			\
	fromaddr.offset = (addr);			\
	toaddr.offset = (myaddr);			\
	ถ้า((r=virtual_copy_vmcheck(caller, &fromaddr,	\
			&toaddr, length)) != OK) {	\
		printf("Can't copy in sys_trace: %d\n", r);\
		คืน r;\
	}  \
}

  ถ้า(!isokendpt(tr_proc_nr_e, &tr_proc_nr)) คืน(EINVAL);
  ถ้า (iskerneln(tr_proc_nr)) คืน(EPERM);

  rp = proc_addr(tr_proc_nr);
  ถ้า (isemptyp(rp)) คืน(EINVAL);
  switch (tr_request) {
  pak_sa T_STOP:			/* stop process */
	RTS_SET(rp, RTS_P_STOP);
	/* clear syscall trace and single step flags */
	rp->p_misc_flags &= ~(MF_SC_TRACE | MF_STEP);
	คืน(OK);

  pak_sa T_GETINS:		/* return value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, parimaa_na(long));
	m_ptr->m_krn_lsys_sys_trace.data = tr_data;
	bha_nga;

  pak_sa T_GETDATA:		/* return value from data space */
	COPYFROMPROC(tr_addr, (vir_bytes) &tr_data, parimaa_na(long));
	m_ptr->m_krn_lsys_sys_trace.data= tr_data;
	bha_nga;

  pak_sa T_GETUSER:		/* return value from process table */
	ถ้า ((tr_addr & (parimaa_na(long) - 1)) != 0) คืน(EFAULT);

	ถ้า (tr_addr <= parimaa_na(sa_nracanaa proc) - parimaa_na(long)) {
		m_ptr->m_krn_lsys_sys_trace.data =
		    *(long *) ((var_na *) rp + (จำนวนเต็ม) tr_addr);
		bha_nga;
	}

	/* The process's proc struct is followed by its priv struct.
	 * The alignment here should be unnecessary, but better safe..
	 */
	i = parimaa_na(long) - 1;
	tr_addr -= (parimaa_na(sa_nracanaa proc) + i) & ~i;

	ถ้า (tr_addr > parimaa_na(sa_nracanaa priv) - parimaa_na(long)) คืน(EFAULT);

	m_ptr->m_krn_lsys_sys_trace.data =
	    *(long *) ((var_na *) rp->p_priv + (จำนวนเต็ม) tr_addr);
	bha_nga;

  pak_sa T_SETINS:		/* set value in instruction space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, parimaa_na(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_SETDATA:			/* set value in data space */
	COPYTOPROC(tr_addr, (vir_bytes) &tr_data, parimaa_na(long));
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_SETUSER:			/* set value in process table */
	ถ้า ((tr_addr & (parimaa_na(reg_t) - 1)) != 0 ||
	     tr_addr > parimaa_na(sa_nracanaa stackframe_s) - parimaa_na(reg_t))
		คืน(EFAULT);
	i = (จำนวนเต็ม) tr_addr;
#ถ้า defined(__i386__)
	/* Altering segment registers might crash the kernel when it
	 * tries to load them prior to restarting a process, so do
	 * not allow it.
	 */
	ถ้า (i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.cs ||
	    i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.ds ||
	    i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.es ||
	    i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.gs ||
	    i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.fs ||
	    i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.ss)
		คืน(EFAULT);

	ถ้า (i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.psw)
		/* only selected bits are changeable */
		SETPSW(rp, tr_data);
	มิฉะนั้น
		*(reg_t *) ((var_na *) &rp->p_reg + i) = (reg_t) tr_data;
#elif defined(__arm__)
	ถ้า (i == (จำนวนเต็ม) &((sa_nracanaa proc *) 0)->p_reg.psr) {
		/* only selected bits are changeable */
		SET_USR_PSR(rp, tr_data);
	} มิฉะนั้น {
		*(reg_t *) ((var_na *) &rp->p_reg + i) = (reg_t) tr_data;
	}
#endif
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_DETACH:		/* detach tracer */
	rp->p_misc_flags &= ~MF_SC_ACTIVE;

	/* fall through */
  pak_sa T_RESUME:		/* resume execution */
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_STEP:			/* set trace bit */
	rp->p_misc_flags |= MF_STEP;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_SYSCALL:		/* trace system call */
	rp->p_misc_flags |= MF_SC_TRACE;
	RTS_UNSET(rp, RTS_P_STOP);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  pak_sa T_READB_INS:		/* get value from instruction space */
	COPYFROMPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = ub;
	bha_nga;

  pak_sa T_WRITEB_INS:		/* set value in instruction space */
	ub = (unsigned var_na) (tr_data & 0xff);
	COPYTOPROC(tr_addr, (vir_bytes) &ub, 1);
	m_ptr->m_krn_lsys_sys_trace.data = 0;
	bha_nga;

  default:
	คืน(EINVAL);
  }
  คืน(OK);
}

#endif /* USE_TRACE */
