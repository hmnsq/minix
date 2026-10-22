#جيڪڏهن_وصف_نه_ٿيل GLO_H
#وصف_ڪريو GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#جيڪڏهن_وصف_ٿيل جدول_2
#وصف_رد EXTERN
#وصف_ڪريو EXTERN
#جيڪڏهن_پورو

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

EXTERN vir_بائيٽون minix_kerninfo_user;

#وصف_ڪريو kmess kmessages
#وصف_ڪريو kloadinfo loadinfo

#وصف_ڪريو system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN بنية proc *vmrequest;  /* first process on vmrequest queue */
EXTERN بنانشان گم_ticks;	/* clock ticks counted outside clock task */
EXTERN حرف *ipc_call_نالا[IPCNO_HIGHEST+1]; /* human-readable call names */
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_APIC
EXTERN صحيح config_no_apic; /* optionally turn off apic */
EXTERN صحيح config_apic_timer_x; /* apic timer slowdown factor */
#جيڪڏهن_پورو

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#وصف_ڪريو cpu_سيٽ_freq(cpu, freq)	افعل {cpu_hz[cpu] = freq;} طالما (0)
#وصف_ڪريو cpu_get_freq(cpu)	cpu_hz[cpu]

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
EXTERN صحيح config_no_smp; /* optionally turn off SMP */
#جيڪڏهن_پورو

/* VM */
EXTERN صحيح vm_running;
EXTERN صحيح catch_pagefaults;
EXTERN صحيح kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
خارجي بنية boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN متطاير صحيح serial_debug_سرگرم;

EXTERN بنية cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN بنانشان bkl_tries[CONFIG_MAX_CPUS];
EXTERN بنانشان bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN صحيح minix_feature_flags;

#جيڪڏهن_پورو /* GLO_H */
