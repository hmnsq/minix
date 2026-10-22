
#నిర్వచితం_కాకపోతే _I386_ACONST_H
#నిర్వచించు _I386_ACONST_H 1

#అన్తర్భావ <machine/interrupt.h>
#అన్తర్భావ <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#నిర్వచించు IDT_కొలత 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#నిర్వచించు KERN_CS_సూచీ        1
#నిర్వచించు KERN_DS_సూచీ        2
#నిర్వచించు USER_CS_సూచీ        3
#నిర్వచించు USER_DS_సూచీ        4
#నిర్వచించు LDT_సూచీ            5
#నిర్వచించు TSS_సూచీ_మొదటి      6
#నిర్వచించు TSS_సూచీ(cpu)       (TSS_సూచీ_మొదటి + (cpu)) /* per cpu kernel tss */
#నిర్వచించు GDT_కొలత             (TSS_సూచీ(CONFIG_MAX_CPUS))	/* LDT descriptor */

#నిర్వచించు SEG_SELECTOR(i)          ((i)*8)
#నిర్వచించు KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_సూచీ)
#నిర్వచించు KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_సూచీ)
#నిర్వచించు USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_సూచీ) | USER_PRIVILEGE)
#నిర్వచించు USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_సూచీ) | USER_PRIVILEGE)
#నిర్వచించు LDT_SELECTOR SEG_SELECTOR(LDT_సూచీ)
#నిర్వచించు TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_సూచీ(cpu))

#నిర్వచించు DESC_కొలత	8

/* Privileges. */
#నిర్వచించు INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#నిర్వచించు USER_PRIVILEGE       3	/* servers and user processes */
#నిర్వచించు RPL_ముసుగు             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#నిర్వచించు BOUNDS_సదిశ        5	/* bounds check failed */
#నిర్వచించు INVAL_OP_సదిశ      6	/* invalid opcode */
#నిర్వచించు COPROC_NOT_సదిశ    7	/* coprocessor not available */
#నిర్వచించు DOUBLE_FAULT_సదిశ  8
#నిర్వచించు COPROC_SEG_సదిశ    9	/* coprocessor segment overrun */
#నిర్వచించు INVAL_TSS_సదిశ    10	/* invalid TSS */
#నిర్వచించు SEG_NOT_సదిశ      11	/* segment not present */
#నిర్వచించు దొంతర_FAULT_సదిశ  12	/* stack exception */
#నిర్వచించు PROTECTION_సదిశ   13	/* general protection */

/* Selector bits. */
#నిర్వచించు TI                0x04	/* table indicator */
#నిర్వచించు RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#నిర్వచించు పునాది_మధ్య_జరుపు   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#నిర్వచించు PRESENT           0x80	/* set for descriptor present */
#నిర్వచించు DPL               0x60	/* descriptor privilege level mask */
#నిర్వచించు DPL_జరుపు            5
#నిర్వచించు SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#నిర్వచించు EXECUTABLE        0x08	/* set for executable segment */
#నిర్వచించు CONFORMING        0x04	/* set for conforming segment if executable */
#నిర్వచించు EXPAND_కిందికి       0x04	/* set for expand-down segment if !executable*/
#నిర్వచించు READABLE          0x02	/* set for readable segment if executable */
#నిర్వచించు WRITEABLE         0x02	/* set for writeable segment if !executable */
#నిర్వచించు TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#నిర్వచించు ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#నిర్వచించు AVL_286_TSS          1	/* available 286 TSS */
#నిర్వచించు LDT                  2	/* local descriptor table */
#నిర్వచించు BUSY_286_TSS         3	/* set transparently to the software */
#నిర్వచించు CALL_286_GATE        4	/* not used */
#నిర్వచించు TASK_GATE            5	/* only used by debugger */
#నిర్వచించు INT_286_GATE         6	/* interrupt gate, used for all vectors */
#నిర్వచించు TRAP_286_GATE        7	/* not used */

