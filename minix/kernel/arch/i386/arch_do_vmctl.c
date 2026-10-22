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

#अन्तर्भाव "arch_proto.h"

बाह्य phys_bytes video_mem_vaddr;

बाह्य अक्षर *video_mem;

स्थिर शून्य setcr3(संरचना proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	प्रतिज्ञा(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	जर(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	जर(p->p_nr == VM_PROC_NR) {
		जर (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
पूर्णांक arch_do_vmctl(
  नोंदणी message *m_ptr,	/* pointer to request message */
  संरचना proc *p
)
{
  निवड(m_ptr->SVMCTL_PARAM) {
	स्थिती VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_मूल्य = p->p_seg.p_cr3;
		परत OK;
	स्थिती VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		परत OK;
	स्थिती VMCTL_FLUSHTLB:
	{
		reload_cr3();
		परत OK;
	}
	स्थिती VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_मूल्य);
		परत OK;
	}
  }



  म_लिखो("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  परत EINVAL;
}
