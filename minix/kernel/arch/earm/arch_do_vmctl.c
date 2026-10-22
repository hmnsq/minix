/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <minix/type.h>

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

ସ୍ଥିର ଶୂନ୍ୟ set_ttbr(ସଂରଚନା proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	ପ୍ରତିଜ୍ଞା(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	ଯଦି(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	ଯଦି(p->p_nr == VM_PROC_NR) {
		ଯଦି (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_do_vmctl(
  ପଞ୍ଜିକରଣ message *m_ptr,	/* pointer to request message */
  ସଂରଚନା proc *p
)
{
  ବାଛ(m_ptr->SVMCTL_PARAM) {
	ସ୍ଥିତି VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_ମୂଲ୍ୟ = p->p_seg.p_ttbr;
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ଫେରନ୍ତୁ OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ଫେରନ୍ତୁ EINVAL;
}
