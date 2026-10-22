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

#अन्तर्भाव "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
पूर्णाङ्क do_diagctl(संरचना proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  स्थिर वर्ण mybuf[DIAG_BUFSIZE];
  पूर्णाङ्क s, i, proc_nr;

  विकल्प (m_ptr->m_lsys_krn_sys_diagctl.code) {
    पक्ष DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	सचे(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		पटिनिवत्त EINVAL;
	}
	सचे((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		पटिनिवत्त s;
	}
	अत्थाय(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	पटिनिवत्त OK;
    पक्ष DIAGCTL_CODE_STACKTRACE:
	सचे(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		पटिनिवत्त EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	पटिनिवत्त OK;
    पक्ष DIAGCTL_CODE_REGISTER:
	सचे (!(priv(caller)->s_flags & SYS_PROC))
		पटिनिवत्त EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	सचे (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	पटिनिवत्त OK;
    पक्ष DIAGCTL_CODE_UNREGISTER:
	सचे (!(priv(caller)->s_flags & SYS_PROC))
		पटिनिवत्त EPERM;
	priv(caller)->s_diag_sig = FALSE;
	पटिनिवत्त OK;
    स्वभाव:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        पटिनिवत्त(EINVAL);
  }
}

