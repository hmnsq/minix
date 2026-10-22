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
पूर्णांक do_diagctl(संरचना proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  स्थिर अक्षर mybuf[DIAG_BUFSIZE];
  पूर्णांक s, i, proc_nr;

  निवड (m_ptr->m_lsys_krn_sys_diagctl.code) {
    स्थिती DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	जर(len < 1 || len > DIAG_BUFSIZE) {
		म_लिखो("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		परत EINVAL;
	}
	जर((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		म_लिखो("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		परत s;
	}
	साठी(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	परत OK;
    स्थिती DIAGCTL_CODE_STACKTRACE:
	जर(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		परत EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	परत OK;
    स्थिती DIAGCTL_CODE_REGISTER:
	जर (!(priv(caller)->s_flags & SYS_PROC))
		परत EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	जर (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	परत OK;
    स्थिती DIAGCTL_CODE_UNREGISTER:
	जर (!(priv(caller)->s_flags & SYS_PROC))
		परत EPERM;
	priv(caller)->s_diag_sig = FALSE;
	परत OK;
    पूर्वनिर्धारित:
	म_लिखो("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        परत(EINVAL);
  }
}

