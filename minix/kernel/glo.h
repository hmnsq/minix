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

#അന്തര്ഭാവ <minix/config.h>
#അന്തര്ഭാവ <minix/ipcconst.h>
#അന്തര്ഭാവ <machine/archtypes.h>
#അന്തര്ഭാവ "archconst.h"
#അന്തര്ഭാവ "config.h"
#അന്തര്ഭാവ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
ബാഹ്യ ഘടന kinfo kinfo;		  /* kernel information for services */
ബാഹ്യ ഘടന machine machine;		  /* machine info for services */
ബാഹ്യ ഘടന kmessages kmessages;  	  /* diagnostic messages in kernel */
ബാഹ്യ ഘടന loadinfo loadinfo;	  /* status of load average */
ബാഹ്യ ഘടന kuserinfo kuserinfo;	  /* kernel information for users */
ബാഹ്യ ഘടന arm_frclock arm_frclock;	  /* ARM free-running timer info */
ബാഹ്യ ഘടന kclockinfo kclockinfo;	  /* clock information */
ബാഹ്യ ഘടന minix_kerninfo minix_kerninfo;

EXTERN ഘടന k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN ഘടന proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN അക്ഷരം *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN ഘടന proc *kbill_kcall; /* process that made kernel call */
EXTERN ഘടന proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN പൂർണ്ണസംഖ്യ irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN പൂർണ്ണസംഖ്യ irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN പൂർണ്ണസംഖ്യ verboseboot;			/* verbose boot, init'ed in cstart */

#എങ്കിൽ DEBUG_TRACE
EXTERN പൂർണ്ണസംഖ്യ verboseflags;
#endif

#ifdef USE_APIC
EXTERN പൂർണ്ണസംഖ്യ config_no_apic; /* optionally turn off apic */
EXTERN പൂർണ്ണസംഖ്യ config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	ചെയ്യുക {cpu_hz[cpu] = freq;} ആകുമ്പോൾ (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN പൂർണ്ണസംഖ്യ config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN പൂർണ്ണസംഖ്യ vm_running;
EXTERN പൂർണ്ണസംഖ്യ catch_pagefaults;
EXTERN പൂർണ്ണസംഖ്യ kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
ബാഹ്യ ഘടന boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN അസ്ഥിര പൂർണ്ണസംഖ്യ serial_debug_active;

EXTERN ഘടന cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN പൂർണ്ണസംഖ്യ minix_feature_flags;

#endif /* GLO_H */
