
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _I386_ACONST_H
#ವ್ಯಾಖ್ಯಾನಿಸು _I386_ACONST_H 1

#ಅನ್ತರ್ಭಾವ <machine/interrupt.h>
#ಅನ್ತರ್ಭಾವ <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#ವ್ಯಾಖ್ಯಾನಿಸು IDT_ಗಾತ್ರ 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#ವ್ಯಾಖ್ಯಾನಿಸು KERN_CS_ಸೂಚ್ಯಂಕ        1
#ವ್ಯಾಖ್ಯಾನಿಸು KERN_DS_ಸೂಚ್ಯಂಕ        2
#ವ್ಯಾಖ್ಯಾನಿಸು USER_CS_ಸೂಚ್ಯಂಕ        3
#ವ್ಯಾಖ್ಯಾನಿಸು USER_DS_ಸೂಚ್ಯಂಕ        4
#ವ್ಯಾಖ್ಯಾನಿಸು LDT_ಸೂಚ್ಯಂಕ            5
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_ಸೂಚ್ಯಂಕ_ಮೊದಲ      6
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_ಸೂಚ್ಯಂಕ(cpu)       (TSS_ಸೂಚ್ಯಂಕ_ಮೊದಲ + (cpu)) /* per cpu kernel tss */
#ವ್ಯಾಖ್ಯಾನಿಸು GDT_ಗಾತ್ರ             (TSS_ಸೂಚ್ಯಂಕ(CONFIG_MAX_CPUS))	/* LDT descriptor */

#ವ್ಯಾಖ್ಯಾನಿಸು SEG_SELECTOR(i)          ((i)*8)
#ವ್ಯಾಖ್ಯಾನಿಸು KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_ಸೂಚ್ಯಂಕ)
#ವ್ಯಾಖ್ಯಾನಿಸು KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_ಸೂಚ್ಯಂಕ)
#ವ್ಯಾಖ್ಯಾನಿಸು USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_ಸೂಚ್ಯಂಕ) | USER_PRIVILEGE)
#ವ್ಯಾಖ್ಯಾನಿಸು USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_ಸೂಚ್ಯಂಕ) | USER_PRIVILEGE)
#ವ್ಯಾಖ್ಯಾನಿಸು LDT_SELECTOR SEG_SELECTOR(LDT_ಸೂಚ್ಯಂಕ)
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_ಸೂಚ್ಯಂಕ(cpu))

#ವ್ಯಾಖ್ಯಾನಿಸು DESC_ಗಾತ್ರ	8

/* Privileges. */
#ವ್ಯಾಖ್ಯಾನಿಸು INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#ವ್ಯಾಖ್ಯಾನಿಸು USER_PRIVILEGE       3	/* servers and user processes */
#ವ್ಯಾಖ್ಯಾನಿಸು RPL_ಮುಖವಾಡ             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOUNDS_ಸದಿಶ        5	/* bounds check failed */
#ವ್ಯಾಖ್ಯಾನಿಸು INVAL_OP_ಸದಿಶ      6	/* invalid opcode */
#ವ್ಯಾಖ್ಯಾನಿಸು COPROC_NOT_ಸದಿಶ    7	/* coprocessor not available */
#ವ್ಯಾಖ್ಯಾನಿಸು DOUBLE_FAULT_ಸದಿಶ  8
#ವ್ಯಾಖ್ಯಾನಿಸು COPROC_SEG_ಸದಿಶ    9	/* coprocessor segment overrun */
#ವ್ಯಾಖ್ಯಾನಿಸು INVAL_TSS_ಸದಿಶ    10	/* invalid TSS */
#ವ್ಯಾಖ್ಯಾನಿಸು SEG_NOT_ಸದಿಶ      11	/* segment not present */
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_FAULT_ಸದಿಶ  12	/* stack exception */
#ವ್ಯಾಖ್ಯಾನಿಸು PROTECTION_ಸದಿಶ   13	/* general protection */

/* Selector bits. */
#ವ್ಯಾಖ್ಯಾನಿಸು TI                0x04	/* table indicator */
#ವ್ಯಾಖ್ಯಾನಿಸು RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ_ಮಧ್ಯ_ಸರಿಸು   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#ವ್ಯಾಖ್ಯಾನಿಸು PRESENT           0x80	/* set for descriptor present */
#ವ್ಯಾಖ್ಯಾನಿಸು DPL               0x60	/* descriptor privilege level mask */
#ವ್ಯಾಖ್ಯಾನಿಸು DPL_ಸರಿಸು            5
#ವ್ಯಾಖ್ಯಾನಿಸು SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#ವ್ಯಾಖ್ಯಾನಿಸು EXECUTABLE        0x08	/* set for executable segment */
#ವ್ಯಾಖ್ಯಾನಿಸು CONFORMING        0x04	/* set for conforming segment if executable */
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_ಕೆಳಕ್ಕೆ       0x04	/* set for expand-down segment if !executable*/
#ವ್ಯಾಖ್ಯಾನಿಸು READABLE          0x02	/* set for readable segment if executable */
#ವ್ಯಾಖ್ಯಾನಿಸು WRITEABLE         0x02	/* set for writeable segment if !executable */
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#ವ್ಯಾಖ್ಯಾನಿಸು ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#ವ್ಯಾಖ್ಯಾನಿಸು AVL_286_TSS          1	/* available 286 TSS */
#ವ್ಯಾಖ್ಯಾನಿಸು LDT                  2	/* local descriptor table */
#ವ್ಯಾಖ್ಯಾನಿಸು BUSY_286_TSS         3	/* set transparently to the software */
#ವ್ಯಾಖ್ಯಾನಿಸು CALL_286_GATE        4	/* not used */
#ವ್ಯಾಖ್ಯಾನಿಸು TASK_GATE            5	/* only used by debugger */
#ವ್ಯಾಖ್ಯಾನಿಸು INT_286_GATE         6	/* interrupt gate, used for all vectors */
#ವ್ಯಾಖ್ಯಾನಿಸು TRAP_286_GATE        7	/* not used */

