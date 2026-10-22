
#جيڪڏهن_وصف_نه_ٿيل _I386_ACONST_H
#وصف_ڪريو _I386_ACONST_H 1

#ضمن <machine/interrupt.h>
#ضمن <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#وصف_ڪريو IDT_سائيز 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#وصف_ڪريو KERN_CS_اشاريو        1
#وصف_ڪريو KERN_DS_اشاريو        2
#وصف_ڪريو USER_CS_اشاريو        3
#وصف_ڪريو USER_DS_اشاريو        4
#وصف_ڪريو LDT_اشاريو            5
#وصف_ڪريو TSS_اشاريو_پهريون      6
#وصف_ڪريو TSS_اشاريو(cpu)       (TSS_اشاريو_پهريون + (cpu)) /* per cpu kernel tss */
#وصف_ڪريو GDT_سائيز             (TSS_اشاريو(CONFIG_MAX_CPUS))	/* LDT descriptor */

#وصف_ڪريو SEG_SELECTOR(i)          ((i)*8)
#وصف_ڪريو KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_اشاريو)
#وصف_ڪريو KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_اشاريو)
#وصف_ڪريو USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_اشاريو) | USER_PRIVILEGE)
#وصف_ڪريو USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_اشاريو) | USER_PRIVILEGE)
#وصف_ڪريو LDT_SELECTOR SEG_SELECTOR(LDT_اشاريو)
#وصف_ڪريو TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_اشاريو(cpu))

#وصف_ڪريو DESC_سائيز	8

/* Privileges. */
#وصف_ڪريو INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#وصف_ڪريو USER_PRIVILEGE       3	/* servers and user processes */
#وصف_ڪريو RPL_نقاب             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#وصف_ڪريو BOUNDS_ويڪٽر        5	/* bounds check failed */
#وصف_ڪريو INVAL_OP_ويڪٽر      6	/* invalid opcode */
#وصف_ڪريو COPROC_NOT_ويڪٽر    7	/* coprocessor not available */
#وصف_ڪريو DOUBLE_FAULT_ويڪٽر  8
#وصف_ڪريو COPROC_SEG_ويڪٽر    9	/* coprocessor segment overrun */
#وصف_ڪريو INVAL_TSS_ويڪٽر    10	/* invalid TSS */
#وصف_ڪريو SEG_NOT_ويڪٽر      11	/* segment not present */
#وصف_ڪريو ڍير_FAULT_ويڪٽر  12	/* stack exception */
#وصف_ڪريو PROTECTION_ويڪٽر   13	/* general protection */

/* Selector bits. */
#وصف_ڪريو TI                0x04	/* table indicator */
#وصف_ڪريو RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#وصف_ڪريو بنياد_وچ_سرڪايو   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#وصف_ڪريو PRESENT           0x80	/* set for descriptor present */
#وصف_ڪريو DPL               0x60	/* descriptor privilege level mask */
#وصف_ڪريو DPL_سرڪايو            5
#وصف_ڪريو SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#وصف_ڪريو EXECUTABLE        0x08	/* set for executable segment */
#وصف_ڪريو CONFORMING        0x04	/* set for conforming segment if executable */
#وصف_ڪريو EXPAND_هيٺ_ڏانهن       0x04	/* set for expand-down segment if !executable*/
#وصف_ڪريو READABLE          0x02	/* set for readable segment if executable */
#وصف_ڪريو WRITEABLE         0x02	/* set for writeable segment if !executable */
#وصف_ڪريو TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#وصف_ڪريو ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#وصف_ڪريو AVL_286_TSS          1	/* available 286 TSS */
#وصف_ڪريو LDT                  2	/* local descriptor table */
#وصف_ڪريو BUSY_286_TSS         3	/* set transparently to the software */
#وصف_ڪريو CALL_286_GATE        4	/* not used */
#وصف_ڪريو TASK_GATE            5	/* only used by debugger */
#وصف_ڪريو INT_286_GATE         6	/* interrupt gate, used for all vectors */
#وصف_ڪريو TRAP_286_GATE        7	/* not used */

