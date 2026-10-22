/* sections */


#include <minix/config.h>
#include <minix/const.h>
#include <machine/asm.h>
#include <machine/interrupt.h>
#include <machine/vm.h>
#include "archconst.h"
#include "kernel/const.h"
#include "sconst.h"
#include <machine/multiboot.h>


/* Easy way to make functions */

/* Make a function of the form func(arg) */

#define STACKARG 8(%ebp)

#define ARG_EAX_ACTION(FUNCTION, ACTION)        ;\
ENTRY(FUNCTION)                         ;\
        निक्षिप    %ebp                    ;\
        चालय     %esp, %ebp              ;\
        चालय     STACKARG, %eax           ;\
        ACTION                          ;\
        उद्धर     %ebp                    ;\
        प्रत्यागच्छ

/* Make a function of the form प्रत्यागच्छ = func() */
#define ARG_EAX_RETURN(FUNCTION, EXPR)        ;\
ENTRY(FUNCTION)                         ;\
        निक्षिप    %ebp                    ;\
        चालय     %esp, %ebp              ;\
        चालय	EXPR, %eax              ;\
        उद्धर     %ebp                    ;\
	प्रत्यागच्छ

/* Make a function of the form प्रत्यागच्छ = func() */
#define ARG_EAX_SET(FUNCTION, DEST)        ;\
ENTRY(FUNCTION)                         ;\
        निक्षिप    %ebp                    ;\
        चालय     %esp, %ebp              ;\
	चालय     STACKARG, %eax		;\
        चालय	%eax, DEST              ;\
	कूर्द 0f /* a jump is required for some sets */ ;\
0: 	उद्धर     %ebp                    ;\
        प्रत्यागच्छ	

/* Make a function of the form प्रत्यागच्छ = func() */
#define ARG_AX_SET(FUNCTION, DEST)        ;\
ENTRY(FUNCTION)                         ;\
        निक्षिप    %ebp                    ;\
        चालय     %esp, %ebp              ;\
	चालय     STACKARG, %eax		;\
        चालय	%ax, DEST               ;\
	कूर्द 0f /* a jump is required for some sets */ ;\
0: 	उद्धर     %ebp                    ;\
        प्रत्यागच्छ	

/*
 * This file contains a number of assembly code utility routines needed by the
 * kernel.
 */

ENTRY(__main)
	प्रत्यागच्छ


/*===========================================================================*/
/*				phys_insw				     */
/*===========================================================================*/
/*
 * सार्वजनिक void phys_insw(Port_t port, phys_bytes buf, size_t count); 
 * Input an array from an I/O port.  Absolute address version of insw(). 
 */
/* transfer data from (disk controller) port to memory */
ENTRY(phys_insw)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	cld
	निक्षिप	%edi

	चालय	8(%ebp), %edx	/* port to read from */
	चालय	12(%ebp), %edi	/* destination addr */
	चालय	16(%ebp), %ecx	/* byte count */
	shr	$1, %ecx	/* word count */
	rep insw	/* input many words */
	उद्धर	%edi
	उद्धर	%ebp
	प्रत्यागच्छ


/*===========================================================================*/
/*				phys_insb				     */
/*===========================================================================*/
/*
 * सार्वजनिक void phys_insb(Port_t port, phys_bytes buf, size_t count); 
 * Input an array from an I/O port.  Absolute address version of insb(). 
 */
/* transfer data from (disk controller) port to memory byte by byte */
ENTRY(phys_insb)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	cld
	निक्षिप	%edi

	चालय	8(%ebp), %edx	/* port to read from */
	चालय	12(%ebp), %edi	/* destination addr */
	चालय	16(%ebp), %ecx	/* byte count */
	rep insb	/* input many bytes */
	उद्धर	%edi
	उद्धर	%ebp
	प्रत्यागच्छ


/*===========================================================================*/
/*				phys_outsw				     */
/*===========================================================================*/
/*
 * सार्वजनिक void phys_outsw(Port_t port, phys_bytes buf, size_t count); 
 * Output an array to an I/O port.  Absolute address version of outsw(). 
 */
/* transfer data from memory to (disk controller) port */
ENTRY(phys_outsw)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	cld
	निक्षिप	%esi

	चालय	8(%ebp), %edx	/* port to write to */
	चालय	12(%ebp), %esi	/* source addr */
	चालय	16(%ebp), %ecx	/* byte count */
	shr	$1, %ecx	/* word count */
	rep outsw	/* output many words */
	उद्धर	%esi
	उद्धर	%ebp
	प्रत्यागच्छ


