/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <assert.h>
#अन्तर्भाव <minix/type.h>

#अन्तर्भाव "arch_proto.h"

स्थिर शून्य set_ttbr(संरचना proc *p, u32_t ttbr, u32_t *v)
{
	/* Set process TTBR. */
	p->p_seg.p_ttbr = ttbr;
	प्रतिज्ञा(p->p_seg.p_ttbr);
	p->p_seg.p_ttbr_v = v;
	यदि(p == get_cpulocal_var(ptproc)) {
		write_ttbr0(p->p_seg.p_ttbr);
	}
	यदि(p->p_nr == VM_PROC_NR) {
		यदि (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
पूर्णाङ्क arch_do_vmctl(
  पञ्जिका message *m_ptr,	/* pointer to request message */
  संरचना proc *p
)
{
  विकल्प(m_ptr->SVMCTL_PARAM) {
	पक्ष VMCTL_GET_PDBR:
		/* Get process page directory base reg (TTBR). */
		m_ptr->SVMCTL_मूल्य = p->p_seg.p_ttbr;
		प्रत्यागम OK;
	पक्ष VMCTL_SETADDRSPACE:
		set_ttbr(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		प्रत्यागम OK;
	पक्ष VMCTL_FLUSHTLB:
	{
		reload_ttbr0();
		प्रत्यागम OK;
	}
  }

  म_लिखो("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  प्रत्यागम EINVAL;
}
