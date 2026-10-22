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

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

ବାହ୍ୟ phys_bytes video_mem_vaddr;

ବାହ୍ୟ ଅକ୍ଷର *video_mem;

ସ୍ଥିର ଶୂନ୍ୟ setcr3(ସଂରଚନା proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	ପ୍ରତିଜ୍ଞା(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	ଯଦି(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
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
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_ମୂଲ୍ୟ = p->p_seg.p_cr3;
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ଫେରନ୍ତୁ OK;
	ସ୍ଥିତି VMCTL_FLUSHTLB:
	{
		reload_cr3();
		ଫେରନ୍ତୁ OK;
	}
	ସ୍ଥିତି VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_ମୂଲ୍ୟ);
		ଫେରନ୍ତୁ OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ଫେରନ୍ତୁ EINVAL;
}
