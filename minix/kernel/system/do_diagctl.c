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

#অন্তর্ভাব "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_diagctl(সংরচনা proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  স্থির বর্ণ mybuf[DIAG_BUFSIZE];
  পূর্ণাঙ্ক s, i, proc_nr;

  বিকল্প (m_ptr->m_lsys_krn_sys_diagctl.code) {
    পক্ষ DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	যদি(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		প্রত্যাগম EINVAL;
	}
	যদি((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		প্রত্যাগম s;
	}
	প্রতি(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	প্রত্যাগম OK;
    পক্ষ DIAGCTL_CODE_STACKTRACE:
	যদি(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		প্রত্যাগম EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	প্রত্যাগম OK;
    পক্ষ DIAGCTL_CODE_REGISTER:
	যদি (!(priv(caller)->s_flags & SYS_PROC))
		প্রত্যাগম EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	যদি (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	প্রত্যাগম OK;
    পক্ষ DIAGCTL_CODE_UNREGISTER:
	যদি (!(priv(caller)->s_flags & SYS_PROC))
		প্রত্যাগম EPERM;
	priv(caller)->s_diag_sig = FALSE;
	প্রত্যাগম OK;
    স্বভাব:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        প্রত্যাগম(EINVAL);
  }
}

