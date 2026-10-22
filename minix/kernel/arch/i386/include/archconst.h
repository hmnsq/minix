
#که_نه_تعریف_شوی _I386_ACONST_H
#تعریف_کړه _I386_ACONST_H 1

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#تعریف_کړه IDT_اندازه 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#تعریف_کړه KERN_CS_شاخص        1
#تعریف_کړه KERN_DS_شاخص        2
#تعریف_کړه USER_CS_شاخص        3
#تعریف_کړه USER_DS_شاخص        4
#تعریف_کړه LDT_شاخص            5
#تعریف_کړه TSS_شاخص_لومړی      6
#تعریف_کړه TSS_شاخص(cpu)       (TSS_شاخص_لومړی + (cpu)) /* per cpu kernel tss */
#تعریف_کړه GDT_اندازه             (TSS_شاخص(CONFIG_MAX_CPUS))	/* LDT descriptor */

#تعریف_کړه SEG_SELECTOR(i)          ((i)*8)
#تعریف_کړه KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_شاخص)
#تعریف_کړه KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_شاخص)
#تعریف_کړه USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_شاخص) | USER_PRIVILEGE)
#تعریف_کړه USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_شاخص) | USER_PRIVILEGE)
#تعریف_کړه LDT_SELECTOR SEG_SELECTOR(LDT_شاخص)
#تعریف_کړه TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_شاخص(cpu))

#تعریف_کړه DESC_اندازه	8

/* Privileges. */
#تعریف_کړه INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#تعریف_کړه USER_PRIVILEGE       3	/* servers and user processes */
#تعریف_کړه RPL_ماسک             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#تعریف_کړه BOUNDS_ویکټور        5	/* bounds check failed */
#تعریف_کړه INVAL_OP_ویکټور      6	/* invalid opcode */
#تعریف_کړه COPROC_NOT_ویکټور    7	/* coprocessor not available */
#تعریف_کړه DOUBLE_FAULT_ویکټور  8
#تعریف_کړه COPROC_SEG_ویکټور    9	/* coprocessor segment overrun */
#تعریف_کړه INVAL_TSS_ویکټور    10	/* invalid TSS */
#تعریف_کړه SEG_NOT_ویکټور      11	/* segment not present */
#تعریف_کړه ډېری_FAULT_ویکټور  12	/* stack exception */
#تعریف_کړه PROTECTION_ویکټور   13	/* general protection */

/* Selector bits. */
#تعریف_کړه TI                0x04	/* table indicator */
#تعریف_کړه RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#تعریف_کړه بنسټ_منځ_ښويدنه   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#تعریف_کړه PRESENT           0x80	/* set for descriptor present */
#تعریف_کړه DPL               0x60	/* descriptor privilege level mask */
#تعریف_کړه DPL_ښويدنه            5
#تعریف_کړه SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#تعریف_کړه EXECUTABLE        0x08	/* set for executable segment */
#تعریف_کړه CONFORMING        0x04	/* set for conforming segment if executable */
#تعریف_کړه EXPAND_ښکته       0x04	/* set for expand-down segment if !executable*/
#تعریف_کړه READABLE          0x02	/* set for readable segment if executable */
#تعریف_کړه WRITEABLE         0x02	/* set for writeable segment if !executable */
#تعریف_کړه TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#تعریف_کړه ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#تعریف_کړه AVL_286_TSS          1	/* available 286 TSS */
#تعریف_کړه LDT                  2	/* local descriptor table */
#تعریف_کړه BUSY_286_TSS         3	/* set transparently to the software */
#تعریف_کړه CALL_286_GATE        4	/* not used */
#تعریف_کړه TASK_GATE            5	/* only used by debugger */
#تعریف_کړه INT_286_GATE         6	/* interrupt gate, used for all vectors */
#تعریف_کړه TRAP_286_GATE        7	/* not used */

#تعریف_کړه INT_GATE_TYPE	(INT_286_GATE | DESC_386_بټ)
#تعریف_کړه TSS_TYPE	(AVL_286_TSS  | DESC_386_بټ)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#تعریف_کړه پاڼه_FAULT_ویکټور   14
#تعریف_کړه COPROC_ERR_ویکټور   16	/* coprocessor error */
#تعریف_کړه ALIGNMENT_CHECK_ویکټور	17
#تعریف_کړه MACHINE_CHECK_ویکټور	18
#تعریف_کړه SIMD_EXCEPTION_ویکټور	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#تعریف_کړه DESC_GRANULARITY     6	/* to granularity byte */
#تعریف_کړه DESC_بنسټ_لوړ       7	/* to base_high */

/* Type-byte bits. */
#تعریف_کړه DESC_386_بټ  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#تعریف_کړه بنسټ_لوړ_ښويدنه     24  /* shift for base --> base_high */
#تعریف_کړه BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#تعریف_کړه GRANULARITY_ښويدنه   16  /* shift for limit --> granularity */
#تعریف_کړه دننه_واټن_لوړ_ښويدنه   16  /* shift for (gate) offset --> offset_high */
#تعریف_کړه پاڼه_GRAN_ښويدنه     12  /* extra shift for page granular limits */

/* Granularity byte. */
#تعریف_کړه GRANULAR  	  0x80	/* set for 4K granularilty */
#تعریف_کړه DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#تعریف_کړه BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#تعریف_کړه AVL        	  0x10	/* 0 for available */
#تعریف_کړه حد_لوړ   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#تعریف_کړه INIT_PSW      0x0200    /* initial psw */
#تعریف_کړه INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#تعریف_کړه TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#تعریف_کړه SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#تعریف_کړه IF_ماسک 0x00000200
#تعریف_کړه IOPL_ماسک 0x003000

#تعریف_کړه INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#تعریف_کړه INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#تعریف_کړه INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#تعریف_کړه AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#تعریف_کړه AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#تعریف_کړه AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#تعریف_کړه CPU_VENDOR_INTEL	0
#تعریف_کړه CPU_VENDOR_AMD		2
#تعریف_کړه CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#تعریف_کړه FPUALIGN		16

/* Poweroff 16-bit code address */
#تعریف_کړه BIOS_POWEROFF_ثبت 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کړه X86_ډېری_سر_RESERVED	(2 * حجم(reg_t))

#تعریف_کړه PG_ALLOCATEME ((phys_بایټونه)-1)

/* MSRs */
#تعریف_کړه INTEL_MSR_PERFMON_CRT0         0xc1
#تعریف_کړه INTEL_MSR_SYSENTER_CS         0x174
#تعریف_کړه INTEL_MSR_SYSENTER_ESP        0x175
#تعریف_کړه INTEL_MSR_SYSENTER_EIP        0x176
#تعریف_کړه INTEL_MSR_PERFMON_SEL0        0x186

#تعریف_کړه INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#تعریف_کړه AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#تعریف_کړه AMD_MSR_EFER		0xC0000080	/* extended features msr */
#تعریف_کړه AMD_MSR_ستوری		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#تعریف_کړه KTS_NONE	1 /* invalid */
#تعریف_کړه KTS_INT_HARD	2 /* exception / hard interrupt */
#تعریف_کړه KTS_INT_ORIG	3 /* soft interrupt from libc */
#تعریف_کړه KTS_INT_UM	4 /* soft interrupt from usermapped code */
#تعریف_کړه KTS_FULLCONTEXT	5 /* must restore full context */
#تعریف_کړه KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#تعریف_کړه KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#د_که_پای /* _I386_ACONST_H */
