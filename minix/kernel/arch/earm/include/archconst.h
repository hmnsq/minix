
#यदि_अपरिभाषितम् _ARM_ACONST_H
#परिभाषय _ARM_ACONST_H

#अन्तर्भाव <machine/interrupt.h>
#अन्तर्भाव <machine/memory.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <arm/armreg.h>

/* Program stack words and masks. */
#परिभाषय INIT_PSR      (PSR_USR32_बहुलकम् | PSR_F)    /* initial psr */
#परिभाषय INIT_TASK_PSR (PSR_SVC32_बहुलकम् | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#परिभाषय RESET_सदिशम्                  0
#परिभाषय UNDEFINED_INST_सदिशम्         1
#परिभाषय SUPERVISOR_CALL_सदिशम्        2
#परिभाषय PREFETCH_ABORT_सदिशम्         3
#परिभाषय दत्त_ABORT_सदिशम्             4
#परिभाषय HYPERVISOR_CALL_सदिशम्        5
#परिभाषय व्यवधानम्_सदिशम्              6
#परिभाषय FAST_व्यवधानम्_सदिशम्         7

/* Data abort helper */
#परिभाषय is_align_fault(fault_स्थितिः) \
	((fault_स्थितिः) == FAULT_ALIGN_0)

#परिभाषय is_trans_fault(fault_स्थितिः) \
	(((fault_स्थितिः) == FAULT_TRANS_S) || ((fault_स्थितिः) == FAULT_TRANS_P))

#परिभाषय is_perm_fault(fault_स्थितिः) \
	(((fault_स्थितिः) == FAULT_PERM_S) || ((fault_स्थितिः) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#परिभाषय ARM_राशिः_शिखरम्_RESERVED	(2 * परिमाण(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#परिभाषय समुच्चयः_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#परिभाषय PG_ALLOCATEME ((phys_अष्टकानि)-1)

#यदि_अन्तः /* _ARM_ACONST_H */
