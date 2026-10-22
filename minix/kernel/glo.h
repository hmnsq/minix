#ifndef GLO_H
#define GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#ifdef _TABLE
#undef EXTERN
#define EXTERN
#endif

#அந்தர்பாவ <minix/config.h>
#அந்தர்பாவ <minix/ipcconst.h>
#அந்தர்பாவ <machine/archtypes.h>
#அந்தர்பாவ "archconst.h"
#அந்தர்பாவ "config.h"
#அந்தர்பாவ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
வெளி கட்டமைப்பு kinfo kinfo;		  /* kernel information for services */
வெளி கட்டமைப்பு machine machine;		  /* machine info for services */
வெளி கட்டமைப்பு kmessages kmessages;  	  /* diagnostic messages in kernel */
வெளி கட்டமைப்பு loadinfo loadinfo;	  /* status of load average */
வெளி கட்டமைப்பு kuserinfo kuserinfo;	  /* kernel information for users */
வெளி கட்டமைப்பு arm_frclock arm_frclock;	  /* ARM free-running timer info */
வெளி கட்டமைப்பு kclockinfo kclockinfo;	  /* clock information */
வெளி கட்டமைப்பு minix_kerninfo minix_kerninfo;

EXTERN கட்டமைப்பு k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN கட்டமைப்பு proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN எழுத்து *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN கட்டமைப்பு proc *kbill_kcall; /* process that made kernel call */
EXTERN கட்டமைப்பு proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN முழு irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN முழு irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN முழு verboseboot;			/* verbose boot, init'ed in cstart */

#எனில் DEBUG_TRACE
EXTERN முழு verboseflags;
#endif

#ifdef USE_APIC
EXTERN முழு config_no_apic; /* optionally turn off apic */
EXTERN முழு config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	செய் {cpu_hz[cpu] = freq;} போது (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN முழு config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN முழு vm_running;
EXTERN முழு catch_pagefaults;
EXTERN முழு kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
வெளி கட்டமைப்பு boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN நிலையற்ற முழு serial_debug_active;

EXTERN கட்டமைப்பு cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN முழு minix_feature_flags;

#endif /* GLO_H */
