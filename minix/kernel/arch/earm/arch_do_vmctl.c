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
#ضمن <minix/type.h>

#ضمن "arch_proto.h"

ساكن فراغ set_ttbr(بنية proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	أكد(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	إذا(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
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
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_قيمة = p->p_seg.p_ttbr;
		أرجع OK;
	حالة VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		أرجع OK;
	حالة VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		أرجع OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  أرجع EINVAL;
}