/*===========================================================================*/
/*				phys_outsb				     */
/*===========================================================================*/
/* 
 * सार्वजनिक void phys_outsb(Port_t port, phys_bytes buf, size_t count);
 * Output an array to an I/O port.  Absolute address version of outsb().
 */
/* transfer data from memory to (disk controller) port byte by byte */
ENTRY(phys_outsb)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	cld
	निक्षिप	%esi

	चालय	8(%ebp), %edx	/* port to write to */
	चालय	12(%ebp), %esi	/* source addr */
	चालय	16(%ebp), %ecx	/* byte count */
	rep outsb	/* output many bytes */
	उद्धर	%esi
	उद्धर	%ebp
	प्रत्यागच्छ


/*===========================================================================*/
/*				phys_copy				     */
/*===========================================================================*/
/*
 * सार्वजनिक phys_bytes phys_copy(phys_bytes source, phys_bytes destination,
 *			phys_bytes bytecount); 
 * Copy a block of data from anywhere to anywhere in physical memory.
 */
/*		es edi esi eip	 src dst len */
ENTRY(phys_copy)
	निक्षिप	%ebp
	चालय	%esp, %ebp

	cld
	निक्षिप	%esi
	निक्षिप	%edi

	चालय	8(%ebp), %esi
	चालय	12(%ebp), %edi
	चालय	16(%ebp), %eax

	तुलय	$10, %eax	/* avoid align overhead for small counts */
	jb	pc_small
	चालय	%esi, %ecx	/* align source, hope target is too */
	neg	%ecx
	and	$3, %ecx	/* count for alignment */
	वियोजय	%ecx, %eax

	rep 	movsb (%esi), (%edi)
	चालय	%eax, %ecx
	shr	$2, %ecx	/* count of dwords */

	rep 	movsl (%esi), (%edi)
	and	$3, %eax
pc_small:
	xchg	%eax, %ecx	/* remainder */

	rep 	movsb (%esi), (%edi)

	चालय	$0, %eax		/* 0 means: no fault */
LABEL(phys_copy_fault)		/* kernel can send us here */
	उद्धर	%edi
	उद्धर	%esi
	उद्धर	%ebp
	प्रत्यागच्छ

LABEL(phys_copy_fault_in_kernel)	/* kernel can send us here */
	उद्धर	%edi
	उद्धर	%esi
	उद्धर	%ebp
	चालय	%cr2, %eax
	प्रत्यागच्छ


/*===========================================================================*/
/*				copy_msg_from_user			     */
/*===========================================================================*/
/*
 * int copy_msg_from_user(message * user_mbuf, message * dst);
 *
 * Copies a message of 64 bytes from user process space to a kernel buffer. This
 * function assumes that the process address space is installed (cr3 loaded).
 *
 * This function from the callers point of view either succeeds or returns an
 * error which gives the caller a chance to respond accordingly. In fact it
 * either succeeds or if it generates a pagefault, general protection or other
 * exception, the trap handler has to redirect the execution to
 * __user_copy_msg_pointer_failure where the error is reported to the caller
 * without resolving the pagefault. It is not kernel's problem to deal with
 * wrong pointers from userspace and the caller should return an error to
 * userspace as if wrong values or request were passed to the kernel
 */
ENTRY(copy_msg_from_user)
	/* load the source pointer */
	चालय	4(%esp), %ecx
	/* load the destination pointer */
	चालय	8(%esp), %edx

/*	चालय	0*4(%ecx), %eax
	चालय	%eax, 0*4(%edx) */ 
	चालय	1*4(%ecx), %eax
	चालय	%eax, 1*4(%edx)
	चालय	2*4(%ecx), %eax
	चालय	%eax, 2*4(%edx)
	चालय	3*4(%ecx), %eax
	चालय	%eax, 3*4(%edx)
	चालय	4*4(%ecx), %eax
	चालय	%eax, 4*4(%edx)
	चालय	5*4(%ecx), %eax
	चालय	%eax, 5*4(%edx)
	चालय	6*4(%ecx), %eax
	चालय	%eax, 6*4(%edx)
	चालय	7*4(%ecx), %eax
	चालय	%eax, 7*4(%edx)
	चालय	8*4(%ecx), %eax
	चालय	%eax, 8*4(%edx)

	चालय	 9*4(%ecx), %eax
	चालय	%eax,  9*4(%edx) 
	चालय	10*4(%ecx), %eax
	चालय	%eax, 10*4(%edx)
	चालय	11*4(%ecx), %eax
	चालय	%eax, 11*4(%edx)
	चालय	12*4(%ecx), %eax
	चालय	%eax, 12*4(%edx)
	चालय	13*4(%ecx), %eax
	चालय	%eax, 13*4(%edx)
	चालय	14*4(%ecx), %eax
	चालय	%eax, 14*4(%edx)
	चालय	15*4(%ecx), %eax
	चालय	%eax, 15*4(%edx)

