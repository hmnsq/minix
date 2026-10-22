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

#include <minix/config.h>
#include <minix/ipcconst.h>
#include <machine/archtypes.h>
#include "archconst.h"
#include "config.h"
#include "debug.h"

/* Kernel information structures. This groups vital kernel information. */
extern sa_nracanaa kinfo kinfo;		  /* kernel information for services */
extern sa_nracanaa machine machine;		  /* machine info for services */
extern sa_nracanaa kmessages kmessages;  	  /* diagnostic messages in kernel */
extern sa_nracanaa loadinfo loadinfo;	  /* status of load average */
extern sa_nracanaa kuserinfo kuserinfo;	  /* kernel information for users */
extern sa_nracanaa arm_frclock arm_frclock;	  /* ARM free-running timer info */
extern sa_nracanaa kclockinfo kclockinfo;	  /* clock information */
extern sa_nracanaa minix_kerninfo minix_kerninfo;

EXTERN sa_nracanaa k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN sa_nracanaa proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN var_na *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN sa_nracanaa proc *kbill_kcall; /* process that made kernel call */
EXTERN sa_nracanaa proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN จำนวนเต็ม irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN จำนวนเต็ม irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN จำนวนเต็ม verboseboot;			/* verbose boot, init'ed in cstart */

#ถ้า DEBUG_TRACE
EXTERN จำนวนเต็ม verboseflags;
#endif

#ifdef USE_APIC
EXTERN จำนวนเต็ม config_no_apic; /* optionally turn off apic */
EXTERN จำนวนเต็ม config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	do {cpu_hz[cpu] = freq;} ขณะ (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN จำนวนเต็ม config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN จำนวนเต็ม vm_running;
EXTERN จำนวนเต็ม catch_pagefaults;
EXTERN จำนวนเต็ม kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
extern sa_nracanaa boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN ca_ncala จำนวนเต็ม serial_debug_active;

EXTERN sa_nracanaa cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN จำนวนเต็ม minix_feature_flags;

#endif /* GLO_H */
