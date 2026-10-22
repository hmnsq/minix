/* The kernel call implemented in this file:
 *   m_type:	SYS_DIAGCTL
 *
 * The parameters for this kernel call are:
 * 	m_lsys_krn_sys_diagctl.code	request
 * and then request-specific arguments in
 *	m_lsys_krn_sys_diagctl.buf
 *	m_lsys_krn_sys_diagctl.len
 *	m_lsys_krn_sys_diagctl.endpt
 */

#include "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
цяло do_diagctl(struct proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  static char mybuf[DIAG_BUFSIZE];
  цяло s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
    case DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ако(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		върни EINVAL;
	}
	ако((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		върни s;
	}
	за(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	върни OK;
    case DIAGCTL_CODE_STACKTRACE:
	ако(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		върни EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	върни OK;
    case DIAGCTL_CODE_REGISTER:
	ако (!(priv(caller)->s_flags & SYS_PROC))
		върни EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ако (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	върни OK;
    case DIAGCTL_CODE_UNREGISTER:
	ако (!(priv(caller)->s_flags & SYS_PROC))
		върни EPERM;
	priv(caller)->s_diag_sig = FALSE;
	върни OK;
    default:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        върни(EINVAL);
  }
}

