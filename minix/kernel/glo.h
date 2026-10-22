#اگر_تعریف_نشده GLO_H
#تعریف_کن GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#اگر_تعریف_شده جدول_2
#لغو_تعریف EXTERN
#تعریف_کن EXTERN
#پایان_اگر

#ضمن <minix/config.h>
#ضمن <minix/ipcconst.h>
#ضمن <machine/archtypes.h>
#ضمن "archconst.h"
#ضمن "config.h"
#ضمن "debug.h"

/* Kernel information structures. This groups vital kernel information. */
خارجي بنية kinfo kinfo;		  /* kernel information for services */
خارجي بنية machine machine;		  /* machine info for services */
خارجي بنية kmessages kmessages;  	  /* diagnostic messages in kernel */
خارجي بنية loadinfo loadinfo;	  /* status of load average */
خارجي بنية kuserinfo kuserinfo;	  /* kernel information for users */
خارجي بنية arm_frclock arm_frclock;	  /* ARM free-running timer info */
خارجي بنية kclockinfo kclockinfo;	  /* clock information */
خارجي بنية minix_kerninfo minix_kerninfo;

EXTERN بنية k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_بایتها minix_kerninfo_user;

#تعریف_کن kmess kmessages
#تعریف_کن kloadinfo loadinfo

#تعریف_کن system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN بنية proc *vmrequest;  /* first process on vmrequest queue */
EXTERN بیعلامت گمشده_ticks;	/* clock ticks counted outside clock task */
EXTERN حرف *ipc_call_نامها[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN بنية proc *kbill_kcall; /* process that made kernel call */
EXTERN بنية proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN صحيح irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN صحيح irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN صحيح verboseboot;			/* verbose boot, init'ed in cstart */

#إذا DEBUG_TRACE
EXTERN صحيح verboseflags;
#پایان_اگر

#اگر_تعریف_شده USE_APIC
EXTERN صحيح config_no_apic; /* optionally turn off apic */
EXTERN صحيح config_apic_timer_x; /* apic timer slowdown factor */
#پایان_اگر

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#تعریف_کن cpu_مجموعه_freq(cpu, freq)	افعل {cpu_hz[cpu] = freq;} طالما (0)
#تعریف_کن cpu_get_freq(cpu)	cpu_hz[cpu]

#اگر_تعریف_شده CONFIG_SMP
EXTERN صحيح config_no_smp; /* optionally turn off SMP */
#پایان_اگر

/* VM */
EXTERN صحيح vm_running;
EXTERN صحيح catch_pagefaults;
EXTERN صحيح kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
خارجي بنية boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN متطاير صحيح serial_debug_فعال;

EXTERN بنية cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN بیعلامت bkl_tries[CONFIG_MAX_CPUS];
EXTERN بیعلامت bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN صحيح minix_feature_flags;

#پایان_اگر /* GLO_H */