LABEL(__copy_msg_from_user_end)
	movl	$0, %eax
	प्रत्यागच्छ

/*===========================================================================*/
/*				copy_msg_to_user			     */
/*===========================================================================*/
/*
 * void copy_msg_to_user(message * src, message * user_mbuf);
 *
 * Copies a message of 64 bytes to user process space from a kernel buffer.
 *
 * All the other copy_msg_from_user() comments apply here as well!
 */
ENTRY(copy_msg_to_user)
	/* load the source pointer */
	चालय	4(%esp), %ecx
	/* load the destination pointer */
	चालय	8(%esp), %edx

	चालय	0*4(%ecx), %eax
	चालय	%eax, 0*4(%edx)
	चालय	1*4(%ecx), %eax
	चालय	%eax, 1*4(%edx)
	चालय	2*4(%ecx), %eax
	चालय	%eax, 2*4(%edx)
	चालय	3*4(%ecx), %eax
	चालय	%eax, 3*4(%edx)
	चालय	4*4(%ecx), %eax
	चालय	%eax, 4*4(%edx)
	चालय	5*4(%ecx), %eax
	चालय	%eax, 5*4(%edx)
	चालय	6*4(%ecx), %eax
	चालय	%eax, 6*4(%edx)
	चालय	7*4(%ecx), %eax
	चालय	%eax, 7*4(%edx)
	चालय	8*4(%ecx), %eax
	चालय	%eax, 8*4(%edx)


	चालय	9*4(%ecx), %eax
	चालय	%eax, 9*4(%edx)
	चालय	10*4(%ecx), %eax
	चालय	%eax, 10*4(%edx)
	चालय	11*4(%ecx), %eax
	चालय	%eax, 11*4(%edx)
	चालय	12*4(%ecx), %eax
	चालय	%eax, 12*4(%edx)
	चालय	13*4(%ecx), %eax
	चालय	%eax, 13*4(%edx)
	चालय	14*4(%ecx), %eax
	चालय	%eax, 14*4(%edx)
	चालय	15*4(%ecx), %eax
	चालय	%eax, 15*4(%edx)

LABEL(__copy_msg_to_user_end)
	movl	$0, %eax
	प्रत्यागच्छ

/*
 * if a function from a selected set of copies from or to userspace fails, it is
 * because of a wrong pointer supplied by the userspace. We have to clean up and
 * and return -1 to indicated that something wrong has happend. The place it was
 * called from has to handle this situation. The exception handler redirect us
 * here to continue, clean up and report the error
 */
ENTRY(__user_copy_msg_pointer_failure)
	movl	$-1, %eax
	प्रत्यागच्छ

/*===========================================================================*/
/*				phys_memset				     */
/*===========================================================================*/
/*
 * सार्वजनिक void phys_memset(phys_bytes dst, unsigned long pattern, 
 *	phys_bytes bytecount); 
 * Fill a block of physical memory with pattern. 
 */
ENTRY(phys_memset)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	निक्षिप	%edi
	cld

	चालय	8(%ebp), %edi
	चालय	16(%ebp), %ecx
	चालय	12(%ebp), %eax
	shr	$2, %ecx
	rep stosl

/* Any remaining bytes? */
	चालय	16(%ebp), %ecx
	and	$3, %ecx
	rep stosb

LABEL(memset_fault)		/* kernel can send us here */
	चालय	$0, %eax		/* 0 means: no fault */
	उद्धर	%edi
	उद्धर	%ebp
	प्रत्यागच्छ

LABEL(memset_fault_in_kernel)		/* kernel can send us here */
	उद्धर	%edi
	उद्धर	%ebp
	चालय	%cr2, %eax
	प्रत्यागच्छ

/*===========================================================================*/
/*				x86_triplefault				     */
/*===========================================================================*/
/*
 * सार्वजनिक void x86_triplefault(); 
 * Reset the system by loading IDT with offset 0 and interrupting. 
 */
ENTRY(x86_triplefault)
	lidt	idt_zero
	int	$3	/* anything goes, the 386 will not like it */
