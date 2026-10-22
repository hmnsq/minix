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

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

ಬಾಹ್ಯ phys_bytes video_mem_vaddr;

ಬಾಹ್ಯ ವರ್ಣ *video_mem;

ಸ್ಥಿರ ಶೂನ್ಯ setcr3(ಸಂರಚನಾ proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	ಯದಿ(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
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
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_ಮೌಲ್ಯ = p->p_seg.p_cr3;
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ಪ್ರತ್ಯಾಗಮ OK;
	ಪಕ್ಷ VMCTL_FLUSHTLB:
	{
		reload_cr3();
		ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಪಕ್ಷ VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_ಮೌಲ್ಯ);
		ಪ್ರತ್ಯಾಗಮ OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ಪ್ರತ್ಯಾಗಮ EINVAL;
}
