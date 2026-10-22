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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_diagctl(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  ಸ್ಥಿರ ವರ್ಣ mybuf[DIAG_BUFSIZE];
  ಪೂರ್ಣಾಙ್ಕ s, i, proc_nr;

  ವಿಕಲ್ಪ (m_ptr->m_lsys_krn_sys_diagctl.code) {
    ಪಕ್ಷ DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ಯದಿ(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	}
	ಯದಿ((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ಪ್ರತ್ಯಾಗಮ s;
	}
	ಪ್ರತಿ(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪಕ್ಷ DIAGCTL_CODE_STACKTRACE:
	ಯದಿ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪಕ್ಷ DIAGCTL_CODE_REGISTER:
	ಯದಿ (!(priv(caller)->s_flags & SYS_PROC))
		ಪ್ರತ್ಯಾಗಮ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ಯದಿ (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪಕ್ಷ DIAGCTL_CODE_UNREGISTER:
	ಯದಿ (!(priv(caller)->s_flags & SYS_PROC))
		ಪ್ರತ್ಯಾಗಮ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ಪ್ರತ್ಯಾಗಮ OK;
    ಸ್ವಭಾವ:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        ಪ್ರತ್ಯಾಗಮ(EINVAL);
  }
}