.data
idt_zero:
.long	0, 0
.text


/*===========================================================================*/
/*			      	halt_cpu				     */
/*===========================================================================*/
/*
 * सार्वजनिक void halt_cpu(void);
 * reanables interrupts and puts the cpu in the halts state. Once an interrupt
 * is handled the execution resumes by disabling interrupts and continues
 */
ENTRY(halt_cpu)
	sti
	hlt /* interrupts enabled only after this instruction is executed! */
	/*
	 * interrupt handlers make sure that the interrupts are disabled when we
	 * get here so we take only _one_ interrupt after halting the CPU
	 */
	प्रत्यागच्छ

/*===========================================================================*/
/*			poweroff_vmware_clihlt				     */
/*===========================================================================*/
/*
 * सार्वजनिक void poweroff_vmware_clihlt(void);
 * VMware detects this peculiar sequence and forces the virtual machine off
 * when the parameter gui.exitOnCLIHLT is set to TRUE.
 * Otherwise this sequence just hangs the CPU, requiring a power down action.
 */
ENTRY(poweroff_vmware_clihlt)
#ifndef NO_VMWARE_DETECTION
	चालय	$1, %eax
	cpuid
	test	$[1<<31], %ecx /* "virtualized" */
	jz	1f	/* always 0 on real hardware */
	चालय	$0x40000000, %eax /* select hypervisor-use leaf */
	cpuid
	तुलय	$0x61774D56, %ebx /* ASCII "VMwa" */
	jne	1f
	तुलय	$0x4D566572, %ecx /* ASCII "reVM" */
	jne	1f
	तुलय	$0x65726177, %edx /* ASCII "ware" */
	jne	1f
	/* we are virtualized by some VMware product! */
#endif
	cli
	hlt
1:	प्रत्यागच्छ

/*===========================================================================*/
/*			      read_flags				     */
/*===========================================================================*/
/*
 * सार्वजनिक unsigned long read_cpu_flags(void);
 * Read CPU status flags from C.
 */
ENTRY(read_cpu_flags)
	pushf
	चालय	(%esp), %eax
	योजय	$4, %esp
	प्रत्यागच्छ

ENTRY(read_ds)
	चालय	$0, %eax
	चालय	%ds, %ax
	प्रत्यागच्छ

ENTRY(read_cs)
	चालय	$0, %eax
	चालय	%cs, %ax
	प्रत्यागच्छ

ENTRY(read_ss)
	चालय	$0, %eax
	चालय	%ss, %ax
	प्रत्यागच्छ

/*===========================================================================*/
/*                            fpu_routines                                   */
/*===========================================================================*/

/* non-waiting FPU initialization */
ENTRY(fninit)
	fninit
	प्रत्यागच्छ

ENTRY(clts)
	clts
	प्रत्यागच्छ

/* store status word (non-waiting) */
ENTRY(fnstsw)
	xor     %eax, %eax

	/* DO न CHANGE THE OPERAND!!! gas2ack does not handle it yet */
	fnstsw	%ax
	प्रत्यागच्छ

/*===========================================================================*/
/*				fxrstor					     */
/*===========================================================================*/
ENTRY(fxrstor)
	चालय	4(%esp), %eax
	fxrstor	(%eax)
ENTRY(__fxrstor_end)
	xor     %eax, %eax
	प्रत्यागच्छ

/*===========================================================================*/
/*				frstor					     */
/*===========================================================================*/
ENTRY(frstor)
	चालय	4(%esp), %eax
	frstor	(%eax)
ENTRY(__frstor_end)
	xor     %eax, %eax
	प्रत्यागच्छ

/* Shared exception handler for both fxrstor and frstor. */
ENTRY(__frstor_failure)
	चालय	$1, %eax
	प्रत्यागच्छ

/* Read/write control registers */
ARG_EAX_RETURN(read_cr0, %cr0);
ARG_EAX_RETURN(read_cr2, %cr2);
ARG_EAX_RETURN(read_cr3, %cr3);
ARG_EAX_RETURN(read_cr4, %cr4);
ARG_EAX_SET(write_cr4, %cr4);
ARG_EAX_SET(write_cr0, %cr0);
ARG_EAX_SET(write_cr3, %cr3);

