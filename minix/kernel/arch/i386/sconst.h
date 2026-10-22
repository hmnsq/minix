#నిర్వచితం_కాకపోతే __SCONST_H__
#నిర్వచించు __SCONST_H__

#అన్తర్భావ "kernel/const.h"
#అన్తర్భావ "kernel/procoffsets.h"

/*
 * offset to current process pointer right after trap, we assume we always have
 * error code on the stack
 */
#నిర్వచించు CURR_PROC_PTR		20

/*
 * tests whether the interrupt was triggered in kernel. If so, jump to the
 * label. Displacement tell the macro ha far is the CS value saved by the trap
 * from the current %esp. The kernel code segment selector has the lower 3 bits
 * zeroed
 */
#నిర్వచించు TEST_INT_లోపల_KERNEL(displ, label)	\
	cmpl	$KERN_CS_SELECTOR, displ(%esp)	;\
	je	label				;

/*
 * saves the basic interrupt context (no error code) to the process structure
 *
 * displ is the displacement of %esp from the original stack after trap
 * pptr is the process structure pointer
 * tmp is an available temporary register
 */
#నిర్వచించు SAVE_TRAP_CTX(displ, pptr, tmp)			\
	movl	(0 + displ)(%esp), tmp			;\
	movl	tmp, PCREG(pptr)			;\
	movl	(4 + displ)(%esp), tmp			;\
	movl	tmp, CSREG(pptr)			;\
	movl	(8 + displ)(%esp), tmp			;\
	movl	tmp, PSWREG(pptr)			;\
	movl	(12 + displ)(%esp), tmp			;\
	movl	tmp, SPREG(pptr)

/*
 * restore kernel segments. %cs is already set and %fs, %gs are not used */
#నిర్వచించు RESTORE_KERNEL_SEGS	\
	mov	$KERN_DS_SELECTOR, %si	;\
	mov	%si, %ds	;\
	mov	%si, %es	;\
	movw	$0, %si		;\
	mov	%si, %gs	;\
	mov	%si, %fs	;

#నిర్వచించు SAVE_GP_REGS(pptr)	\
	mov	%eax, AXREG(pptr)		;\
	mov	%ecx, CXREG(pptr)		;\
	mov	%edx, DXREG(pptr)		;\
	mov	%ebx, BXREG(pptr)		;\
	mov	%esi, SIREG(pptr)		;\
	mov	%edi, DIREG(pptr)		;

#నిర్వచించు RESTORE_GP_REGS(pptr)	\
	movl	AXREG(pptr), %eax		;\
	movl	CXREG(pptr), %ecx		;\
	movl	DXREG(pptr), %edx		;\
	movl	BXREG(pptr), %ebx		;\
	movl	SIREG(pptr), %esi		;\
	movl	DIREG(pptr), %edi		;

/*
 * save the context of the interrupted process to the structure in the process
 * table. It pushses the %ebp to stack to get a scratch register. After %esi is
 * saved, we can use it to get the saved %ebp from stack and save it to the
 * final location
 *
 * displ is the stack displacement. In case of an exception, there are two extra
 * value on the stack - error code and the exception number
 */
#నిర్వచించు SAVE_PROCESS_CTX(displ, trapcode) \
								\
	cld /* set the direction flag to a known state */	;\
								\
	నెట్టు	%ebp					;\
							;\
	movl	(CURR_PROC_PTR + 4 + displ)(%esp), %ebp	;\
							\
	SAVE_GP_REGS(%ebp)				;\
        movl	$trapcode, P_KERN_TRAP_STYLE(%ebp)	;\
	తీయి	%esi			/* get the orig %ebp and save it */ ;\
	mov	%esi, BPREG(%ebp)			;\
							\
	RESTORE_KERNEL_SEGS				;\
	SAVE_TRAP_CTX(displ, %ebp, %esi)		;

/*
 * clear the IF flag in eflags which are stored somewhere in memory, e.g. on
 * stack. iret or popf will load the new value later
 */
#నిర్వచించు CLEAR_IF(where)	\
	mov	where, %eax						;\
	andl	$0xfffffdff, %eax					;\
	mov	%eax, where						;

#అయితే_ముగింపు /* __SCONST_H__ */
