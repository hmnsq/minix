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
ಪೂರ್ಣಾಂಕ do_diagctl(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
  vir_ಬೈಟುಗಳು len, buf;
  ಸ್ಥಿರ ಅಕ್ಷರ mybuf[DIAG_BUFSIZE];
  ಪೂರ್ಣಾಂಕ s, i, proc_nr;

  ಆಯ್ಕೆ (m_ptr->m_lsys_krn_sys_diagctl.ಸಂಕೇತ_2) {
    ಸ್ಥಿತಿ DIAGCTL_ಸಂಕೇತ_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ಆದರೆ(len < 1 || len > DIAG_BUFSIZE) {
		ಬರೆ("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ಹಿಂತಿರುಗಿ EINVAL;
	}
	ಆದರೆ((s=ದತ್ತಾಂಶ_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_ಬೈಟುಗಳು) mybuf, len)) != OK) {
		ಬರೆ("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ಹಿಂತಿರುಗಿ s;
	}
	ಗಾಗಿ(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(ಕೊನೆ_ದ_KMESS);
	ಹಿಂತಿರುಗಿ OK;
    ಸ್ಥಿತಿ DIAGCTL_ಸಂಕೇತ_STACKTRACE:
	ಆದರೆ(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ಹಿಂತಿರುಗಿ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ಹಿಂತಿರುಗಿ OK;
    ಸ್ಥಿತಿ DIAGCTL_ಸಂಕೇತ_ರಿಜಿಸ್ಟರ್:
	ಆದರೆ (!(priv(caller)->s_flags & SYS_PROC))
		ಹಿಂತಿರುಗಿ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ಆದರೆ (kmess.km_ಗಾತ್ರ > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ಹಿಂತಿರುಗಿ OK;
    ಸ್ಥಿತಿ DIAGCTL_ಸಂಕೇತ_UNREGISTER:
	ಆದರೆ (!(priv(caller)->s_flags & SYS_PROC))
		ಹಿಂತಿರುಗಿ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ಹಿಂತಿರುಗಿ OK;
    ಪೂರ್ವನಿಯೋಜಿತ:
	ಬರೆ("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.ಸಂಕೇತ_2);
        ಹಿಂತಿರುಗಿ(EINVAL);
  }
}

