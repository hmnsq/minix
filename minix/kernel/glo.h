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

#অন্তর্ভাব <minix/config.h>
#অন্তর্ভাব <minix/ipcconst.h>
#অন্তর্ভাব <machine/archtypes.h>
#অন্তর্ভাব "archconst.h"
#অন্তর্ভাব "config.h"
#অন্তর্ভাব "debug.h"

/* Kernel information structures. This groups vital kernel information. */
বাহ্যিক কাঠামো kinfo kinfo;		  /* kernel information for services */
বাহ্যিক কাঠামো machine machine;		  /* machine info for services */
বাহ্যিক কাঠামো kmessages kmessages;  	  /* diagnostic messages in kernel */
বাহ্যিক কাঠামো loadinfo loadinfo;	  /* status of load average */
বাহ্যিক কাঠামো kuserinfo kuserinfo;	  /* kernel information for users */
বাহ্যিক কাঠামো arm_frclock arm_frclock;	  /* ARM free-running timer info */
বাহ্যিক কাঠামো kclockinfo kclockinfo;	  /* clock information */
বাহ্যিক কাঠামো minix_kerninfo minix_kerninfo;

EXTERN কাঠামো k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN কাঠামো proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN অক্ষর *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN কাঠামো proc *kbill_kcall; /* process that made kernel call */
EXTERN কাঠামো proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN পূর্ণসংখ্যা irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN পূর্ণসংখ্যা irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN পূর্ণসংখ্যা verboseboot;			/* verbose boot, init'ed in cstart */

#যদি DEBUG_TRACE
EXTERN পূর্ণসংখ্যা verboseflags;
#endif

#ifdef USE_APIC
EXTERN পূর্ণসংখ্যা config_no_apic; /* optionally turn off apic */
EXTERN পূর্ণসংখ্যা config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	করো {cpu_hz[cpu] = freq;} যখন (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN পূর্ণসংখ্যা config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN পূর্ণসংখ্যা vm_running;
EXTERN পূর্ণসংখ্যা catch_pagefaults;
EXTERN পূর্ণসংখ্যা kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
বাহ্যিক কাঠামো boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN অস্থির পূর্ণসংখ্যা serial_debug_active;

EXTERN কাঠামো cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN পূর্ণসংখ্যা minix_feature_flags;

#endif /* GLO_H */