#وصف_ڪريو INT_GATE_TYPE	(INT_286_GATE | DESC_386_ٻٽو_انگ)
#وصف_ڪريو TSS_TYPE	(AVL_286_TSS  | DESC_386_ٻٽو_انگ)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#وصف_ڪريو صفحو_FAULT_ويڪٽر   14
#وصف_ڪريو COPROC_ERR_ويڪٽر   16	/* coprocessor error */
#وصف_ڪريو ALIGNMENT_CHECK_ويڪٽر	17
#وصف_ڪريو MACHINE_CHECK_ويڪٽر	18
#وصف_ڪريو SIMD_EXCEPTION_ويڪٽر	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#وصف_ڪريو DESC_GRANULARITY     6	/* to granularity byte */
#وصف_ڪريو DESC_بنياد_مٿاهون       7	/* to base_high */

/* Type-byte bits. */
#وصف_ڪريو DESC_386_ٻٽو_انگ  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#وصف_ڪريو بنياد_مٿاهون_سرڪايو     24  /* shift for base --> base_high */
#وصف_ڪريو BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#وصف_ڪريو GRANULARITY_سرڪايو   16  /* shift for limit --> granularity */
#وصف_ڪريو وٿي_مٿاهون_سرڪايو   16  /* shift for (gate) offset --> offset_high */
#وصف_ڪريو صفحو_GRAN_سرڪايو     12  /* extra shift for page granular limits */

/* Granularity byte. */
#وصف_ڪريو GRANULAR  	  0x80	/* set for 4K granularilty */
#وصف_ڪريو DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#وصف_ڪريو BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#وصف_ڪريو AVL        	  0x10	/* 0 for available */
#وصف_ڪريو حد_بندي_مٿاهون   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#وصف_ڪريو INIT_PSW      0x0200    /* initial psw */
#وصف_ڪريو INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#وصف_ڪريو TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#وصف_ڪريو SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#وصف_ڪريو IF_نقاب 0x00000200
#وصف_ڪريو IOPL_نقاب 0x003000

#وصف_ڪريو INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#وصف_ڪريو INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#وصف_ڪريو INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#وصف_ڪريو AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#وصف_ڪريو AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#وصف_ڪريو AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#وصف_ڪريو CPU_VENDOR_INTEL	0
#وصف_ڪريو CPU_VENDOR_AMD		2
#وصف_ڪريو CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#وصف_ڪريو FPUALIGN		16

/* Poweroff 16-bit code address */
#وصف_ڪريو BIOS_POWEROFF_داخلا 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#وصف_ڪريو X86_ڍير_چوٽي_RESERVED	(2 * حجم(reg_t))

#وصف_ڪريو PG_ALLOCATEME ((phys_بائيٽون)-1)

/* MSRs */
#وصف_ڪريو INTEL_MSR_PERFMON_CRT0         0xc1
#وصف_ڪريو INTEL_MSR_SYSENTER_CS         0x174
#وصف_ڪريو INTEL_MSR_SYSENTER_ESP        0x175
#وصف_ڪريو INTEL_MSR_SYSENTER_EIP        0x176
#وصف_ڪريو INTEL_MSR_PERFMON_SEL0        0x186

#وصف_ڪريو INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#وصف_ڪريو AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#وصف_ڪريو AMD_MSR_EFER		0xC0000080	/* extended features msr */
#وصف_ڪريو AMD_MSR_تارو		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#وصف_ڪريو KTS_NONE	1 /* invalid */
#وصف_ڪريو KTS_INT_HARD	2 /* exception / hard interrupt */
#وصف_ڪريو KTS_INT_ORIG	3 /* soft interrupt from libc */
#وصف_ڪريو KTS_INT_UM	4 /* soft interrupt from usermapped code */
#وصف_ڪريو KTS_FULLCONTEXT	5 /* must restore full context */
#وصف_ڪريو KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#وصف_ڪريو KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#جيڪڏهن_پورو /* _I386_ACONST_H */
