/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <assert.h>

#అన్తర్భావ "arch_proto.h"

బాహ్య phys_బైట్లు video_mem_vaddr;

బాహ్య అక్షరం *video_mem;

స్థిర శూన్య setcr3(నిర్మాణం proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	ప్రతిజ్ఞా(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	ఒకవేళ(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	ఒకవేళ(p->p_nr == VM_PROC_NR) {
		ఒకవేళ (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
పూర్ణాంకం arch_do_vmctl(
  నమోదు సందేశం *m_ptr,	/* pointer to request message */
  నిర్మాణం proc *p
)
{
  ఎంపిక(m_ptr->SVMCTL_PARAM) {
	స్థితి VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_విలువ = p->p_seg.p_cr3;
		తిరిగి OK;
	స్థితి VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		తిరిగి OK;
	స్థితి VMCTL_FLUSHTLB:
	{
		reload_cr3();
		తిరిగి OK;
	}
	స్థితి VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_విలువ);
		తిరిగి OK;
	}
  }



  రాయి("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  తిరిగి EINVAL;
}
