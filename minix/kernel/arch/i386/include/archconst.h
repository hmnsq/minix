
#अगर_अघोषित _I386_ACONST_H
#घोषणा _I386_ACONST_H 1

#समावेश <machine/interrupt.h>
#समावेश <machine/memory.h>

/* Constants for protected mode. */

/* Table sizes. */
#घोषणा IDT_आकार 256	/* the table is set to it's maximal size */

/* GDT layout (SYSENTER/SYSEXIT compliant) */
#घोषणा KERN_CS_सूचकांक        1
#घोषणा KERN_DS_सूचकांक        2
#घोषणा USER_CS_सूचकांक        3
#घोषणा USER_DS_सूचकांक        4
#घोषणा LDT_सूचकांक            5
#घोषणा TSS_सूचकांक_FIRST      6
#घोषणा TSS_सूचकांक(cpu)       (TSS_सूचकांक_FIRST + (cpu)) /* per cpu kernel tss */
#घोषणा GDT_आकार             (TSS_सूचकांक(CONFIG_MAX_CPUS))	/* LDT descriptor */

#घोषणा SEG_SELECTOR(i)          ((i)*8)
#घोषणा KERN_CS_SELECTOR SEG_SELECTOR(KERN_CS_सूचकांक)
#घोषणा KERN_DS_SELECTOR SEG_SELECTOR(KERN_DS_सूचकांक)
#घोषणा USER_CS_SELECTOR (SEG_SELECTOR(USER_CS_सूचकांक) | USER_PRIVILEGE)
#घोषणा USER_DS_SELECTOR (SEG_SELECTOR(USER_DS_सूचकांक) | USER_PRIVILEGE)
#घोषणा LDT_SELECTOR SEG_SELECTOR(LDT_सूचकांक)
#घोषणा TSS_SELECTOR(cpu)	SEG_SELECTOR(TSS_सूचकांक(cpu))

#घोषणा DESC_आकार	8

/* Privileges. */
#घोषणा INTR_PRIVILEGE       0	/* kernel and interrupt handlers */
#घोषणा USER_PRIVILEGE       3	/* servers and user processes */
#घोषणा RPL_MASK             0x03	/* bits in selector RPL */

/* 286 hardware constants. */

/* Exception vector numbers. */
#घोषणा BOUNDS_VECTOR        5	/* bounds check failed */
#घोषणा INVAL_OP_VECTOR      6	/* invalid opcode */
#घोषणा COPROC_NOT_VECTOR    7	/* coprocessor not available */
#घोषणा DOUBLE_FAULT_VECTOR  8
#घोषणा COPROC_SEG_VECTOR    9	/* coprocessor segment overrun */
#घोषणा INVAL_TSS_VECTOR    10	/* invalid TSS */
#घोषणा SEG_NOT_VECTOR      11	/* segment not present */
#घोषणा ढेर_FAULT_VECTOR  12	/* stack exception */
#घोषणा PROTECTION_VECTOR   13	/* general protection */

/* Selector bits. */
#घोषणा TI                0x04	/* table indicator */
#घोषणा RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#घोषणा BASE_मध्य_SHIFT   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#घोषणा PRESENT           0x80	/* set for descriptor present */
#घोषणा DPL               0x60	/* descriptor privilege level mask */
#घोषणा DPL_SHIFT            5
#घोषणा SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#घोषणा EXECUTABLE        0x08	/* set for executable segment */
#घोषणा CONFORMING        0x04	/* set for conforming segment if executable */
#घोषणा EXPAND_DOWN       0x04	/* set for expand-down segment if !executable*/
#घोषणा READABLE          0x02	/* set for readable segment if executable */
#घोषणा WRITEABLE         0x02	/* set for writeable segment if !executable */
#घोषणा TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#घोषणा ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#घोषणा AVL_286_TSS          1	/* available 286 TSS */
#घोषणा LDT                  2	/* local descriptor table */
#घोषणा BUSY_286_TSS         3	/* set transparently to the software */
#घोषणा CALL_286_GATE        4	/* not used */
#घोषणा TASK_GATE            5	/* only used by debugger */
#घोषणा INT_286_GATE         6	/* interrupt gate, used for all vectors */
#घोषणा TRAP_286_GATE        7	/* not used */

#घोषणा INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#घोषणा TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)


/* Extra 386 hardware constants. */

/* Exception vector numbers. */
#घोषणा PAGE_FAULT_VECTOR   14
#घोषणा COPROC_ERR_VECTOR   16	/* coprocessor error */
#घोषणा ALIGNMENT_CHECK_VECTOR	17
#घोषणा MACHINE_CHECK_VECTOR	18
#घोषणा SIMD_EXCEPTION_VECTOR	19     /* SIMD Floating-Point Exception (#XM) */

