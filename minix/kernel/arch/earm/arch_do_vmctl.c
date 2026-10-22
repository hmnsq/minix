/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

ಸ್ಥಿರ ಶೂನ್ಯ set_ttbr(ಸಂರಚನಾ proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	ಯದಿ(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	ಯದಿ(p->p_nr == VM_PROC_NR) {
		ಯದಿ (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ arch_do_vmctl(
  ಪಞ್ಜಿಕಾ message *m_ptr,	/* pointer to request message */
  ಸಂರಚನಾ proc *p
)
{
  ವಿಕಲ್ಪ(m_ptr->SVMCTL_PARAM) {
	ಪಕ್ಷ VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_ಮೌಲ್ಯ = p->p_seg.p_ttbr;
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ಪ್ರತ್ಯಾಗಮ OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ಪ್ರತ್ಯಾಗಮ EINVAL;
}
