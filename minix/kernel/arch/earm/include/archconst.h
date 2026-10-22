
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _ARM_ACONST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ARM_ACONST_H

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/memory.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arm/armreg.h>

/* Program stack words and masks. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INIT_PSR      (PSR_USR32_ਬਹੁਲਕ | PSR_F)    /* initial psr */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INIT_TASK_PSR (PSR_SVC32_ਬਹੁਲਕ | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESET_ਸਦਿਸ਼                  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDEFINED_INST_ਸਦਿਸ਼         1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUPERVISOR_CALL_ਸਦਿਸ਼        2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PREFETCH_ABORT_ਸਦਿਸ਼         3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_ABORT_ਸਦਿਸ਼             4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HYPERVISOR_CALL_ਸਦਿਸ਼        5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰੁਕਾਵਟ_ਸਦਿਸ਼              6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAST_ਰੁਕਾਵਟ_ਸਦਿਸ਼         7

/* Data abort helper */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_align_fault(fault_ਸਥਿਤੀ) \
	((fault_ਸਥਿਤੀ) == FAULT_ALIGN_0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_trans_fault(fault_ਸਥਿਤੀ) \
	(((fault_ਸਥਿਤੀ) == FAULT_TRANS_S) || ((fault_ਸਥਿਤੀ) == FAULT_TRANS_P))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_perm_fault(fault_ਸਥਿਤੀ) \
	(((fault_ਸਥਿਤੀ) == FAULT_PERM_S) || ((fault_ਸਥਿਤੀ) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARM_ਢੇਰ_ਸਿਖਰ_RESERVED	(2 * ਆਕਾਰ(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PG_ALLOCATEME ((phys_ਬਾਈਟਾਂ)-1)

#ਜੇ_ਅੰਤ /* _ARM_ACONST_H */
