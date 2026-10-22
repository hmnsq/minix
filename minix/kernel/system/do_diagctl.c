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

#include "kernel/system.h"


/*===========================================================================*
 *			        do_diagctl				     *
 *===========================================================================*/
ကိန်းပြည့် do_diagctl(sa_nracanaa proc * caller, message * m_ptr)
{
  vir_bytes len, buf;
  static var_na mybuf[DIAG_BUFSIZE];
  ကိန်းပြည့် s, i, proc_nr;

  switch (m_ptr->m_lsys_krn_sys_diagctl.code) {
    pak_sa DIAGCTL_CODE_DIAG:
        buf = m_ptr->m_lsys_krn_sys_diagctl.buf;
        len = m_ptr->m_lsys_krn_sys_diagctl.len;
	အကယ်၍(len < 1 || len > DIAG_BUFSIZE) {
		printf("do_diagctl: diag for %d: len %d out of range\n",
			caller->p_endpoint, len);
		ပြန်ပေး EINVAL;
	}
	အကယ်၍((s=data_copy_vmcheck(caller, caller->p_endpoint, buf, KERNEL,
					(vir_bytes) mybuf, len)) != OK) {
		printf("do_diagctl: diag for %d: len %d: copy failed: %d\n",
			caller->p_endpoint, len, s);
		ပြန်ပေး s;
	}
	အတွက်(i = 0; i < len; i++)
		kputc(mybuf[i]);
	kputc(END_OF_KMESS);
	ပြန်ပေး OK;
    pak_sa DIAGCTL_CODE_STACKTRACE:
	အကယ်၍(!isokendpt(m_ptr->m_lsys_krn_sys_diagctl.endpt, &proc_nr))
		ပြန်ပေး EINVAL;
	proc_stacktrace(proc_addr(proc_nr));
	ပြန်ပေး OK;
    pak_sa DIAGCTL_CODE_REGISTER:
	အကယ်၍ (!(priv(caller)->s_flags & SYS_PROC))
		ပြန်ပေး EPERM;
	priv(caller)->s_diag_sig = TRUE;
	/* If the message log is not empty, send a first notification
	 * immediately. After bootup the log is basically never empty.
	 */
	အကယ်၍ (kmess.km_size > 0 && !kinfo.do_serial_debug)
		send_sig(caller->p_endpoint, SIGKMESS);
	ပြန်ပေး OK;
    pak_sa DIAGCTL_CODE_UNREGISTER:
	အကယ်၍ (!(priv(caller)->s_flags & SYS_PROC))
		ပြန်ပေး EPERM;
	priv(caller)->s_diag_sig = FALSE;
	ပြန်ပေး OK;
    default:
	printf("do_diagctl: invalid request %d\n", m_ptr->m_lsys_krn_sys_diagctl.code);
        ပြန်ပေး(EINVAL);
  }
}

