/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <assert.h>

#અન્તર્ભાવ "arch_proto.h"

બાહ્ય phys_bytes video_mem_vaddr;

બાહ્ય અક્ષર *video_mem;

સ્થિર શૂન્ય setcr3(સંરચના proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	પ્રતિજ્ઞા(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	જો(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	જો(p->p_nr == VM_PROC_NR) {
		જો (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
પૂર્ણાંક arch_do_vmctl(
  નોંધણી message *m_ptr,	/* pointer to request message */
  સંરચના proc *p
)
{
  પસંદગી(m_ptr->SVMCTL_PARAM) {
	સ્થિતિ VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_કિંમત = p->p_seg.p_cr3;
		પાછા OK;
	સ્થિતિ VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		પાછા OK;
	સ્થિતિ VMCTL_FLUSHTLB:
	{
		reload_cr3();
		પાછા OK;
	}
	સ્થિતિ VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_કિંમત);
		પાછા OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  પાછા EINVAL;
}
