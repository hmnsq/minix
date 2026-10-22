
#اگر_تعریف_نشده _ARM_ACONST_H
#تعریف_کن _ARM_ACONST_H

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

/* Program stack words and masks. */
#تعریف_کن INIT_PSR      (PSR_USR32_نما | PSR_F)    /* initial psr */
#تعریف_کن INIT_TASK_PSR (PSR_SVC32_نما | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#تعریف_کن RESET_وکتور                  0
#تعریف_کن UNDEFINED_INST_وکتور         1
#تعریف_کن SUPERVISOR_CALL_وکتور        2
#تعریف_کن PREFETCH_ABORT_وکتور         3
#تعریف_کن داده_ABORT_وکتور             4
#تعریف_کن HYPERVISOR_CALL_وکتور        5
#تعریف_کن وقفه_وکتور              6
#تعریف_کن FAST_وقفه_وکتور         7

/* Data abort helper */
#تعریف_کن is_align_fault(fault_وضعیت) \
	((fault_وضعیت) == FAULT_ALIGN_0)

#تعریف_کن is_trans_fault(fault_وضعیت) \
	(((fault_وضعیت) == FAULT_TRANS_S) || ((fault_وضعیت) == FAULT_TRANS_P))

#تعریف_کن is_perm_fault(fault_وضعیت) \
	(((fault_وضعیت) == FAULT_PERM_S) || ((fault_وضعیت) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کن ARM_پشته_بالاترین_RESERVED	(2 * حجم(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#تعریف_کن مجموعه_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#تعریف_کن PG_ALLOCATEME ((phys_بایتها)-1)

#پایان_اگر /* _ARM_ACONST_H */
