/* The kernel call implemented in this file:
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#समावेश "kernel/system.h"
#समावेश <assert.h>

#समावेश "arch_proto.h"

बाह्य phys_bytes video_mem_vaddr;

बाह्य अक्षर *video_mem;

अटल व्योम setcr3(काष्ठा proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	निश्चित(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	अगर(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
	}
	अगर(p->p_nr == VM_PROC_NR) {
		अगर (arch_enable_paging(p) != OK)
			panic("arch_enable_paging failed");
	}
	RTS_UNSET(p, RTS_VMINHIBIT);
}

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
पूर्णांक arch_do_vmctl(
  रेजिस्टर message *m_ptr,	/* pointer to request message */
  काष्ठा proc *p
)
{
  चयन(m_ptr->SVMCTL_PARAM) {
	हाल VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_मूल्य = p->p_seg.p_cr3;
		वापस OK;
	हाल VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		वापस OK;
	हाल VMCTL_FLUSHTLB:
	{
		reload_cr3();
		वापस OK;
	}
	हाल VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_मूल्य);
		वापस OK;
	}
  }



  म_लिखो("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  वापस EINVAL;
}