/* Descriptor structure offsets. */
#घोषणा DESC_GRANULARITY     6	/* to granularity byte */
#घोषणा DESC_BASE_ऊपरी       7	/* to base_high */

/* Type-byte bits. */
#घोषणा DESC_386_BIT  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

/* Base and limit sizes and shifts. */ 
#घोषणा BASE_ऊपरी_SHIFT     24  /* shift for base --> base_high */
#घोषणा BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#घोषणा GRANULARITY_SHIFT   16  /* shift for limit --> granularity */
#घोषणा OFFSET_ऊपरी_SHIFT   16  /* shift for (gate) offset --> offset_high */
#घोषणा PAGE_GRAN_SHIFT     12  /* extra shift for page granular limits */

/* Granularity byte. */
#घोषणा GRANULAR  	  0x80	/* set for 4K granularilty */
#घोषणा DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#घोषणा BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#घोषणा AVL        	  0x10	/* 0 for available */
#घोषणा LIMIT_ऊपरी   	  0x0F	/* mask for high bits of limit */

/* Program stack words and masks. */
#घोषणा INIT_PSW      0x0200    /* initial psw */
#घोषणा INIT_TASK_PSW 0x1200    /* initial psw for tasks (with IOPL 1) */
#घोषणा TRACEBIT      0x0100    /* OR this with psw in proc[] for tracing */
#घोषणा SETPSW(rp, new)         /* permits only certain bits to be set */ \
        ((rp)->p_reg.psw = ((rp)->p_reg.psw & ~0xCD5) | ((new) & 0xCD5))
#घोषणा IF_MASK 0x00000200
#घोषणा IOPL_MASK 0x003000

#घोषणा INTEL_CPUID_GEN_EBX	0x756e6547 /* ASCII value of "Genu" */
#घोषणा INTEL_CPUID_GEN_EDX	0x49656e69 /* ASCII value of "ineI" */
#घोषणा INTEL_CPUID_GEN_ECX	0x6c65746e /* ASCII value of "ntel" */

#घोषणा AMD_CPUID_GEN_EBX	0x68747541 /* ASCII value of "Auth" */
#घोषणा AMD_CPUID_GEN_EDX	0x69746e65 /* ASCII value of "enti" */
#घोषणा AMD_CPUID_GEN_ECX	0x444d4163 /* ASCII value of "cAMD" */

#घोषणा CPU_VENDOR_INTEL	0
#घोषणा CPU_VENDOR_AMD		2
#घोषणा CPU_VENDOR_UNKNOWN	0xff

/* fpu context should be saved in 16-byte aligned memory */
#घोषणा FPUALIGN		16

/* Poweroff 16-bit code address */
#घोषणा BIOS_POWEROFF_ENTRY 0x1000


/* 
 * defines how many bytes are reserved at the top of the kernel stack for global
 * information like currently scheduled process or current cpu id
 */
#घोषणा X86_ढेर_शीर्ष_RESERVED	(2 * माप(reg_t))

#घोषणा PG_ALLOCATEME ((phys_bytes)-1)

/* MSRs */
#घोषणा INTEL_MSR_PERFMON_CRT0         0xc1
#घोषणा INTEL_MSR_SYSENTER_CS         0x174
#घोषणा INTEL_MSR_SYSENTER_ESP        0x175
#घोषणा INTEL_MSR_SYSENTER_EIP        0x176
#घोषणा INTEL_MSR_PERFMON_SEL0        0x186

#घोषणा INTEL_MSR_PERFMON_SEL0_ENABLE (1 << 22)

#घोषणा AMD_EFER_SCE		(1L << 0) /* SYSCALL/SYSRET enabled */
#घोषणा AMD_MSR_EFER		0xC0000080	/* extended features msr */
#घोषणा AMD_MSR_STAR		0xC0000081	/* SYSCALL params msr */

/* trap styles recorded on kernel entry and exit */
#घोषणा KTS_NONE	1 /* invalid */
#घोषणा KTS_INT_HARD	2 /* exception / hard interrupt */
#घोषणा KTS_INT_ORIG	3 /* soft interrupt from libc */
#घोषणा KTS_INT_UM	4 /* soft interrupt from usermapped code */
#घोषणा KTS_FULLCONTEXT	5 /* must restore full context */
#घोषणा KTS_SYSENTER	6 /* SYSENTER instruction (usermapped) */
#घोषणा KTS_SYSCALL	7 /* SYSCALL instruction (usermapped) */

#पूर्ण_अगर /* _I386_ACONST_H */
