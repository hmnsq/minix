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

#અન્તર્ભાવ <minix/config.h>
#અન્તર્ભાવ <minix/ipcconst.h>
#અન્તર્ભાવ <machine/archtypes.h>
#અન્તર્ભાવ "archconst.h"
#અન્તર્ભાવ "config.h"
#અન્તર્ભાવ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
બાહ્ય સંરચના kinfo kinfo;		  /* kernel information for services */
બાહ્ય સંરચના machine machine;		  /* machine info for services */
બાહ્ય સંરચના kmessages kmessages;  	  /* diagnostic messages in kernel */
બાહ્ય સંરચના loadinfo loadinfo;	  /* status of load average */
બાહ્ય સંરચના kuserinfo kuserinfo;	  /* kernel information for users */
બાહ્ય સંરચના arm_frclock arm_frclock;	  /* ARM free-running timer info */
બાહ્ય સંરચના kclockinfo kclockinfo;	  /* clock information */
બાહ્ય સંરચના minix_kerninfo minix_kerninfo;

EXTERN સંરચના k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#define kmess kmessages
#define kloadinfo loadinfo

#define system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN સંરચના proc *vmrequest;  /* first process on vmrequest queue */
EXTERN unsigned lost_ticks;	/* clock ticks counted outside clock task */
EXTERN અક્ષર *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN સંરચના proc *kbill_kcall; /* process that made kernel call */
EXTERN સંરચના proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN પૂર્ણાંક irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN પૂર્ણાંક irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN પૂર્ણાંક verboseboot;			/* verbose boot, init'ed in cstart */

#જો DEBUG_TRACE
EXTERN પૂર્ણાંક verboseflags;
#endif

#ifdef USE_APIC
EXTERN પૂર્ણાંક config_no_apic; /* optionally turn off apic */
EXTERN પૂર્ણાંક config_apic_timer_x; /* apic timer slowdown factor */
#endif

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#define cpu_set_freq(cpu, freq)	કરો {cpu_hz[cpu] = freq;} જ્યારે (0)
#define cpu_get_freq(cpu)	cpu_hz[cpu]

#ifdef CONFIG_SMP
EXTERN પૂર્ણાંક config_no_smp; /* optionally turn off SMP */
#endif

/* VM */
EXTERN પૂર્ણાંક vm_running;
EXTERN પૂર્ણાંક catch_pagefaults;
EXTERN પૂર્ણાંક kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
બાહ્ય સંરચના boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN અસ્થિર પૂર્ણાંક serial_debug_active;

EXTERN સંરચના cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_tries[CONFIG_MAX_CPUS];
EXTERN unsigned bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN પૂર્ણાંક minix_feature_flags;

#endif /* GLO_H */
