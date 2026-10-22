
#اگر_غیر_تعریف_شدہ _I386_ACONST_H
#تعریف_کرو _I386_ACONST_H 1

#شامل <machine/interrupt.h>
#شامل <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#تعریف_کرو IDT_سائز 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#تعریف_کرو KERN_CS_اشاریہ        1
#تعریف_کرو KERN_DS_اشاریہ        2
#تعریف_کرو USER_CS_اشاریہ        3
#تعریف_کرو USER_DS_اشاریہ        4
#تعریف_کرو LDT_اشاریہ            5
#تعریف_کرو TSS_اشاریہ_پہلا      6
#تعریف_کرو TSS_اشاریہ(cpu)       (TSS_اشاریہ_پہلا + (cpu)) /* per cpu kernel tss */
#تعریف_کرو GDT_سائز             (TSS_اشاریہ(CONFIG_MAX_CPUS))	/* LDT descriptor */

#تعریف_کرو SEG_SELECTOR(i)          ((i)*8)
#تعریف_کرو KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_اشاریہ)
#تعریف_کرو KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_اشاریہ)
#تعریف_کرو USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_اشاریہ) | USER_PRIVILEGE)
#تعریف_کرو USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_اشاریہ) | USER_PRIVILEGE)
#تعریف_کرو LDT_SELECTOR SEG_SELECTOR(LDT_اشاریہ)
#تعریف_کرو TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_اشاریہ(cpu))

#تعریف_کرو DESC_سائز	8

/* Privileges. */
#تعریف_کرو INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#تعریف_کرو USER_PRIVILEGE       3	/* servers and user processes */
#تعریف_کرو RPL_نقاب             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#تعریف_کرو BOUNDS_سمتیہ        5	/* bounds check failed */
#تعریف_کرو INVAL_OP_سمتیہ      6	/* invalid opcode */
#تعریف_کرو COPROC_NOT_سمتیہ    7	/* coprocessor not available */
#تعریف_کرو DOUBLE_FAULT_سمتیہ  8
#تعریف_کرو COPROC_SEG_سمتیہ    9	/* coprocessor segment overrun */
#تعریف_کرو INVAL_TSS_سمتیہ    10	/* invalid TSS */
#تعریف_کرو SEG_NOT_سمتیہ      11	/* segment not present */
#تعریف_کرو ڈھیر_FAULT_سمتیہ  12	/* stack exception */
#تعریف_کرو PROTECTION_سمتیہ   13	/* general protection */

/* Selector bits. */
#تعریف_کرو TI                0x04	/* table indicator */
#تعریف_کرو RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#تعریف_کرو بنیاد_درمیان_سرکاؤ   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#تعریف_کرو PRESENT           0x80	/* set for descriptor present */
#تعریف_کرو DPL               0x60	/* descriptor privilege level mask */
#تعریف_کرو DPL_سرکاؤ            5
#تعریف_کرو SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#تعریف_کرو EXECUTABLE        0x08	/* set for executable segment */
#تعریف_کرو CONFORMING        0x04	/* set for conforming segment if executable */
#تعریف_کرو EXPAND_نیچے_کو       0x04	/* set for expand-down segment if !executable*/
#تعریف_کرو READABLE          0x02	/* set for readable segment if executable */
#تعریف_کرو WRITEABLE         0x02	/* set for writeable segment if !executable */
#تعریف_کرو TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#تعریف_کرو ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#تعریف_کرو AVL_286_TSS          1	/* available 286 TSS */
#تعریف_کرو LDT                  2	/* local descriptor table */
#تعریف_کرو BUSY_286_TSS         3	/* set transparently to the software */
#تعریف_کرو CALL_286_GATE        4	/* not used */
#تعریف_کرو TASK_GATE            5	/* only used by debugger */
#تعریف_کرو INT_286_GATE         6	/* interrupt gate, used for all vectors */
#تعریف_کرو TRAP_286_GATE        7	/* not used */