/* Read/write various descriptor tables */
ARG_EAX_ACTION(x86_ltr, ltr STACKARG );
ARG_EAX_ACTION(x86_lidt, lidtl (%eax));
ARG_EAX_ACTION(x86_lgdt, lgdt (%eax));
ARG_EAX_ACTION(x86_lldt, lldt STACKARG);
ARG_EAX_ACTION(x86_sgdt, sgdt (%eax));
ARG_EAX_ACTION(x86_sidt, sidt (%eax));

/* Load segments */
ARG_AX_SET(x86_load_ds, %ds)
ARG_AX_SET(x86_load_es, %es)
ARG_AX_SET(x86_load_fs, %fs)
ARG_AX_SET(x86_load_gs, %gs)
ARG_AX_SET(x86_load_ss, %ss)

/* FPU */
ARG_EAX_ACTION(fnsave, fnsave (%eax) ; fwait);
ARG_EAX_ACTION(fxsave, fxsave (%eax));
ARG_EAX_ACTION(fnstcw, fnstcw (%eax));

/* invlpg */
ARG_EAX_ACTION(i386_invlpg, invlpg (%eax));

ENTRY(x86_load_kerncs)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	चालय	8(%ebp), %eax
	कूर्द	$KERN_CS_SELECTOR, $newcs
newcs:
	उद्धर	%ebp
	प्रत्यागच्छ

/*
 * Read the Model Specific Register (MSR) of IA32 architecture
 *
 * void ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo)
 */
ENTRY(ia32_msr_read)
	निक्षिप	%ebp
	चालय	%esp, %ebp

	चालय	8(%ebp), %ecx
	rdmsr
	चालय	12(%ebp), %ecx
	चालय	%edx, (%ecx)
	चालय	16(%ebp), %ecx
	चालय	%eax, (%ecx)

	उद्धर	%ebp
	प्रत्यागच्छ

/*
 * Write the Model Specific Register (MSR) of IA32 architecture
 *
 * void ia32_msr_write(u32_t reg, u32_t hi, u32_t lo)
 */
ENTRY(ia32_msr_write)
	निक्षिप	%ebp
	चालय	%esp, %ebp

	चालय	12(%ebp), %edx
	चालय	16(%ebp), %eax
	चालय	8(%ebp), %ecx
	wrmsr

	उद्धर	%ebp
	प्रत्यागच्छ

/*===========================================================================*/
/*			      __switch_address_space			     */
/*===========================================================================*/
/* सार्वजनिक void __switch_address_space(struct proc *p, struct ** ptproc)
 *
 * sets the %cr3 register to the supplied value if it is not already set to the
 * same value in which case it would only result in an extra TLB flush which is
 * not desirable
 */
ENTRY(__switch_address_space)
	/* read the process pointer */
	चालय	4(%esp), %edx
	/* get the new cr3 value */
	movl	P_CR3(%edx), %eax
	/* test if the new cr3 != NULL */
	cmpl	$0, %eax
	je	0f

	/*
	 * test if the cr3 is loaded with the current value to avoid unnecessary
	 * TLB flushes
	 */
	चालय	%cr3, %ecx
	तुलय	%ecx, %eax
	je	0f
	चालय	%eax, %cr3
	/* get ptproc */
	चालय	8(%esp), %eax
	चालय	%edx, (%eax)
0:
	प्रत्यागच्छ

/* acknowledge just the master PIC */
ENTRY(eoi_8259_master)
	movb	$END_OF_INT, %al
	outb	$INT_CTL
	प्रत्यागच्छ
	
/* we have to acknowledge both PICs */
ENTRY(eoi_8259_slave)
	movb	$END_OF_INT, %al
	outb	$INT_CTL
	outb	$INT2_CTL
	प्रत्यागच्छ
	
/* in some cases we need to force TLB update, reloading cr3 does the trick */
ENTRY(refresh_tlb)
	चालय	%cr3, %eax
	चालय	%eax, %cr3
	प्रत्यागच्छ

#ifdef CONFIG_SMP

/*===========================================================================*/
/*			      smp_get_htt				     */
/*===========================================================================*/
/*  सार्वजनिक int smp_get_htt(void); */
/*  return true if the processor is hyper-threaded. */
ENTRY(smp_get_htt)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	pushf
	उद्धर	%eax
	चालय	%eax, %ebx
	and	$0x200000, %eax
	je	0f
	चालय	$0x1, %eax
/* FIXME don't use the byte code */
.byte	0x0f, 0xa2	/*  opcode for cpuid  */
	चालय	%edx, %eax
	उद्धर	%ebp
	प्रत्यागच्छ
0:
	xor	%eax, %eax
	उद्धर	%ebp
	प्रत्यागच्छ

