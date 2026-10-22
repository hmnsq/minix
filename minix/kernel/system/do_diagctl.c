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

#அந்தர்பாவ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
முழு do_diagctl(கட்டமைப்பு proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  நிலையான எழுத்து mybuf[DIAG_BUFSIZE];
  முழு s, i, proc_nr;

  தேர்வு (m_ptr->m_lsys_krn_sys_diagctl.code) {
    நிலை DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	எனில்(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		திரும்பு EINVAL;
	}
	எனில்((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		திரும்பு s;
	}
	க்கு(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	திரும்பு OK;
    நிலை DIAGCTL_CODE_STACKTRACE:
	எனில்(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		திரும்பு EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	திரும்பு OK;
    நிலை DIAGCTL_CODE_REGISTER:
	எனில் (!(priv(caller)->s_flags & SYS_PROC))
		திரும்பு EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	எனில் (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	திரும்பு OK;
    நிலை DIAGCTL_CODE_UNREGISTER:
	எனில் (!(priv(caller)->s_flags & SYS_PROC))
		திரும்பு EPERM;
	priv(caller)->s_diag_sig = FALSE;
	திரும்பு OK;
    இயல்பு:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        திரும்பு(EINVAL);
  }
}

