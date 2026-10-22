
#که_نه_تعریف_شوی _ARM_ACONST_H
#تعریف_کړه _ARM_ACONST_H

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

/* Program stack words and masks. */
#تعریف_کړه INIT_PSR      (PSR_USR32_ډېرپېښ | PSR_F)    /* initial psr */
#تعریف_کړه INIT_TASK_PSR (PSR_SVC32_ډېرپېښ | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#تعریف_کړه RESET_ویکټور                  0
#تعریف_کړه UNDEFINED_INST_ویکټور         1
#تعریف_کړه SUPERVISOR_CALL_ویکټور        2
#تعریف_کړه PREFETCH_ABORT_ویکټور         3
#تعریف_کړه معلومات_ABORT_ویکټور             4
#تعریف_کړه HYPERVISOR_CALL_ویکټور        5
#تعریف_کړه مداخله_ویکټور              6
#تعریف_کړه FAST_مداخله_ویکټور         7

/* Data abort helper */
#تعریف_کړه is_align_fault(fault_حالت) \
	((fault_حالت) == FAULT_ALIGN_0)

#تعریف_کړه is_trans_fault(fault_حالت) \
	(((fault_حالت) == FAULT_TRANS_S) || ((fault_حالت) == FAULT_TRANS_P))

#تعریف_کړه is_perm_fault(fault_حالت) \
	(((fault_حالت) == FAULT_PERM_S) || ((fault_حالت) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کړه ARM_ډېری_سر_RESERVED	(2 * حجم(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#تعریف_کړه سېټ_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#تعریف_کړه PG_ALLOCATEME ((phys_بایټونه)-1)

#د_که_پای /* _ARM_ACONST_H */
