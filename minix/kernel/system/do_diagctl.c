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

#شامل "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
صحیح do_diagctl(ساختار proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  ایستا نویسه mybuf[DIAG_BUFSIZE];
  صحیح s, i, proc_nr;

  انتخاب (m_ptr->m_lsys_krn_sys_diagctl.code) {
    حالت DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	اگر(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		بازگشت EINVAL;
	}
	اگر((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		بازگشت s;
	}
	برای(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	بازگشت OK;
    حالت DIAGCTL_CODE_STACKTRACE:
	اگر(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		بازگشت EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	بازگشت OK;
    حالت DIAGCTL_CODE_REGISTER:
	اگر (!(priv(caller)->s_flags & SYS_PROC))
		بازگشت EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	اگر (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	بازگشت OK;
    حالت DIAGCTL_CODE_UNREGISTER:
	اگر (!(priv(caller)->s_flags & SYS_PROC))
		بازگشت EPERM;
	priv(caller)->s_diag_sig = FALSE;
	بازگشت OK;
    پیشفرض:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        بازگشت(EINVAL);
  }
}