#تعریف_کرو INT_GATE_TYPE	(INT_286_GATE | DESC_386_ثنائی_ہندسہ)
#تعریف_کرو TSS_TYPE	(AVL_286_TSS  | DESC_386_ثنائی_ہندسہ)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#تعریف_کرو صفحہ_FAULT_سمتیہ   14
#تعریف_کرو COPROC_ERR_سمتیہ   16	/* coprocessor error */
#تعریف_کرو ALIGNMENT_CHECK_سمتیہ	17
#تعریف_کرو MACHINE_CHECK_سمتیہ	18
#تعریف_کرو SIMD_EXCEPTION_سمتیہ	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#تعریف_کرو DESC_GRANULARITY     6	/* to granularity byte */
#تعریف_کرو DESC_بنیاد_بلند       7	/* to base_high */

/* Type-byte bits. */
#تعریف_کرو DESC_386_ثنائی_ہندسہ  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#تعریف_کرو بنیاد_بلند_سرکاؤ     24  /* shift for base --> base_high */
#تعریف_کرو BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#تعریف_کرو GRANULARITY_سرکاؤ   16  /* shift for limit --> granularity */
#تعریف_کرو انحراف_بلند_سرکاؤ   16  /* shift for (gate) offset --> offset_high */
#تعریف_کرو صفحہ_GRAN_سرکاؤ     12  /* extra shift for page granular limits */

/* Granularity byte. */
#تعریف_کرو GRANULAR  	  0x80	/* set for 4K granularilty */
#تعریف_کرو DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#تعریف_کرو BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#تعریف_کرو AVL        	  0x10	/* 0 for available */
#تعریف_کرو حد_بندی_بلند   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#تعریف_کرو INIT_PSW      0x0200    /* initial psw */
#تعریف_کرو INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#تعریف_کرو TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#تعریف_کرو SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#تعریف_کرو IF_نقاب 0x00000200
#تعریف_کرو IOPL_نقاب 0x003000

#تعریف_کرو INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#تعریف_کرو INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#تعریف_کرو INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#تعریف_کرو AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#تعریف_کرو AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#تعریف_کرو AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#تعریف_کرو CPU_VENDOR_INTEL	0
#تعریف_کرو CPU_VENDOR_AMD		2
#تعریف_کرو CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#تعریف_کرو FPUALIGN		16

/* Poweroff 16-bit code address */
#تعریف_کرو BIOS_POWEROFF_اندراج 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#تعریف_کرو X86_ڈھیر_چوٹی_RESERVED	(2 * حجم(reg_t))

#تعریف_کرو PG_ALLOCATEME ((phys_بائٹیں)-1)

/* MSRs */
#تعریف_کرو INTEL_MSR_PERFMON_CRT0         0xc1
#تعریف_کرو INTEL_MSR_SYSENTER_CS         0x174
#تعریف_کرو INTEL_MSR_SYSENTER_ESP        0x175
#تعریف_کرو INTEL_MSR_SYSENTER_EIP        0x176
#تعریف_کرو INTEL_MSR_PERFMON_SEL0        0x186

#تعریف_کرو INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#تعریف_کرو AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#تعریف_کرو AMD_MSR_EFER		0xC0000080	/* extended features msr */
#تعریف_کرو AMD_MSR_ستارہ		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#تعریف_کرو KTS_NONE	1 /* invalid */
#تعریف_کرو KTS_INT_HARD	2 /* exception / hard interrupt */
#تعریف_کرو KTS_INT_ORIG	3 /* soft interrupt from libc */
#تعریف_کرو KTS_INT_UM	4 /* soft interrupt from usermapped code */
#تعریف_کرو KTS_FULLCONTEXT	5 /* must restore full context */
#تعریف_کرو KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#تعریف_کرو KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#اگر_ختم /* _I386_ACONST_H */
