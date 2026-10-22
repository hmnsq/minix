
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _I386_ACONST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _I386_ACONST_H 1

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDT_ਮਾਪ 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERN_CS_ਸੂਚਕਾਂਕ        1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERN_DS_ਸੂਚਕਾਂਕ        2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_CS_ਸੂਚਕਾਂਕ        3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_DS_ਸੂਚਕਾਂਕ        4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDT_ਸੂਚਕਾਂਕ            5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_ਸੂਚਕਾਂਕ_ਪਹਿਲਾ      6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_ਸੂਚਕਾਂਕ(cpu)       (TSS_ਸੂਚਕਾਂਕ_ਪਹਿਲਾ + (cpu)) /* per cpu kernel tss */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GDT_ਮਾਪ             (TSS_ਸੂਚਕਾਂਕ(CONFIG_MAX_CPUS))	/* LDT descriptor */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEG_SELECTOR(i)          ((i)*8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_ਸੂਚਕਾਂਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_ਸੂਚਕਾਂਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_ਸੂਚਕਾਂਕ) | USER_PRIVILEGE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_ਸੂਚਕਾਂਕ) | USER_PRIVILEGE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDT_SELECTOR SEG_SELECTOR(LDT_ਸੂਚਕਾਂਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_ਸੂਚਕਾਂਕ(cpu))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESC_ਮਾਪ	8

/* Privileges. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USER_PRIVILEGE       3	/* servers and user processes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RPL_ਨਕਾਬ             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOUNDS_ਸਦਿਸ਼        5	/* bounds check failed */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVAL_OP_ਸਦਿਸ਼      6	/* invalid opcode */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPROC_NOT_ਸਦਿਸ਼    7	/* coprocessor not available */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOUBLE_FAULT_ਸਦਿਸ਼  8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPROC_SEG_ਸਦਿਸ਼    9	/* coprocessor segment overrun */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVAL_TSS_ਸਦਿਸ਼    10	/* invalid TSS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEG_NOT_ਸਦਿਸ਼      11	/* segment not present */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_FAULT_ਸਦਿਸ਼  12	/* stack exception */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTECTION_ਸਦਿਸ਼   13	/* general protection */

/* Selector bits. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TI                0x04	/* table indicator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਧਾਰ_ਵਿਚਕਾਰ_ਖਿਸਕਾਓ   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESENT           0x80	/* set for descriptor present */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DPL               0x60	/* descriptor privilege level mask */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DPL_ਖਿਸਕਾਓ            5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXECUTABLE        0x08	/* set for executable segment */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFORMING        0x04	/* set for conforming segment if executable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_ਥੱਲੇ       0x04	/* set for expand-down segment if !executable*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READABLE          0x02	/* set for readable segment if executable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WRITEABLE         0x02	/* set for writeable segment if !executable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AVL_286_TSS          1	/* available 286 TSS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDT                  2	/* local descriptor table */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUSY_286_TSS         3	/* set transparently to the software */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CALL_286_GATE        4	/* not used */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TASK_GATE            5	/* only used by debugger */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_286_GATE         6	/* interrupt gate, used for all vectors */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRAP_286_GATE        7	/* not used */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_GATE_TYPE	(INT_286_GATE | DESC_386_ਦੋਆਧਾਰੀ_ਅੰਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_TYPE	(AVL_286_TSS  | DESC_386_ਦੋਆਧਾਰੀ_ਅੰਕ)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_FAULT_ਸਦਿਸ਼   14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COPROC_ERR_ਸਦਿਸ਼   16	/* coprocessor error */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGNMENT_CHECK_ਸਦਿਸ਼	17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MACHINE_CHECK_ਸਦਿਸ਼	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIMD_EXCEPTION_ਸਦਿਸ਼	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESC_GRANULARITY     6	/* to granularity byte */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESC_ਅਧਾਰ_ਉੱਚਾ       7	/* to base_high */

/* Type-byte bits. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DESC_386_ਦੋਆਧਾਰੀ_ਅੰਕ  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਧਾਰ_ਉੱਚਾ_ਖਿਸਕਾਓ     24  /* shift for base --> base_high */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GRANULARITY_ਖਿਸਕਾਓ   16  /* shift for limit --> granularity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਥ_ਉੱਚਾ_ਖਿਸਕਾਓ   16  /* shift for (gate) offset --> offset_high */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_GRAN_ਖਿਸਕਾਓ     12  /* extra shift for page granular limits */

/* Granularity byte. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GRANULAR  	  0x80	/* set for 4K granularilty */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AVL        	  0x10	/* 0 for available */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ_ਉੱਚਾ   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INIT_PSW      0x0200    /* initial psw */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IF_ਨਕਾਬ 0x00000200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOPL_ਨਕਾਬ 0x003000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPU_VENDOR_INTEL	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPU_VENDOR_AMD		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPUALIGN		16

/* Poweroff 16-bit code address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIOS_POWEROFF_ਇੰਦਰਾਜ 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_ਢੇਰ_ਸਿਖਰ_RESERVED	(2 * ਆਕਾਰ(reg_t))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PG_ALLOCATEME ((phys_ਬਾਈਟਾਂ)-1)

/* MSRs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_PERFMON_CRT0         0xc1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_SYSENTER_CS         0x174
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_SYSENTER_ESP        0x175
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_SYSENTER_EIP        0x176
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_PERFMON_SEL0        0x186

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_MSR_EFER		0xC0000080	/* extended features msr */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_MSR_ਤਾਰਾ		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_NONE	1 /* invalid */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_INT_HARD	2 /* exception / hard interrupt */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_INT_ORIG	3 /* soft interrupt from libc */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_INT_UM	4 /* soft interrupt from usermapped code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_FULLCONTEXT	5 /* must restore full context */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#ਜੇ_ਅੰਤ /* _I386_ACONST_H */
