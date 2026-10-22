
#यदि_अपरिभाषितम् _I386_ACONST_H
#परिभाषय _I386_ACONST_H 1

#अन्तर्भाव <machine/interrupt.h>
#अन्तर्भाव <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#परिभाषय IDT_आकार 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#परिभाषय KERN_CS_सूचकांक        1
#परिभाषय KERN_DS_सूचकांक        2
#परिभाषय USER_CS_सूचकांक        3
#परिभाषय USER_DS_सूचकांक        4
#परिभाषय LDT_सूचकांक            5
#परिभाषय TSS_सूचकांक_FIRST      6
#परिभाषय TSS_सूचकांक(cpu)       (TSS_सूचकांक_FIRST + (cpu)) /* per cpu kernel tss */
#परिभाषय GDT_आकार             (TSS_सूचकांक(CONFIG_MAX_CPUS))	/* LDT descriptor */

#परिभाषय SEG_SELECTOR(i)          ((i)*8)
#परिभाषय KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_सूचकांक)
#परिभाषय KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_सूचकांक)
#परिभाषय USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_सूचकांक) | USER_PRIVILEGE)
#परिभाषय USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_सूचकांक) | USER_PRIVILEGE)
#परिभाषय LDT_SELECTOR SEG_SELECTOR(LDT_सूचकांक)
#परिभाषय TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_सूचकांक(cpu))

#परिभाषय DESC_आकार	8

/* Privileges. */
#परिभाषय INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#परिभाषय USER_PRIVILEGE       3	/* servers and user processes */
#परिभाषय RPL_आवरणम्             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#परिभाषय BOUNDS_सदिशम्        5	/* bounds check failed */
#परिभाषय INVAL_OP_सदिशम्      6	/* invalid opcode */
#परिभाषय COPROC_NOT_सदिशम्    7	/* coprocessor not available */
#परिभाषय DOUBLE_FAULT_सदिशम्  8
#परिभाषय COPROC_SEG_सदिशम्    9	/* coprocessor segment overrun */
#परिभाषय INVAL_TSS_सदिशम्    10	/* invalid TSS */
#परिभाषय SEG_NOT_सदिशम्      11	/* segment not present */
#परिभाषय राशिः_FAULT_सदिशम्  12	/* stack exception */
#परिभाषय PROTECTION_सदिशम्   13	/* general protection */

/* Selector bits. */
#परिभाषय TI                0x04	/* table indicator */
#परिभाषय RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#परिभाषय आधारः_मध्यः_सरणम्   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#परिभाषय PRESENT           0x80	/* set for descriptor present */
#परिभाषय DPL               0x60	/* descriptor privilege level mask */
#परिभाषय DPL_सरणम्            5
#परिभाषय SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#परिभाषय EXECUTABLE        0x08	/* set for executable segment */
#परिभाषय CONFORMING        0x04	/* set for conforming segment if executable */
#परिभाषय EXPAND_अधस्तात्       0x04	/* set for expand-down segment if !executable*/
#परिभाषय READABLE          0x02	/* set for readable segment if executable */
#परिभाषय WRITEABLE         0x02	/* set for writeable segment if !executable */
#परिभाषय TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#परिभाषय ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#परिभाषय AVL_286_TSS          1	/* available 286 TSS */
#परिभाषय LDT                  2	/* local descriptor table */
#परिभाषय BUSY_286_TSS         3	/* set transparently to the software */
#परिभाषय CALL_286_GATE        4	/* not used */
#परिभाषय TASK_GATE            5	/* only used by debugger */
#परिभाषय INT_286_GATE         6	/* interrupt gate, used for all vectors */
#परिभाषय TRAP_286_GATE        7	/* not used */

#परिभाषय INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#परिभाषय TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#परिभाषय PAGE_FAULT_सदिशम्   14
#परिभाषय COPROC_ERR_सदिशम्   16	/* coprocessor error */
#परिभाषय ALIGNMENT_CHECK_सदिशम्	17
#परिभाषय MACHINE_CHECK_सदिशम्	18
#परिभाषय SIMD_EXCEPTION_सदिशम्	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#परिभाषय DESC_GRANULARITY     6	/* to granularity byte */
#परिभाषय DESC_आधारः_उच्चः       7	/* to base_high */

/* Type-byte bits. */
#परिभाषय DESC_386_BIT  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#परिभाषय आधारः_उच्चः_सरणम्     24  /* shift for base --> base_high */
#परिभाषय BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#परिभाषय GRANULARITY_सरणम्   16  /* shift for limit --> granularity */
#परिभाषय विस्थापनम्_उच्चः_सरणम्   16  /* shift for (gate) offset --> offset_high */
#परिभाषय PAGE_GRAN_सरणम्     12  /* extra shift for page granular limits */

/* Granularity byte. */
#परिभाषय GRANULAR  	  0x80	/* set for 4K granularilty */
#परिभाषय DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#परिभाषय BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#परिभाषय AVL        	  0x10	/* 0 for available */
#परिभाषय सीमा_उच्चः   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#परिभाषय INIT_PSW      0x0200    /* initial psw */
#परिभाषय INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#परिभाषय TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#परिभाषय SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#परिभाषय IF_आवरणम् 0x00000200
#परिभाषय IOPL_आवरणम् 0x003000

#परिभाषय INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#परिभाषय INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#परिभाषय INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#परिभाषय AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#परिभाषय AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#परिभाषय AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#परिभाषय CPU_VENDOR_INTEL	0
#परिभाषय CPU_VENDOR_AMD		2
#परिभाषय CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#परिभाषय FPUALIGN		16

/* Poweroff 16-bit code address */
#परिभाषय BIOS_POWEROFF_प्रविष्टिः 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#परिभाषय X86_राशिः_शिखरम्_RESERVED	(2 * परिमाण(reg_t))

#परिभाषय PG_ALLOCATEME ((phys_अष्टकानि)-1)

/* MSRs */
#परिभाषय INTEL_MSR_PERFMON_CRT0         0xc1
#परिभाषय INTEL_MSR_SYSENTER_CS         0x174
#परिभाषय INTEL_MSR_SYSENTER_ESP        0x175
#परिभाषय INTEL_MSR_SYSENTER_EIP        0x176
#परिभाषय INTEL_MSR_PERFMON_SEL0        0x186

#परिभाषय INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#परिभाषय AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#परिभाषय AMD_MSR_EFER		0xC0000080	/* extended features msr */
#परिभाषय AMD_MSR_तारकम्		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#परिभाषय KTS_NONE	1 /* invalid */
#परिभाषय KTS_INT_HARD	2 /* exception / hard interrupt */
#परिभाषय KTS_INT_ORIG	3 /* soft interrupt from libc */
#परिभाषय KTS_INT_UM	4 /* soft interrupt from usermapped code */
#परिभाषय KTS_FULLCONTEXT	5 /* must restore full context */
#परिभाषय KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#परिभाषय KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#यदि_अन्तः /* _I386_ACONST_H */
