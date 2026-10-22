
#अगर_अघोषित _ARM_ACONST_H
#घोषणा _ARM_ACONST_H

#समावेश <machine/interrupt.h>
#समावेश <machine/memory.h>
#समावेश <machine/cpu.h>
#समावेश <arm/armreg.h>

/* Program stack words and masks. */
#घोषणा INIT_PSR      (PSR_USR32_MODE | PSR_F)    /* initial psr */
#घोषणा INIT_TASK_PSR (PSR_SVC32_MODE | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#घोषणा RESET_VECTOR                  0
#घोषणा UNDEFINED_INST_VECTOR         1
#घोषणा SUPERVISOR_CALL_VECTOR        2
#घोषणा PREFETCH_ABORT_VECTOR         3
#घोषणा तथ्य_ABORT_VECTOR             4
#घोषणा HYPERVISOR_CALL_VECTOR        5
#घोषणा INTERRUPT_VECTOR              6
#घोषणा FAST_INTERRUPT_VECTOR         7

/* Data abort helper */
#घोषणा is_align_fault(fault_status) \
	((fault_status) == FAULT_ALIGN_0)

#घोषणा is_trans_fault(fault_status) \
	(((fault_status) == FAULT_TRANS_S) || ((fault_status) == FAULT_TRANS_P))

#घोषणा is_perm_fault(fault_status) \
	(((fault_status) == FAULT_PERM_S) || ((fault_status) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#घोषणा ARM_ढेर_शीर्ष_RESERVED	(2 * माप(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#घोषणा SET_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#घोषणा PG_ALLOCATEME ((phys_bytes)-1)

#पूर्ण_अगर /* _ARM_ACONST_H */
