#నిర్వచితం_కాకపోతే GLO_H
#నిర్వచించు GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#నిర్వచితమైతే పట్టిక_2
#నిర్వచనం_రద్దు EXTERN
#నిర్వచించు EXTERN
#అయితే_ముగింపు

#అన్తర్భావ <minix/config.h>
#అన్తర్భావ <minix/ipcconst.h>
#అన్తర్భావ <machine/archtypes.h>
#అన్తర్భావ "archconst.h"
#అన్తర్భావ "config.h"
#అన్తర్భావ "debug.h"

/* Kernel information structures. This groups vital kernel information. */
బాహ్య నిర్మాణం kinfo kinfo;		  /* kernel information for services */
బాహ్య నిర్మాణం machine machine;		  /* machine info for services */
బాహ్య నిర్మాణం kmessages kmessages;  	  /* diagnostic messages in kernel */
బాహ్య నిర్మాణం loadinfo loadinfo;	  /* status of load average */
బాహ్య నిర్మాణం kuserinfo kuserinfo;	  /* kernel information for users */
బాహ్య నిర్మాణం arm_frclock arm_frclock;	  /* ARM free-running timer info */
బాహ్య నిర్మాణం kclockinfo kclockinfo;	  /* clock information */
బాహ్య నిర్మాణం minix_kerninfo minix_kerninfo;

EXTERN నిర్మాణం k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_బైట్లు minix_kerninfo_user;

#నిర్వచించు kmess kmessages
#నిర్వచించు kloadinfo loadinfo

#నిర్వచించు system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN నిర్మాణం proc *vmrequest;  /* first process on vmrequest queue */
EXTERN గుర్తులేని కోల్పోయిన_ticks;	/* clock ticks counted outside clock task */
EXTERN అక్షరం *ipc_call_పేర్లు[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN నిర్మాణం proc *kbill_kcall; /* process that made kernel call */
EXTERN నిర్మాణం proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN పూర్ణాంకం irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN పూర్ణాంకం irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN పూర్ణాంకం verboseboot;			/* verbose boot, init'ed in cstart */

#ఒకవేళ DEBUG_TRACE
EXTERN పూర్ణాంకం verboseflags;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_APIC
EXTERN పూర్ణాంకం config_no_apic; /* optionally turn off apic */
EXTERN పూర్ణాంకం config_apic_timer_x; /* apic timer slowdown factor */
#అయితే_ముగింపు

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#నిర్వచించు cpu_సమితి_freq(cpu, freq)	చేయి {cpu_hz[cpu] = freq;} అయితే (0)
#నిర్వచించు cpu_get_freq(cpu)	cpu_hz[cpu]

#నిర్వచితమైతే CONFIG_SMP
EXTERN పూర్ణాంకం config_no_smp; /* optionally turn off SMP */
#అయితే_ముగింపు

/* VM */
EXTERN పూర్ణాంకం vm_running;
EXTERN పూర్ణాంకం catch_pagefaults;
EXTERN పూర్ణాంకం kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
బాహ్య నిర్మాణం boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN అస్థిర పూర్ణాంకం serial_debug_చురుకైన;

EXTERN నిర్మాణం cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN గుర్తులేని bkl_tries[CONFIG_MAX_CPUS];
EXTERN గుర్తులేని bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN పూర్ణాంకం minix_feature_flags;

#అయితే_ముగింపు /* GLO_H */
