/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ضمن "kernel/system.h"
#ضمن <assert.h>

#ضمن "arch_proto.h"

خارجي phys_bytes video_mem_vaddr;

خارجي حرف *video_mem;

ساكن فراغ setcr3(بنية proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	أكد(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	إذا(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	إذا(p->p_nr == VM_PROC_NR) {
		إذا (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
صحيح arch_do_vmctl(
  سجل message *m_ptr,	/* pointer to request message */
  بنية proc *p
)
{
  بدل(m_ptr->SVMCTL_PARAM) {
	حالة VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_درج = p->p_seg.p_cr3;
		أرجع OK;
	حالة VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		أرجع OK;
	حالة VMCTL_FLUSHTLB:
	{
		reload_cr3();
		أرجع OK;
	}
	حالة VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_درج);
		أرجع OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  أرجع EINVAL;
}
