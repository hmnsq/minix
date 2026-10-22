
#جے_بنا_تعریف _ARM_ACONST_H
#تعریف_کرو _ARM_ACONST_H

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

/* Program stack words and masks. */
#تعریف_کرو INIT_PSR      (PSR_USR32_منوال | PSR_F)    /* initial psr */
#تعریف_کرو INIT_TASK_PSR (PSR_SVC32_منوال | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#تعریف_کرو RESET_سمتیہ                  0
#تعریف_کرو UNDEFINED_INST_سمتیہ         1
#تعریف_کرو SUPERVISOR_CALL_سمتیہ        2
#تعریف_کرو PREFETCH_ABORT_سمتیہ         3
#تعریف_کرو انکڑے_ABORT_سمتیہ             4
#تعریف_کرو HYPERVISOR_CALL_سمتیہ        5
#تعریف_کرو رکاوٹ_سمتیہ              6
#تعریف_کرو FAST_رکاوٹ_سمتیہ         7

/* Data abort helper */
#تعریف_کرو is_align_fault(fault_حالت) \
	((fault_حالت) == FAULT_ALIGN_0)

#تعریف_کرو is_trans_fault(fault_حالت) \
	(((fault_حالت) == FAULT_TRANS_S) || ((fault_حالت) == FAULT_TRANS_P))

#تعریف_کرو is_perm_fault(fault_حالت) \
	(((fault_حالت) == FAULT_PERM_S) || ((fault_حالت) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کرو ARM_ڈھیر_چوٹی_RESERVED	(2 * حجم(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#تعریف_کرو سیٹ_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#تعریف_کرو PG_ALLOCATEME ((phys_بائٹاں)-1)

#جے_انت /* _ARM_ACONST_H */
