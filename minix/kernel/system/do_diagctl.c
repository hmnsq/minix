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
정수 do_diagctl(struct proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  static char mybuf[DIAG_BUFSIZE];
  정수 s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
    case DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	만약(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		반환 EINVAL;
	}
	만약((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		반환 s;
	}
	위해(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	반환 OK;
    case DIAGCTL_CODE_STACKTRACE:
	만약(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		반환 EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	반환 OK;
    case DIAGCTL_CODE_REGISTER:
	만약 (!(priv(caller)->s_flags & SYS_PROC))
		반환 EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	만약 (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	반환 OK;
    case DIAGCTL_CODE_UNREGISTER:
	만약 (!(priv(caller)->s_flags & SYS_PROC))
		반환 EPERM;
	priv(caller)->s_diag_sig = FALSE;
	반환 OK;
    default:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        반환(EINVAL);
  }
}

