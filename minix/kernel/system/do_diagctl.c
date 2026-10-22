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

#समावेश "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
पूर्णांक do_diagctl(काष्ठा proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  अटल अक्षर mybuf[DIAG_BUFSIZE];
  पूर्णांक s, i, proc_nr;

  चयन (m_ptr->m_lsys_krn_sys_diagctl.code) {
    हाल DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	अगर(len < 1 || len > DIAG_BUFSIZE) {
		म_लिखो("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		वापस EINVAL;
	}
	अगर((s=तथ्य_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		म_लिखो("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		वापस s;
	}
	क्रम(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	वापस OK;
    हाल DIAGCTL_CODE_STACKTRACE:
	अगर(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		वापस EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	वापस OK;
    हाल DIAGCTL_CODE_REGISTER:
	अगर (!(priv(caller)->s_flags & SYS_PROC))
		वापस EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	अगर (kmess.km_आकार > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	वापस OK;
    हाल DIAGCTL_CODE_UNREGISTER:
	अगर (!(priv(caller)->s_flags & SYS_PROC))
		वापस EPERM;
	priv(caller)->s_diag_sig = FALSE;
	वापस OK;
    शेष:
	म_लिखो("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        वापस(EINVAL);
  }
}

