
#جيڪڏهن_وصف_نه_ٿيل _ARM_ACONST_H
#وصف_ڪريو _ARM_ACONST_H

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

/* Program stack words and masks. */
#وصف_ڪريو INIT_PSR      (PSR_USR32_گهڻائي | PSR_F)    /* initial psr */
#وصف_ڪريو INIT_TASK_PSR (PSR_SVC32_گهڻائي | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#وصف_ڪريو RESET_ويڪٽر                  0
#وصف_ڪريو UNDEFINED_INST_ويڪٽر         1
#وصف_ڪريو SUPERVISOR_CALL_ويڪٽر        2
#وصف_ڪريو PREFETCH_ABORT_ويڪٽر         3
#وصف_ڪريو معلومات_ABORT_ويڪٽر             4
#وصف_ڪريو HYPERVISOR_CALL_ويڪٽر        5
#وصف_ڪريو مداخلت_ويڪٽر              6
#وصف_ڪريو FAST_مداخلت_ويڪٽر         7

/* Data abort helper */
#وصف_ڪريو is_align_fault(fault_حالت) \
	((fault_حالت) == FAULT_ALIGN_0)

#وصف_ڪريو is_trans_fault(fault_حالت) \
	(((fault_حالت) == FAULT_TRANS_S) || ((fault_حالت) == FAULT_TRANS_P))

#وصف_ڪريو is_perm_fault(fault_حالت) \
	(((fault_حالت) == FAULT_PERM_S) || ((fault_حالت) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#وصف_ڪريو ARM_ڍير_چوٽي_RESERVED	(2 * حجم(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#وصف_ڪريو سيٽ_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#وصف_ڪريو PG_ALLOCATEME ((phys_بائيٽون)-1)

#جيڪڏهن_پورو /* _ARM_ACONST_H */
