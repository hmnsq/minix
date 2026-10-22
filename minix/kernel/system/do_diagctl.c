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
	𐡄𐡍(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		أرجع EINVAL;
	}
	𐡄𐡍((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		أرجع s;
	}
	لكل(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	أرجع OK;
    حالة DIAGCTL_CODE_STACKTRACE:
	𐡄𐡍(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		أرجع EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	أرجع OK;
    حالة DIAGCTL_CODE_REGISTER:
	𐡄𐡍 (!(priv(caller)->s_flags & SYS_PROC))
		أرجع EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	𐡄𐡍 (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	أرجع OK;
    حالة DIAGCTL_CODE_UNREGISTER:
	𐡄𐡍 (!(priv(caller)->s_flags & SYS_PROC))
		أرجع EPERM;
	priv(caller)->s_diag_sig = FALSE;
	أرجع OK;
    افتراضي:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        أرجع(EINVAL);
  }
}

