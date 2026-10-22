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
صحيح do_diagctl(بنية proc * caller, پيغام * m_ptr)
{
  vir_بائيٽون len, buf;
  ساكن حرف mybuf[DIAG_BUFSIZE];
  صحيح s, i, proc_nr;

  بدل (m_ptr->m_lsys_krn_sys_diagctl.ڪوڊ) {
    حالة DIAGCTL_ڪوڊ_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	إذا(len < 1 || len > DIAG_BUFSIZE) {
		لکو("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		أرجع EINVAL;
	}
	إذا((s=معلومات_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_بائيٽون) mybuf, len)) != OK) {
		لکو("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		أرجع s;
	}
	لكل(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(پڇاڙي_جو_KMESS);
	أرجع OK;
    حالة DIAGCTL_ڪوڊ_STACKTRACE:
	إذا(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		أرجع EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	أرجع OK;
    حالة DIAGCTL_ڪوڊ_رجسٽر:
	إذا (!(priv(caller)->s_flags & SYS_PROC))
		أرجع EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	إذا (kmess.km_سائيز > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	أرجع OK;
    حالة DIAGCTL_ڪوڊ_UNREGISTER:
	إذا (!(priv(caller)->s_flags & SYS_PROC))
		أرجع EPERM;
	priv(caller)->s_diag_sig = FALSE;
	أرجع OK;
    افتراضي:
	لکو("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.ڪوڊ);
        أرجع(EINVAL);
  }
}

