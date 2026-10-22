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

#include "arch_proto.h"

extern phys_bytes video_mem_vaddr;

extern var_na *video_mem;

static void setcr3(sa_nracanaa proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	assert(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	නම්(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	නම්(p->p_nr == VM_PROC_NR) {
		නම් (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
පූර්ණ_සංඛ්‍යා arch_do_vmctl(
  pa_njikaa message *m_ptr,	/* pointer to request message */
  sa_nracanaa proc *p
)
{
  switch(m_ptr->SVMCTL_PARAM) {
	pak_sa VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_VALUE = p->p_seg.p_cr3;
		ආපසු OK;
	pak_sa VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ආපසු OK;
	pak_sa VMCTL_FLUSHTLB:
	{
		reload_cr3();
		ආපසු OK;
	}
	pak_sa VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_VALUE);
		ආපසු OK;
	}
  }



  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ආපසු EINVAL;
}
