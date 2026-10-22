/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <minix/type.h>

#അന്തര്ഭാവ "arch_proto.h"

സ്ഥിതം ശൂന്യം set_ttbr(ഘടന proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	പ്രതിജ്ഞാ(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	എങ്കിൽ(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	എങ്കിൽ(p->p_nr == VM_PROC_NR) {
		എങ്കിൽ (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ arch_do_vmctl(
  രജിസ്റ്റർ message *m_ptr,	/* pointer to request message */
  ഘടന proc *p
)
{
  തിരഞ്ഞെടുപ്പ്(m_ptr->SVMCTL_PARAM) {
	അവസ്ഥ VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_മൂല്യം = p->p_seg.p_ttbr;
		തിരികെ OK;
	അവസ്ഥ VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		തിരികെ OK;
	അവസ്ഥ VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		തിരികെ OK;
	}
  }

  printf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  തിരികെ EINVAL;
}
