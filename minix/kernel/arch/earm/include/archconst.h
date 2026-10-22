
#నిర్వచితం_కాకపోతే _ARM_ACONST_H
#నిర్వచించు _ARM_ACONST_H

#అన్తర్భావ <machine/interrupt.h>
#అన్తర్భావ <machine/memory.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <arm/armreg.h>

/* Program stack words and masks. */
#నిర్వచించు INIT_PSR      (PSR_USR32_బహుళకం | PSR_F)    /* initial psr */
#నిర్వచించు INIT_TASK_PSR (PSR_SVC32_బహుళకం | PSR_F)    /* initial psr for tasks */

/* Exception vector numbers */
#నిర్వచించు RESET_సదిశ                  0
#నిర్వచించు UNDEFINED_INST_సదిశ         1
#నిర్వచించు SUPERVISOR_CALL_సదిశ        2
#నిర్వచించు PREFETCH_ABORT_సదిశ         3
#నిర్వచించు దత్తాంశం_ABORT_సదిశ             4
#నిర్వచించు HYPERVISOR_CALL_సదిశ        5
#నిర్వచించు అంతరాయం_సదిశ              6
#నిర్వచించు FAST_అంతరాయం_సదిశ         7

/* Data abort helper */
#నిర్వచించు is_align_fault(fault_స్థితి) \
	((fault_స్థితి) == FAULT_ALIGN_0)

#నిర్వచించు is_trans_fault(fault_స్థితి) \
	(((fault_స్థితి) == FAULT_TRANS_S) || ((fault_స్థితి) == FAULT_TRANS_P))

#నిర్వచించు is_perm_fault(fault_స్థితి) \
	(((fault_స్థితి) == FAULT_PERM_S) || ((fault_స్థితి) == FAULT_PERM_P))

/*
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#నిర్వచించు ARM_దొంతర_శిఖరం_RESERVED	(2 * పరిమాణం(reg_t))

/* only selected bits are changeable by user e.g.[31:9] and skip the 
 * mode bits. It is probably is a better idea to look at the current 
 * status to determine if one is allowed to write these values. This 
 * might allow debugging of privileged processes 
 */
#నిర్వచించు సమితి_USR_PSR(rp, npsr) \
	rp->p_reg.psr = ( rp->p_reg.psr & 0x1F) | ( npsr & ~0x1F)


#నిర్వచించు PG_ALLOCATEME ((phys_బైట్లు)-1)

#అయితే_ముగింపు /* _ARM_ACONST_H */
