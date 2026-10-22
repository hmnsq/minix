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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
ਪੂਰਨ do_diagctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  vir_ਬਾਈਟਾਂ len, buf;
  ਸਥਿਰ ਅੱਖਰ mybuf[DIAG_BUFSIZE];
  ਪੂਰਨ s, i, proc_nr;

  ਚੋਣ (m_ptr->m_lsys_krn_sys_diagctl.ਕੋਡ) {
    ਹਾਲਤ DIAGCTL_ਕੋਡ_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ਜੇ(len < 1 || len > DIAG_BUFSIZE) {
		ਲਿਖੋ("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ਵਾਪਸ EINVAL;
	}
	ਜੇ((s=ਅੰਕੜੇ_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_ਬਾਈਟਾਂ) mybuf, len)) != OK) {
		ਲਿਖੋ("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ਵਾਪਸ s;
	}
	ਲਈ(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(ਸਿਰਾ_ਦਾ_KMESS);
	ਵਾਪਸ OK;
    ਹਾਲਤ DIAGCTL_ਕੋਡ_STACKTRACE:
	ਜੇ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ਵਾਪਸ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ਵਾਪਸ OK;
    ਹਾਲਤ DIAGCTL_ਕੋਡ_ਰਜਿਸਟਰ:
	ਜੇ (!(priv(caller)->s_flags & SYS_PROC))
		ਵਾਪਸ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ਜੇ (kmess.km_ਮਾਪ > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ਵਾਪਸ OK;
    ਹਾਲਤ DIAGCTL_ਕੋਡ_UNREGISTER:
	ਜੇ (!(priv(caller)->s_flags & SYS_PROC))
		ਵਾਪਸ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ਵਾਪਸ OK;
    ਮੂਲ:
	ਲਿਖੋ("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.ਕੋਡ);
        ਵਾਪਸ(EINVAL);
  }
}

