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

#అన్తర్భావ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
పూర్ణాంకం do_diagctl(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  vir_బైట్లు len, buf;
  స్థిర అక్షరం mybuf[DIAG_BUFSIZE];
  పూర్ణాంకం s, i, proc_nr;

  ఎంపిక (m_ptr->m_lsys_krn_sys_diagctl.సంకేతం_2) {
    స్థితి DIAGCTL_సంకేతం_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ఒకవేళ(len < 1 || len > DIAG_BUFSIZE) {
		రాయి("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		తిరిగి EINVAL;
	}
	ఒకవేళ((s=దత్తాంశం_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_బైట్లు) mybuf, len)) != OK) {
		రాయి("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		తిరిగి s;
	}
	కొరకు(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(చివర_యొక్క_KMESS);
	తిరిగి OK;
    స్థితి DIAGCTL_సంకేతం_STACKTRACE:
	ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		తిరిగి EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	తిరిగి OK;
    స్థితి DIAGCTL_సంకేతం_రిజిస్టరు:
	ఒకవేళ (!(priv(caller)->s_flags & SYS_PROC))
		తిరిగి EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ఒకవేళ (kmess.km_కొలత > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	తిరిగి OK;
    స్థితి DIAGCTL_సంకేతం_UNREGISTER:
	ఒకవేళ (!(priv(caller)->s_flags & SYS_PROC))
		తిరిగి EPERM;
	priv(caller)->s_diag_sig = FALSE;
	తిరిగి OK;
    అప్రమేయ:
	రాయి("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.సంకేతం_2);
        తిరిగి(EINVAL);
  }
}

