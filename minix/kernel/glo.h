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

#अन्तर्भाव <minix/config.h>
#अन्तर्भाव <minix/ipcconst.h>
#अन्तर्भाव <machine/archtypes.h>
#अन्तर्भाव "archconst.h"
#अन्तर्भाव "config.h"
#अन्तर्भाव "debug.h"

/* Kernel information structures. This groups vital kernel information. */
बाह्य संरचना kinfo kinfo;		  /* kernel information for services */
बाह्य संरचना machine machine;		  /* machine info for services */
बाह्य संरचना kmessages kmessages;  	  /* diagnostic messages in kernel */
बाह्य संरचना loadinfo loadinfo;	  /* status of load average */
बाह्य संरचना kuserinfo kuserinfo;	  /* kernel information for users */
बाह्य संरचना arm_frclock arm_frclock;	  /* ARM free-running timer info */
बाह्य संरचना kclockinfo kclockinfo;	  /* clock information */
बाह्य संरचना minix_kerninfo minix_kerninfo;

EXTERN संरचना k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN संरचना proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN वर्ण *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN संरचना proc *kbill_kcall; /* process that made kernel call */
EXTERN संरचना proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN पूर्णाङ्क irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN पूर्णाङ्क irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN पूर्णाङ्क verboseboot;			/* verbose boot, init'ed in cstart */

#सचे DEBUG_TRACE
EXTERN पूर्णाङ्क verboseflags;
#endif

#ifdef USE_APIC
EXTERN पूर्णाङ्क config_no_apic; /* optionally turn off apic */
EXTERN पूर्णाङ्क config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	करोहि {cpu_hz[cpu] = freq;} याव (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN पूर्णाङ्क config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN पूर्णाङ्क vm_running;
EXTERN पूर्णाङ्क catch_pagefaults;
EXTERN पूर्णाङ्क kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
बाह्य संरचना boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN चञ्चल पूर्णाङ्क serial_debug_active;

EXTERN संरचना cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN पूर्णाङ्क minix_feature_flags;

#endif /* GLO_H */