/*===========================================================================*/
/*			      smp_get_num_htt				     */
/*===========================================================================*/
/*  सार्वजनिक int smp_get_num_htt(void); */
/*  Get the number of hyper-threaded processor cores */
ENTRY(smp_get_num_htt)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	pushf
	उद्धर	%eax
	चालय	%eax, %ebx
	and	$0x200000, %eax
	je	0f
	चालय	$0x1, %eax
/* FIXME don't use the byte code */
.byte	0x0f, 0xa2	/*  opcode for cpuid  */
	चालय	%ebx, %eax
	उद्धर	%ebp
	प्रत्यागच्छ
0:
	xor	%eax, %eax
	उद्धर	%ebp
	प्रत्यागच्छ

/*===========================================================================*/
/*			      smp_get_cores				    */
/*===========================================================================*/
/*  सार्वजनिक int smp_get_cores(void); */
/*  Get the number of cores. */
ENTRY(smp_get_cores)
	निक्षिप	%ebp
	चालय	%esp, %ebp
	pushf
	उद्धर	%eax
	चालय	%eax, %ebx
	and	$0x200000, %eax
	je	0f
	निक्षिप	%ecx
	xor	%ecx, %ecx
	चालय	$0x4, %eax
/* FIXME don't use the byte code */
.byte	0x0f, 0xa2	/*  opcode for cpuid  */
	उद्धर	%ebp
	प्रत्यागच्छ
0:
	xor	%eax, %eax
	उद्धर	%ebp
	प्रत्यागच्छ

/*===========================================================================*/
/*				arch_spinlock_lock				    */
/*===========================================================================*/
/* void arch_spinlock_lock (u32_t  *lock_data)
 * {
 * 	while (test_and_set(lock_data) == 1)
 *		while (*lock_data == 1)
 *			;
 * }
 * eax register is clobbered.
 */
ENTRY(arch_spinlock_lock)
	चालय	4(%esp), %eax
	चालय	$1, %edx
2:
	चालय	$1, %ecx
	xchg	%ecx, (%eax)
	test	%ecx, %ecx
	je	0f

	तुलय	$(1<< 16), %edx
	je	1f
	shl	%edx
1:
	चालय	%edx, %ecx
3:
	pause
	वियोजय	$1, %ecx
	test	%ecx, %ecx
	jz	2b
	कूर्द	3b
0:
	mfence
	प्रत्यागच्छ

/*===========================================================================*/
/*				arch_spinlock_unlock	                             */
/*===========================================================================*/
/* * void arch_spinlock_unlock (unsigned int *lockp) */
/*  spin lock release routine. */
ENTRY(arch_spinlock_unlock)
	चालय	4(%esp), %eax
	चालय	$0, %ecx
	xchg	%ecx, (%eax)
	mfence
	प्रत्यागच्छ

#endif /* CONFIG_SMP */

/*===========================================================================*/
/*			      mfence					     */
/*===========================================================================*/
/*  सार्वजनिक void mfence (void); */
/*  architecture specific memory barrier routine. */
ENTRY(mfence)
	mfence
	प्रत्यागच्छ

/*===========================================================================*/
/*			      arch_pause				     */
/*===========================================================================*/
/*  सार्वजनिक void arch_pause (void); */
/*  architecture specific pause routine. */
ENTRY(arch_pause)
	pause
	प्रत्यागच्छ

/*===========================================================================*/
/*			      read_ebp				     	     */
/*===========================================================================*/
/*  सार्वजनिक u16_t cpuid(void) */
ENTRY(read_ebp)
	चालय	%ebp, %eax
	प्रत्यागच्छ

ENTRY(interrupts_enable)
	sti
	प्रत्यागच्छ

ENTRY(interrupts_disable)
	cli
	प्रत्यागच्छ


/*
 * void switch_k_stack(void * esp, void (* continuation)(void));
 *
 * sets the current stack pointer to the given value and continues execution at
 * the given address
 */
ENTRY(switch_k_stack)
	/* get the arguments from the stack */
	चालय	8(%esp), %eax
	चालय	4(%esp), %ecx
	चालय	$0, %ebp	/* reset %ebp for stack trace */
	चालय	%ecx, %esp	/* set the new stack */
	कूर्द	*%eax		/* and jump to the continuation */

	/* NOT_REACHABLE */
0:	कूर्द	0b

.data
idt_ptr:
	.short 0x3ff
	.long 0x0

ldtsel:
	.long LDT_SELECTOR
