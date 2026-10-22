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

#ಅನ್ತರ್ಭಾವ <minix/config.h>
#ಅನ್ತರ್ಭಾವ <minix/ipcconst.h>
#ಅನ್ತರ್ಭಾವ <machine/archtypes.h>
#ಅನ್ತರ್ಭಾವ "archconst.h"
#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
ಬಾಹ್ಯ ಸಂರಚನಾ kinfo kinfo;		  /* kernel information for services */
ಬಾಹ್ಯ ಸಂರಚನಾ machine machine;		  /* machine info for services */
ಬಾಹ್ಯ ಸಂರಚನಾ kmessages kmessages;  	  /* diagnostic messages in kernel */
ಬಾಹ್ಯ ಸಂರಚನಾ loadinfo loadinfo;	  /* status of load average */
ಬಾಹ್ಯ ಸಂರಚನಾ kuserinfo kuserinfo;	  /* kernel information for users */
ಬಾಹ್ಯ ಸಂರಚನಾ arm_frclock arm_frclock;	  /* ARM free-running timer info */
ಬಾಹ್ಯ ಸಂರಚನಾ kclockinfo kclockinfo;	  /* clock information */
ಬಾಹ್ಯ ಸಂರಚನಾ minix_kerninfo minix_kerninfo;

EXTERN ಸಂರಚನಾ k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN ಸಂರಚನಾ proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN ವರ್ಣ *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN ಸಂರಚನಾ proc *kbill_kcall; /* process that made kernel call */
EXTERN ಸಂರಚನಾ proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN ಪೂರ್ಣಾಙ್ಕ irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN ಪೂರ್ಣಾಙ್ಕ irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN ಪೂರ್ಣಾಙ್ಕ verboseboot;			/* verbose boot, init'ed in cstart */

#ಯದಿ DEBUG_TRACE
EXTERN ಪೂರ್ಣಾಙ್ಕ verboseflags;
#endif

#ifdef USE_APIC
EXTERN ಪೂರ್ಣಾಙ್ಕ config_no_apic; /* optionally turn off apic */
EXTERN ಪೂರ್ಣಾಙ್ಕ config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	ಕುರು {cpu_hz[cpu] = freq;} ಯಾವತ್ (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN ಪೂರ್ಣಾಙ್ಕ config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN ಪೂರ್ಣಾಙ್ಕ vm_running;
EXTERN ಪೂರ್ಣಾಙ್ಕ catch_pagefaults;
EXTERN ಪೂರ್ಣಾಙ್ಕ kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
ಬಾಹ್ಯ ಸಂರಚನಾ boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ serial_debug_active;

EXTERN ಸಂರಚನಾ cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN ಪೂರ್ಣಾಙ್ಕ minix_feature_flags;

#endif /* GLO_H */
