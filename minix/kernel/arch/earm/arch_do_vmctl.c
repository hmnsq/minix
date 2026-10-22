/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <minix/type.h>

#অন্তর্ভাব "arch_proto.h"

স্থির শূন্য set_ttbr(কাঠামো proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	প্রতিজ্ঞা(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	যদি(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	যদি(p->p_nr == VM_PROC_NR) {
		যদি (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
পূর্ণসংখ্যা arch_do_vmctl(
  নিবন্ধ message *m_ptr,	/* pointer to request message */
  কাঠামো proc *p
)
{
  নির্বাচন(m_ptr->SVMCTL_PARAM) {
	ক্ষেত্র VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_মান = p->p_seg.p_ttbr;
		ফেরত OK;
	ক্ষেত্র VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ফেরত OK;
	ক্ষেত্র VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ফেরত OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ফেরত EINVAL;
}
