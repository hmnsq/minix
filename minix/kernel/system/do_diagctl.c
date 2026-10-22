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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_diagctl(ସଂରଚନା proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  ସ୍ଥିର ଅକ୍ଷର mybuf[DIAG_BUFSIZE];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା s, i, proc_nr;

  ବାଛ (m_ptr->m_lsys_krn_sys_diagctl.code) {
    ସ୍ଥିତି DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	ଯଦି(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ଫେରନ୍ତୁ EINVAL;
	}
	ଯଦି((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ଫେରନ୍ତୁ s;
	}
	ପାଇଁ(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	ଫେରନ୍ତୁ OK;
    ସ୍ଥିତି DIAGCTL_CODE_STACKTRACE:
	ଯଦି(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ଫେରନ୍ତୁ EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ଫେରନ୍ତୁ OK;
    ସ୍ଥିତି DIAGCTL_CODE_REGISTER:
	ଯଦି (!(priv(caller)->s_flags & SYS_PROC))
		ଫେରନ୍ତୁ EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	ଯଦି (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ଫେରନ୍ତୁ OK;
    ସ୍ଥିତି DIAGCTL_CODE_UNREGISTER:
	ଯଦି (!(priv(caller)->s_flags & SYS_PROC))
		ଫେରନ୍ତୁ EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ଫେରନ୍ତୁ OK;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        ଫେରନ୍ତୁ(EINVAL);
  }
}

