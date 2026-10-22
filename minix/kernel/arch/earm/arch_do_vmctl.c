/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <minix/type.h>

#அந்தர்பாவ "arch_proto.h"

நிலையான வெறுமை set_ttbr(கட்டமைப்பு proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	ப்ரதிஜ்ஞா(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	எனில்(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	எனில்(p->p_nr == VM_PROC_NR) {
		எனில் (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
முழு arch_do_vmctl(
  பதிவு message *m_ptr,	/* pointer to request message */
  கட்டமைப்பு proc *p
)
{
  தேர்வு(m_ptr->SVMCTL_PARAM) {
	நிலை VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_மதிப்பு = p->p_seg.p_ttbr;
		திரும்பு OK;
	நிலை VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		திரும்பு OK;
	நிலை VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		திரும்பு OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  திரும்பு EINVAL;
}
