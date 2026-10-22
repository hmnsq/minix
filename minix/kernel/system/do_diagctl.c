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
	यदि(len < 1 || len > DIAG_BUFSIZE) {
		म_लिखो("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		प्रत्यागम EINVAL;
	}
	यदि((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		म_लिखो("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		प्रत्यागम s;
	}
	प्रति(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	प्रत्यागम OK;
    पक्ष DIAGCTL_CODE_STACKTRACE:
	यदि(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		प्रत्यागम EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	प्रत्यागम OK;
    पक्ष DIAGCTL_CODE_REGISTER:
	यदि (!(priv(caller)->s_flags & SYS_PROC))
		प्रत्यागम EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	यदि (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	प्रत्यागम OK;
    पक्ष DIAGCTL_CODE_UNREGISTER:
	यदि (!(priv(caller)->s_flags & SYS_PROC))
		प्रत्यागम EPERM;
	priv(caller)->s_diag_sig = FALSE;
	प्रत्यागम OK;
    स्वभाव:
	म_लिखो("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        प्रत्यागम(EINVAL);
  }
}