#ವ್ಯಾಖ್ಯಾನಿಸು INT_GATE_TYPE	(INT_286_GATE | DESC_386_ಬಿಟ್)
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_TYPE	(AVL_286_TSS  | DESC_386_ಬಿಟ್)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_FAULT_ಸದಿಶ   14
#ವ್ಯಾಖ್ಯಾನಿಸು COPROC_ERR_ಸದಿಶ   16	/* coprocessor error */
#ವ್ಯಾಖ್ಯಾನಿಸು ALIGNMENT_CHECK_ಸದಿಶ	17
#ವ್ಯಾಖ್ಯಾನಿಸು MACHINE_CHECK_ಸದಿಶ	18
#ವ್ಯಾಖ್ಯಾನಿಸು SIMD_EXCEPTION_ಸದಿಶ	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#ವ್ಯಾಖ್ಯಾನಿಸು DESC_GRANULARITY     6	/* to granularity byte */
#ವ್ಯಾಖ್ಯಾನಿಸು DESC_ತಳ_ಮೇಲಿನ       7	/* to base_high */

/* Type-byte bits. */
#ವ್ಯಾಖ್ಯಾನಿಸು DESC_386_ಬಿಟ್  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ_ಮೇಲಿನ_ಸರಿಸು     24  /* shift for base --> base_high */
#ವ್ಯಾಖ್ಯಾನಿಸು BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#ವ್ಯಾಖ್ಯಾನಿಸು GRANULARITY_ಸರಿಸು   16  /* shift for limit --> granularity */
#ವ್ಯಾಖ್ಯಾನಿಸು ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ_ಸರಿಸು   16  /* shift for (gate) offset --> offset_high */
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_GRAN_ಸರಿಸು     12  /* extra shift for page granular limits */

/* Granularity byte. */
#ವ್ಯಾಖ್ಯಾನಿಸು GRANULAR  	  0x80	/* set for 4K granularilty */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#ವ್ಯಾಖ್ಯಾನಿಸು BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#ವ್ಯಾಖ್ಯಾನಿಸು AVL        	  0x10	/* 0 for available */
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ_ಮೇಲಿನ   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#ವ್ಯಾಖ್ಯಾನಿಸು INIT_PSW      0x0200    /* initial psw */
#ವ್ಯಾಖ್ಯಾನಿಸು INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#ವ್ಯಾಖ್ಯಾನಿಸು TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#ವ್ಯಾಖ್ಯಾನಿಸು SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#ವ್ಯಾಖ್ಯಾನಿಸು IF_ಮುಖವಾಡ 0x00000200
#ವ್ಯಾಖ್ಯಾನಿಸು IOPL_ಮುಖವಾಡ 0x003000

#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#ವ್ಯಾಖ್ಯಾನಿಸು AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#ವ್ಯಾಖ್ಯಾನಿಸು CPU_VENDOR_INTEL	0
#ವ್ಯಾಖ್ಯಾನಿಸು CPU_VENDOR_AMD		2
#ವ್ಯಾಖ್ಯಾನಿಸು CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#ವ್ಯಾಖ್ಯಾನಿಸು FPUALIGN		16

/* Poweroff 16-bit code address */
#ವ್ಯಾಖ್ಯಾನಿಸು BIOS_POWEROFF_ನಮೂದು 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#ವ್ಯಾಖ್ಯಾನಿಸು X86_ರಾಶಿ_ತುದಿ_RESERVED	(2 * ಗಾತ್ರ(reg_t))

#ವ್ಯಾಖ್ಯಾನಿಸು PG_ALLOCATEME ((phys_ಬೈಟುಗಳು)-1)

/* MSRs */
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_PERFMON_CRT0         0xc1
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_SYSENTER_CS         0x174
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_SYSENTER_ESP        0x175
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_SYSENTER_EIP        0x176
#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_PERFMON_SEL0        0x186

#ವ್ಯಾಖ್ಯಾನಿಸು INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#ವ್ಯಾಖ್ಯಾನಿಸು AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_MSR_EFER		0xC0000080	/* extended features msr */
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_MSR_ನಕ್ಷತ್ರ		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_NONE	1 /* invalid */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_INT_HARD	2 /* exception / hard interrupt */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_INT_ORIG	3 /* soft interrupt from libc */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_INT_UM	4 /* soft interrupt from usermapped code */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_FULLCONTEXT	5 /* must restore full context */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#ವ್ಯಾಖ್ಯಾನಿಸು KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#ಆದರೆ_ಅಂತ್ಯ /* _I386_ACONST_H */
