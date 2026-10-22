/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <minix/type.h>

#অন্তর্ভাৱ "arch_proto.h"

স্থির শূন্য set_ttbr(সংরচনা proc *p, u32_t ttbr, u32_t *v)
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
পূর্ণাঙ্ক arch_do_vmctl(
  পঞ্জিকা message *m_ptr,	/* pointer to request message */
  সংরচনা proc *p
)
{
  ৱিকল্প(m_ptr->SVMCTL_PARAM) {
	পক্ষ VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_মান = p->p_seg.p_ttbr;
		ঘূৰাই OK;
	পক্ষ VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ঘূৰাই OK;
	পক্ষ VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ঘূৰাই OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ঘূৰাই EINVAL;
}
