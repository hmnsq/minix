#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GLO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸਾਰਣੀ_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ EXTERN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTERN
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/ipcconst.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/archtypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "archconst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
ਬਾਹਰੀ ਬਣਤਰ kinfo kinfo;		  /* kernel information for services */
ਬਾਹਰੀ ਬਣਤਰ machine machine;		  /* machine info for services */
ਬਾਹਰੀ ਬਣਤਰ kmessages kmessages;  	  /* diagnostic messages in kernel */
ਬਾਹਰੀ ਬਣਤਰ loadinfo loadinfo;	  /* status of load average */
ਬਾਹਰੀ ਬਣਤਰ kuserinfo kuserinfo;	  /* kernel information for users */
ਬਾਹਰੀ ਬਣਤਰ arm_frclock arm_frclock;	  /* ARM free-running timer info */
ਬਾਹਰੀ ਬਣਤਰ kclockinfo kclockinfo;	  /* clock information */
ਬਾਹਰੀ ਬਣਤਰ minix_kerninfo minix_kerninfo;

EXTERN ਬਣਤਰ k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_ਬਾਈਟਾਂ minix_kerninfo_user;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ kmess kmessages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ kloadinfo loadinfo

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN ਬਣਤਰ proc *vmrequest;  /* first process on vmrequest queue */
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ ਗੁਆਚਾ_ticks;	/* clock ticks counted outside clock task */
EXTERN ਅੱਖਰ *ipc_call_ਨਾਂ[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN ਬਣਤਰ proc *kbill_kcall; /* process that made kernel call */
EXTERN ਬਣਤਰ proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN ਪੂਰਨ irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN ਪੂਰਨ irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN ਪੂਰਨ verboseboot;			/* verbose boot, init'ed in cstart */

#ਜੇ DEBUG_TRACE
EXTERN ਪੂਰਨ verboseflags;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
EXTERN ਪੂਰਨ config_no_apic; /* optionally turn off apic */
EXTERN ਪੂਰਨ config_apic_timer_x; /* apic timer slowdown factor */
#ਜੇ_ਅੰਤ

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_ਸੈੱਟ_freq(cpu, freq)	ਕਰੋ {cpu_hz[cpu] = freq;} ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_get_freq(cpu)	cpu_hz[cpu]

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
EXTERN ਪੂਰਨ config_no_smp; /* optionally turn off SMP */
#ਜੇ_ਅੰਤ

/* VM */
EXTERN ਪੂਰਨ vm_running;
EXTERN ਪੂਰਨ catch_pagefaults;
EXTERN ਪੂਰਨ kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
ਬਾਹਰੀ ਬਣਤਰ boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN ਅਸਥਿਰ ਪੂਰਨ serial_debug_ਸਰਗਰਮ;

EXTERN ਬਣਤਰ cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ bkl_tries[CONFIG_MAX_CPUS];
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN ਪੂਰਨ minix_feature_flags;

#ਜੇ_ਅੰਤ /* GLO_H */
