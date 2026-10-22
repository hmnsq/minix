/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#شامل "kernel/system.h"
#شامل <assert.h>
#شامل <minix/type.h>

#شامل "arch_proto.h"

ایستا تهی set_ttbr(ساختار proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	أكد(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	اگر(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	اگر(p->p_nr == VM_PROC_NR) {
		اگر (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
صحیح arch_do_vmctl(
  ثبات message *m_ptr,	/* pointer to request message */
  ساختار proc *p
)
{
  انتخاب(m_ptr->SVMCTL_PARAM) {
	حالت VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_مقدار = p->p_seg.p_ttbr;
		بازگشت OK;
	حالت VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		بازگشت OK;
	حالت VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		بازگشت OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  بازگشت EINVAL;
}
