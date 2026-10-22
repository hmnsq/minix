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
صحیح do_diagctl(ساخت proc * caller, پیغام * m_ptr)
{
  vir_بائٹیں len, buf;
  ساکن حرف mybuf[DIAG_BUFSIZE];
  صحیح s, i, proc_nr;

  انتخاب (m_ptr->m_lsys_krn_sys_diagctl.رمز) {
    حالت DIAGCTL_رمز_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	اگر(len < 1 || len > DIAG_BUFSIZE) {
		لکھو("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		واپس EINVAL;
	}
	اگر((s=ڈیٹا_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_بائٹیں) mybuf, len)) != OK) {
		لکھو("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		واپس s;
	}
	برائے(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(انتہا_کا_KMESS);
	واپس OK;
    حالت DIAGCTL_رمز_STACKTRACE:
	اگر(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		واپس EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	واپس OK;
    حالت DIAGCTL_رمز_رجسٹر:
	اگر (!(priv(caller)->s_flags & SYS_PROC))
		واپس EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	اگر (kmess.km_سائز > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	واپس OK;
    حالت DIAGCTL_رمز_UNREGISTER:
	اگر (!(priv(caller)->s_flags & SYS_PROC))
		واپس EPERM;
	priv(caller)->s_diag_sig = FALSE;
	واپس OK;
    طےشدہ:
	لکھو("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.رمز);
        واپس(EINVAL);
  }
}