#నిర్వచించు INT_GATE_TYPE	(INT_286_GATE | DESC_386_బిట్టు)
#నిర్వచించు TSS_TYPE	(AVL_286_TSS  | DESC_386_బిట్టు)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#నిర్వచించు పుట_FAULT_సదిశ   14
#నిర్వచించు COPROC_ERR_సదిశ   16	/* coprocessor error */
#నిర్వచించు ALIGNMENT_CHECK_సదిశ	17
#నిర్వచించు MACHINE_CHECK_సదిశ	18
#నిర్వచించు SIMD_EXCEPTION_సదిశ	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#నిర్వచించు DESC_GRANULARITY     6	/* to granularity byte */
#నిర్వచించు DESC_పునాది_ఎగువ       7	/* to base_high */

/* Type-byte bits. */
#నిర్వచించు DESC_386_బిట్టు  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#నిర్వచించు పునాది_ఎగువ_జరుపు     24  /* shift for base --> base_high */
#నిర్వచించు BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#నిర్వచించు GRANULARITY_జరుపు   16  /* shift for limit --> granularity */
#నిర్వచించు స్థానభ్రంశం_ఎగువ_జరుపు   16  /* shift for (gate) offset --> offset_high */
#నిర్వచించు పుట_GRAN_జరుపు     12  /* extra shift for page granular limits */

/* Granularity byte. */
#నిర్వచించు GRANULAR  	  0x80	/* set for 4K granularilty */
#నిర్వచించు DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#నిర్వచించు BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#నిర్వచించు AVL        	  0x10	/* 0 for available */
#నిర్వచించు హద్దు_ఎగువ   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#నిర్వచించు INIT_PSW      0x0200    /* initial psw */
#నిర్వచించు INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#నిర్వచించు TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#నిర్వచించు SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#నిర్వచించు IF_ముసుగు 0x00000200
#నిర్వచించు IOPL_ముసుగు 0x003000

#నిర్వచించు INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#నిర్వచించు INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#నిర్వచించు INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#నిర్వచించు AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#నిర్వచించు AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#నిర్వచించు AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#నిర్వచించు CPU_VENDOR_INTEL	0
#నిర్వచించు CPU_VENDOR_AMD		2
#నిర్వచించు CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#నిర్వచించు FPUALIGN		16

/* Poweroff 16-bit code address */
#నిర్వచించు BIOS_POWEROFF_నమోదు 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#నిర్వచించు X86_దొంతర_శిఖరం_RESERVED	(2 * పరిమాణం(reg_t))

#నిర్వచించు PG_ALLOCATEME ((phys_బైట్లు)-1)

/* MSRs */
#నిర్వచించు INTEL_MSR_PERFMON_CRT0         0xc1
#నిర్వచించు INTEL_MSR_SYSENTER_CS         0x174
#నిర్వచించు INTEL_MSR_SYSENTER_ESP        0x175
#నిర్వచించు INTEL_MSR_SYSENTER_EIP        0x176
#నిర్వచించు INTEL_MSR_PERFMON_SEL0        0x186

#నిర్వచించు INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#నిర్వచించు AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#నిర్వచించు AMD_MSR_EFER		0xC0000080	/* extended features msr */
#నిర్వచించు AMD_MSR_నక్షత్రం		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#నిర్వచించు KTS_NONE	1 /* invalid */
#నిర్వచించు KTS_INT_HARD	2 /* exception / hard interrupt */
#నిర్వచించు KTS_INT_ORIG	3 /* soft interrupt from libc */
#నిర్వచించు KTS_INT_UM	4 /* soft interrupt from usermapped code */
#నిర్వచించు KTS_FULLCONTEXT	5 /* must restore full context */
#నిర్వచించు KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#నిర్వచించు KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#అయితే_ముగింపు /* _I386_ACONST_H */
