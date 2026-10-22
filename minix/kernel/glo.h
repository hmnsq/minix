#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GLO_H
#ವ್ಯಾಖ್ಯಾನಿಸು GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಕೋಷ್ಟಕ_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು EXTERN
#ವ್ಯಾಖ್ಯಾನಿಸು EXTERN
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <minix/config.h>
#ಅನ್ತರ್ಭಾವ <minix/ipcconst.h>
#ಅನ್ತರ್ಭಾವ <machine/archtypes.h>
#ಅನ್ತರ್ಭಾವ "archconst.h"
#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
ಬಾಹ್ಯ ರಚನೆ kinfo kinfo;		  /* kernel information for services */
ಬಾಹ್ಯ ರಚನೆ machine machine;		  /* machine info for services */
ಬಾಹ್ಯ ರಚನೆ kmessages kmessages;  	  /* diagnostic messages in kernel */
ಬಾಹ್ಯ ರಚನೆ loadinfo loadinfo;	  /* status of load average */
ಬಾಹ್ಯ ರಚನೆ kuserinfo kuserinfo;	  /* kernel information for users */
ಬಾಹ್ಯ ರಚನೆ arm_frclock arm_frclock;	  /* ARM free-running timer info */
ಬಾಹ್ಯ ರಚನೆ kclockinfo kclockinfo;	  /* clock information */
ಬಾಹ್ಯ ರಚನೆ minix_kerninfo minix_kerninfo;

EXTERN ರಚನೆ k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_ಬೈಟುಗಳು minix_kerninfo_user;

#ವ್ಯಾಖ್ಯಾನಿಸು kmess kmessages
#ವ್ಯಾಖ್ಯಾನಿಸು kloadinfo loadinfo

#ವ್ಯಾಖ್ಯಾನಿಸು system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN ರಚನೆ proc *vmrequest;  /* first process on vmrequest queue */
EXTERN ಚಿಹ್ನೆರಹಿತ ಕಳೆದುಹೋದ_ticks;	/* clock ticks counted outside clock task */
EXTERN ಅಕ್ಷರ *ipc_call_ಹೆಸರುಗಳು[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN ರಚನೆ proc *kbill_kcall; /* process that made kernel call */
EXTERN ರಚನೆ proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN ಪೂರ್ಣಾಂಕ irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN ಪೂರ್ಣಾಂಕ irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN ಪೂರ್ಣಾಂಕ verboseboot;			/* verbose boot, init'ed in cstart */

#ಆದರೆ DEBUG_TRACE
EXTERN ಪೂರ್ಣಾಂಕ verboseflags;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
EXTERN ಪೂರ್ಣಾಂಕ config_no_apic; /* optionally turn off apic */
EXTERN ಪೂರ್ಣಾಂಕ config_apic_timer_x; /* apic timer slowdown factor */
#ಆದರೆ_ಅಂತ್ಯ

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#ವ್ಯಾಖ್ಯಾನಿಸು cpu_ಗಣ_freq(cpu, freq)	ಮಾಡು {cpu_hz[cpu] = freq;} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು cpu_get_freq(cpu)	cpu_hz[cpu]

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
EXTERN ಪೂರ್ಣಾಂಕ config_no_smp; /* optionally turn off SMP */
#ಆದರೆ_ಅಂತ್ಯ

/* VM */
EXTERN ಪೂರ್ಣಾಂಕ vm_running;
EXTERN ಪೂರ್ಣಾಂಕ catch_pagefaults;
EXTERN ಪೂರ್ಣಾಂಕ kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
ಬಾಹ್ಯ ರಚನೆ boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_debug_ಸಕ್ರಿಯ;

EXTERN ರಚನೆ cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN ಚಿಹ್ನೆರಹಿತ bkl_tries[CONFIG_MAX_CPUS];
EXTERN ಚಿಹ್ನೆರಹಿತ bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN ಪೂರ್ಣಾಂಕ minix_feature_flags;

#ಆದರೆ_ಅಂತ್ಯ /* GLO_H */
