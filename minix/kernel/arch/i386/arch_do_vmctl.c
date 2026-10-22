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

#شامل "arch_proto.h"

خارجی phys_بائٹیں video_mem_vaddr;

خارجی حرف *video_mem;

ساکن خالی setcr3(ساخت proc *p, u32_t cr3, u32_t *v)
{
	/* Set process CR3. */
	p->p_seg.p_cr3 = cr3;
	أكد(p->p_seg.p_cr3);
	p->p_seg.p_cr3_v = v; 
	اگر(p == get_cpulocal_var(ptproc)) {
		write_cr3(p->p_seg.p_cr3);
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
  اندراج پیغام *m_ptr,	/* pointer to request message */
  ساخت proc *p
)
{
  انتخاب(m_ptr->SVMCTL_PARAM) {
	حالت VMCTL_GET_PDBR:
		/* Get process page directory base reg (CR3). */
		m_ptr->SVMCTL_قدر = p->p_seg.p_cr3;
		واپس OK;
	حالت VMCTL_SETADDRSPACE:
		setcr3(p, m_ptr->SVMCTL_PTROOT, (u32_t *) m_ptr->SVMCTL_PTROOT_V);
		واپس OK;
	حالت VMCTL_FLUSHTLB:
	{
		reload_cr3();
		واپس OK;
	}
	حالت VMCTL_I386_INVLPG:
	{
		i386_invlpg(m_ptr->SVMCTL_قدر);
		واپس OK;
	}
  }



  لکھو("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  واپس EINVAL;
}
