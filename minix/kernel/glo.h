#اگر_غیر_تعریف_شدہ GLO_H
#تعریف_کرو GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#اگر_تعریف_شدہ جدول_2
#تعریف_منسوخ EXTERN
#تعریف_کرو EXTERN
#اگر_ختم

#شامل <minix/config.h>
#شامل <minix/ipcconst.h>
#شامل <machine/archtypes.h>
#شامل "archconst.h"
#شامل "config.h"
#شامل "debug.h"

/* Kernel information structures. This groups vital kernel information. */
خارجی ساخت kinfo kinfo;		  /* kernel information for services */
خارجی ساخت machine machine;		  /* machine info for services */
خارجی ساخت kmessages kmessages;  	  /* diagnostic messages in kernel */
خارجی ساخت loadinfo loadinfo;	  /* status of load average */
خارجی ساخت kuserinfo kuserinfo;	  /* kernel information for users */
خارجی ساخت arm_frclock arm_frclock;	  /* ARM free-running timer info */
خارجی ساخت kclockinfo kclockinfo;	  /* clock information */
خارجی ساخت minix_kerninfo minix_kerninfo;

EXTERN ساخت k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_بائٹیں minix_kerninfo_user;

#تعریف_کرو kmess kmessages
#تعریف_کرو kloadinfo loadinfo

#تعریف_کرو system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN ساخت proc *vmrequest;  /* first process on vmrequest queue */
EXTERN غیرعلامتی گمشدہ_ticks;	/* clock ticks counted outside clock task */
EXTERN حرف *ipc_call_نام[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN ساخت proc *kbill_kcall; /* process that made kernel call */
EXTERN ساخت proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN صحیح irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN صحیح irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN صحیح verboseboot;			/* verbose boot, init'ed in cstart */

#اگر DEBUG_TRACE
EXTERN صحیح verboseflags;
#اگر_ختم

#اگر_تعریف_شدہ USE_APIC
EXTERN صحیح config_no_apic; /* optionally turn off apic */
EXTERN صحیح config_apic_timer_x; /* apic timer slowdown factor */
#اگر_ختم

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#تعریف_کرو cpu_سیٹ_freq(cpu, freq)	کرو {cpu_hz[cpu] = freq;} جب (0)
#تعریف_کرو cpu_get_freq(cpu)	cpu_hz[cpu]

#اگر_تعریف_شدہ CONFIG_SMP
EXTERN صحیح config_no_smp; /* optionally turn off SMP */
#اگر_ختم

/* VM */
EXTERN صحیح vm_running;
EXTERN صحیح catch_pagefaults;
EXTERN صحیح kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
خارجی ساخت boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN متلون صحیح serial_debug_فعال;

EXTERN ساخت cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN غیرعلامتی bkl_tries[CONFIG_MAX_CPUS];
EXTERN غیرعلامتی bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN صحیح minix_feature_flags;

#اگر_ختم /* GLO_H */
