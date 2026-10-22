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

#অন্তর্ভাব "arch_proto.h"

বাহ্যিক phys_bytes video_mem_vaddr;

বাহ্যিক অক্ষর *video_mem;

স্থির শূন্য setcr3(কাঠামো proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	প্রতিজ্ঞা(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	যদি(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
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
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_মান = p->p_seg.p_cr3;
		ফেরত OK;
	ক্ষেত্র VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ফেরত OK;
	ক্ষেত্র VMCTL_FLUSHTLB:
	{
		reload_cr3();
		ফেরত OK;
	}
	ক্ষেত্র VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_মান);
		ফেরত OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ফেরত EINVAL;
}
