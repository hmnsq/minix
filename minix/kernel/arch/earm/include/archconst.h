
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _ARM_ACONST_H
#ವ್ಯಾಖ್ಯಾನಿಸು _ARM_ACONST_H

#ಅನ್ತರ್ಭಾವ <machine/interrupt.h>
#ಅನ್ತರ್ಭಾವ <machine/memory.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <arm/armreg.h>

/* Program stack words and masks. */
#ವ್ಯಾಖ್ಯಾನಿಸು INIT_PSR      (PSR_USR32_ಬಹುಲಕ | PSR_F)    /* initial psr */
#ವ್ಯಾಖ್ಯಾನಿಸು INIT_TASK_PSR (PSR_SVC32_ಬಹುಲಕ | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#ವ್ಯಾಖ್ಯಾನಿಸು RESET_ಸದಿಶ                  0
#ವ್ಯಾಖ್ಯಾನಿಸು UNDEFINED_INST_ಸದಿಶ         1
#ವ್ಯಾಖ್ಯಾನಿಸು SUPERVISOR_CALL_ಸದಿಶ        2
#ವ್ಯಾಖ್ಯಾನಿಸು PREFETCH_ABORT_ಸದಿಶ         3
#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_ABORT_ಸದಿಶ             4
#ವ್ಯಾಖ್ಯಾನಿಸು HYPERVISOR_CALL_ಸದಿಶ        5
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಡಚಣೆ_ಸದಿಶ              6
#ವ್ಯಾಖ್ಯಾನಿಸು FAST_ಅಡಚಣೆ_ಸದಿಶ         7

/* Data abort helper */
#ವ್ಯಾಖ್ಯಾನಿಸು is_align_fault(fault_ಸ್ಥಿತಿ) \
	((fault_ಸ್ಥಿತಿ) == FAULT_ALIGN_0)

#ವ್ಯಾಖ್ಯಾನಿಸು is_trans_fault(fault_ಸ್ಥಿತಿ) \
	(((fault_ಸ್ಥಿತಿ) == FAULT_TRANS_S) || ((fault_ಸ್ಥಿತಿ) == FAULT_TRANS_P))

#ವ್ಯಾಖ್ಯಾನಿಸು is_perm_fault(fault_ಸ್ಥಿತಿ) \
	(((fault_ಸ್ಥಿತಿ) == FAULT_PERM_S) || ((fault_ಸ್ಥಿತಿ) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ARM_ರಾಶಿ_ತುದಿ_RESERVED	(2 * ಗಾತ್ರ(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#ವ್ಯಾಖ್ಯಾನಿಸು PG_ALLOCATEME ((phys_ಬೈಟುಗಳು)-1)

#ಆದರೆ_ಅಂತ್ಯ /* _ARM_ACONST_H */
