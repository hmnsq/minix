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

#ضمن "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
صحيح do_diagctl(بنية proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  ساكن حرف mybuf[DIAG_BUFSIZE];
  صحيح s, i, proc_nr;

  بدل (m_ptr->m_lsys_krn_sys_diagctl.code) {
    حالة DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	אם(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		החזר EINVAL;
	}
	אם((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		החזר s;
	}
	עבור(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	החזר OK;
    حالة DIAGCTL_CODE_STACKTRACE:
	אם(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		החזר EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	החזר OK;
    حالة DIAGCTL_CODE_REGISTER:
	אם (!(priv(caller)->s_flags & SYS_PROC))
		החזר EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	אם (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	החזר OK;
    حالة DIAGCTL_CODE_UNREGISTER:
	אם (!(priv(caller)->s_flags & SYS_PROC))
		החזר EPERM;
	priv(caller)->s_diag_sig = FALSE;
	החזר OK;
    افتراضي:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        החזר(EINVAL);
  }
}

