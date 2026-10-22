/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

ਬਾਹਰੀ phys_ਬਾਈਟਾਂ video_mem_vaddr;

ਬਾਹਰੀ ਅੱਖਰ *video_mem;

ਸਥਿਰ ਖਾਲੀ setcr3(ਬਣਤਰ proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	ਪ੍ਰਤਿਜ੍ਞਾ(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	ਜੇ(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	ਜੇ(p->p_nr == VM_PROC_NR) {
		ਜੇ (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
ਪੂਰਨ arch_do_vmctl(
  ਰਜਿਸਟਰ ਸੁਨੇਹਾ *m_ptr,	/* pointer to request message */
  ਬਣਤਰ proc *p
)
{
  ਚੋਣ(m_ptr->SVMCTL_PARAM) {
	ਹਾਲਤ VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_ਮੁੱਲ = p->p_seg.p_cr3;
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		ਵਾਪਸ OK;
	ਹਾਲਤ VMCTL_FLUSHTLB:
	{
		reload_cr3();
		ਵਾਪਸ OK;
	}
	ਹਾਲਤ VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_ਮੁੱਲ);
		ਵਾਪਸ OK;
	}
  }



  ਲਿਖੋ("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  ਵਾਪਸ EINVAL;
}
