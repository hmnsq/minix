/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#include "kernel/system.h"
#include <assert.h>
#include <minix/type.h>

#include "arch_proto.h"

static void set_ttbr(sa_nracanaa proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	assert(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	ပေႃး(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	ပေႃး(p->p_nr == VM_PROC_NR) {
		ပေႃး (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
puur_naa_nka arch_do_vmctl(
  pa_njikaa message *m_ptr,	/* pointer to request message */
  sa_nracanaa proc *p
)
{
  switch(m_ptr->SVMCTL_PARAM) {
	pak_sa VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_VALUE = p->p_seg.p_ttbr;
		ပွၵ်ႈ OK;
	pak_sa VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ပွၵ်ႈ OK;
	pak_sa VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		ပွၵ်ႈ OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ပွၵ်ႈ EINVAL;
}
