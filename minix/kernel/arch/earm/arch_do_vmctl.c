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

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ttbr(ರಚನೆ proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	ಆದರೆ(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	ಆದರೆ(p->p_nr == VM_PROC_NR) {
		ಆದರೆ (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ arch_do_vmctl(
  ನೋಂದಣಿ ಸಂದೇಶ *m_ptr,	/* pointer to request message */
  ರಚನೆ proc *p
)
{
  ಆಯ್ಕೆ(m_ptr->SVMCTL_PARAM) {
	ಸ್ಥಿತಿ VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_ಮೌಲ್ಯ = p->p_seg.p_ttbr;
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_SETADDRSPACE:
		ಗಣ_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ಹಿಂತಿರುಗಿ OK;
	ಸ್ಥಿತಿ VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ಹಿಂತಿರುಗಿ OK;
	}
  }

  ಬರೆ("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ಹಿಂತಿರುಗಿ EINVAL;
}
