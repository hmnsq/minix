
#اگر_تعریف_نشده _I386_ACONST_H
#تعریف_کن _I386_ACONST_H 1

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#تعریف_کن IDT_اندازه 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#تعریف_کن KERN_CS_شاخص        1
#تعریف_کن KERN_DS_شاخص        2
#تعریف_کن USER_CS_شاخص        3
#تعریف_کن USER_DS_شاخص        4
#تعریف_کن LDT_شاخص            5
#تعریف_کن TSS_شاخص_اولی      6
#تعریف_کن TSS_شاخص(cpu)       (TSS_شاخص_اولی + (cpu)) /* per cpu kernel tss */
#تعریف_کن GDT_اندازه             (TSS_شاخص(CONFIG_MAX_CPUS))	/* LDT descriptor */

#تعریف_کن SEG_SELECTOR(i)          ((i)*8)
#تعریف_کن KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_شاخص)
#تعریف_کن KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_شاخص)
#تعریف_کن USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_شاخص) | USER_PRIVILEGE)
#تعریف_کن USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_شاخص) | USER_PRIVILEGE)
#تعریف_کن LDT_SELECTOR SEG_SELECTOR(LDT_شاخص)
#تعریف_کن TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_شاخص(cpu))

#تعریف_کن DESC_اندازه	8

/* Privileges. */
#تعریف_کن INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#تعریف_کن USER_PRIVILEGE       3	/* servers and user processes */
#تعریف_کن RPL_ماسک             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#تعریف_کن BOUNDS_وکتور        5	/* bounds check failed */
#تعریف_کن INVAL_OP_وکتور      6	/* invalid opcode */
#تعریف_کن COPROC_NOT_وکتور    7	/* coprocessor not available */
#تعریف_کن DOUBLE_FAULT_وکتور  8
#تعریف_کن COPROC_SEG_وکتور    9	/* coprocessor segment overrun */
#تعریف_کن INVAL_TSS_وکتور    10	/* invalid TSS */
#تعریف_کن SEG_NOT_وکتور      11	/* segment not present */
#تعریف_کن پشته_FAULT_وکتور  12	/* stack exception */
#تعریف_کن PROTECTION_وکتور   13	/* general protection */

/* Selector bits. */
#تعریف_کن TI                0x04	/* table indicator */
#تعریف_کن RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#تعریف_کن پایه_میانه_لغزش   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#تعریف_کن PRESENT           0x80	/* set for descriptor present */
#تعریف_کن DPL               0x60	/* descriptor privilege level mask */
#تعریف_کن DPL_لغزش            5
#تعریف_کن SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#تعریف_کن EXECUTABLE        0x08	/* set for executable segment */
#تعریف_کن CONFORMING        0x04	/* set for conforming segment if executable */
#تعریف_کن EXPAND_پایین       0x04	/* set for expand-down segment if !executable*/
#تعریف_کن READABLE          0x02	/* set for readable segment if executable */
#تعریف_کن WRITEABLE         0x02	/* set for writeable segment if !executable */
#تعریف_کن TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#تعریف_کن ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#تعریف_کن AVL_286_TSS          1	/* available 286 TSS */
#تعریف_کن LDT                  2	/* local descriptor table */
#تعریف_کن BUSY_286_TSS         3	/* set transparently to the software */
#تعریف_کن CALL_286_GATE        4	/* not used */
#تعریف_کن TASK_GATE            5	/* only used by debugger */
#تعریف_کن INT_286_GATE         6	/* interrupt gate, used for all vectors */
#تعریف_کن TRAP_286_GATE        7	/* not used */

#تعریف_کن INT_GATE_TYPE	(INT_286_GATE | DESC_386_بیت)
#تعریف_کن TSS_TYPE	(AVL_286_TSS  | DESC_386_بیت)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#تعریف_کن صفحه_FAULT_وکتور   14
#تعریف_کن COPROC_ERR_وکتور   16	/* coprocessor error */
#تعریف_کن ALIGNMENT_CHECK_وکتور	17
#تعریف_کن MACHINE_CHECK_وکتور	18
#تعریف_کن SIMD_EXCEPTION_وکتور	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#تعریف_کن DESC_GRANULARITY     6	/* to granularity byte */
#تعریف_کن DESC_پایه_بالا       7	/* to base_high */

/* Type-byte bits. */
#تعریف_کن DESC_386_بیت  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#تعریف_کن پایه_بالا_لغزش     24  /* shift for base --> base_high */
#تعریف_کن BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#تعریف_کن GRANULARITY_لغزش   16  /* shift for limit --> granularity */
#تعریف_کن جابه_جایی_بالا_لغزش   16  /* shift for (gate) offset --> offset_high */
#تعریف_کن صفحه_GRAN_لغزش     12  /* extra shift for page granular limits */

/* Granularity byte. */
#تعریف_کن GRANULAR  	  0x80	/* set for 4K granularilty */
#تعریف_کن DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#تعریف_کن BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#تعریف_کن AVL        	  0x10	/* 0 for available */
#تعریف_کن حد_بالا   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#تعریف_کن INIT_PSW      0x0200    /* initial psw */
#تعریف_کن INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#تعریف_کن TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#تعریف_کن SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#تعریف_کن IF_ماسک 0x00000200
#تعریف_کن IOPL_ماسک 0x003000

#تعریف_کن INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#تعریف_کن INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#تعریف_کن INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#تعریف_کن AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#تعریف_کن AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#تعریف_کن AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#تعریف_کن CPU_VENDOR_INTEL	0
#تعریف_کن CPU_VENDOR_AMD		2
#تعریف_کن CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#تعریف_کن FPUALIGN		16

/* Poweroff 16-bit code address */
#تعریف_کن BIOS_POWEROFF_مدخل 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کن X86_پشته_بالاترین_RESERVED	(2 * حجم(reg_t))

#تعریف_کن PG_ALLOCATEME ((phys_بایتها)-1)

/* MSRs */
#تعریف_کن INTEL_MSR_PERFMON_CRT0         0xc1
#تعریف_کن INTEL_MSR_SYSENTER_CS         0x174
#تعریف_کن INTEL_MSR_SYSENTER_ESP        0x175
#تعریف_کن INTEL_MSR_SYSENTER_EIP        0x176
#تعریف_کن INTEL_MSR_PERFMON_SEL0        0x186

#تعریف_کن INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#تعریف_کن AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#تعریف_کن AMD_MSR_EFER		0xC0000080	/* extended features msr */
#تعریف_کن AMD_MSR_ستاره		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#تعریف_کن KTS_NONE	1 /* invalid */
#تعریف_کن KTS_INT_HARD	2 /* exception / hard interrupt */
#تعریف_کن KTS_INT_ORIG	3 /* soft interrupt from libc */
#تعریف_کن KTS_INT_UM	4 /* soft interrupt from usermapped code */
#تعریف_کن KTS_FULLCONTEXT	5 /* must restore full context */
#تعریف_کن KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#تعریف_کن KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#پایان_اگر /* _I386_ACONST_H */
