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
ចំនួនគត់ do_diagctl(sa_nracanaa proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  static var_na mybuf[DIAG_BUFSIZE];
  ចំនួនគត់ s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
    pak_sa DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ប្រសិនបើ(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ត្រឡប់ EINVAL;
	}
	ប្រសិនបើ((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ត្រឡប់ s;
	}
	សម្រាប់(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	ត្រឡប់ OK;
    pak_sa DIAGCTL_CODE_STACKTRACE:
	ប្រសិនបើ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ត្រឡប់ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ត្រឡប់ OK;
    pak_sa DIAGCTL_CODE_REGISTER:
	ប្រសិនបើ (!(priv(caller)->s_flags & SYS_PROC))
		ត្រឡប់ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ប្រសិនបើ (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ត្រឡប់ OK;
    pak_sa DIAGCTL_CODE_UNREGISTER:
	ប្រសិនបើ (!(priv(caller)->s_flags & SYS_PROC))
		ត្រឡប់ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ត្រឡប់ OK;
    default:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        ត្រឡប់(EINVAL);
  }
}

