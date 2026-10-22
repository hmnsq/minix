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

#അന്തര്ഭാവ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_diagctl(ഘടന proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  സ്ഥിതം അക്ഷരം mybuf[DIAG_BUFSIZE];
  പൂർണ്ണസംഖ്യ s, i, proc_nr;

  തിരഞ്ഞെടുപ്പ് (m_ptr->m_lsys_krn_sys_diagctl.code) {
    അവസ്ഥ DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	എങ്കിൽ(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		തിരികെ EINVAL;
	}
	എങ്കിൽ((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		തിരികെ s;
	}
	വേണ്ടി(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	തിരികെ OK;
    അവസ്ഥ DIAGCTL_CODE_STACKTRACE:
	എങ്കിൽ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		തിരികെ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	തിരികെ OK;
    അവസ്ഥ DIAGCTL_CODE_REGISTER:
	എങ്കിൽ (!(priv(caller)->s_flags & SYS_PROC))
		തിരികെ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	എങ്കിൽ (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	തിരികെ OK;
    അവസ്ഥ DIAGCTL_CODE_UNREGISTER:
	എങ്കിൽ (!(priv(caller)->s_flags & SYS_PROC))
		തിരികെ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	തിരികെ OK;
    സ്ഥിരസ്ഥിതി:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        തിരികെ(EINVAL);
  }
}

