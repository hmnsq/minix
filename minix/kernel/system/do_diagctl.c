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

#અન્તર્ભાવ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
પૂર્ણાંક do_diagctl(સંરચના proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  સ્થિર અક્ષર mybuf[DIAG_BUFSIZE];
  પૂર્ણાંક s, i, proc_nr;

  પસંદગી (m_ptr->m_lsys_krn_sys_diagctl.code) {
    સ્થિતિ DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	જો(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		પાછા EINVAL;
	}
	જો((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		પાછા s;
	}
	માટે(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	પાછા OK;
    સ્થિતિ DIAGCTL_CODE_STACKTRACE:
	જો(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		પાછા EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	પાછા OK;
    સ્થિતિ DIAGCTL_CODE_REGISTER:
	જો (!(priv(caller)->s_flags & SYS_PROC))
		પાછા EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	જો (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	પાછા OK;
    સ્થિતિ DIAGCTL_CODE_UNREGISTER:
	જો (!(priv(caller)->s_flags & SYS_PROC))
		પાછા EPERM;
	priv(caller)->s_diag_sig = FALSE;
	પાછા OK;
    મૂળભૂત:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        પાછા(EINVAL);
  }
}

