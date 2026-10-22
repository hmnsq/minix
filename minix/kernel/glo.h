#अगर_अघोषित GLO_H
#घोषणा GLO_H

/* Global variables used in the kernel. This file contains the declarations;
 * storage space for the variables is allocated in table.c, because EXTERN is
 * defined as extern unless the _TABLE definition is seen. We rely on the 
 * compiler's default initialization (0) for several global variables. 
 */
#अगर_घोषित _TABLE
#अघोषित EXTERN
#घोषणा EXTERN
#पूर्ण_अगर

#समावेश <minix/config.h>
#समावेश <minix/ipcconst.h>
#समावेश <machine/archtypes.h>
#समावेश "archconst.h"
#समावेश "config.h"
#समावेश "debug.h"

/* Kernel information structures. This groups vital kernel information. */
बाह्य काष्ठा kinfo kinfo;		  /* kernel information for services */
बाह्य काष्ठा machine machine;		  /* machine info for services */
बाह्य काष्ठा kmessages kmessages;  	  /* diagnostic messages in kernel */
बाह्य काष्ठा loadinfo loadinfo;	  /* status of load average */
बाह्य काष्ठा kuserinfo kuserinfo;	  /* kernel information for users */
बाह्य काष्ठा arm_frclock arm_frclock;	  /* ARM free-running timer info */
बाह्य काष्ठा kclockinfo kclockinfo;	  /* clock information */
बाह्य काष्ठा minix_kerninfo minix_kerninfo;

EXTERN काष्ठा k_randomness krandom; 	/* gather kernel random information */

EXTERN vir_bytes minix_kerninfo_user;

#घोषणा kmess kmessages
#घोषणा kloadinfo loadinfo

#घोषणा system_hz (kclockinfo.hz)		/* HZ value (alias) */

/* Process scheduling information and the kernel reentry count. */
EXTERN काष्ठा proc *vmrequest;  /* first process on vmrequest queue */
EXTERN अचिन्हित lost_ticks;	/* clock ticks counted outside clock task */
EXTERN अक्षर *ipc_call_names[IPCNO_HIGHEST+1]; /* human-readable call names */
EXTERN काष्ठा proc *kbill_kcall; /* process that made kernel call */
EXTERN काष्ठा proc *kbill_ipc; /* process that invoked ipc */

/* Interrupt related variables. */
EXTERN irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
EXTERN पूर्णांक irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
EXTERN पूर्णांक irq_use;				/* map of all in-use irq's */

/* Miscellaneous. */
EXTERN पूर्णांक verboseboot;			/* verbose boot, init'ed in cstart */

#अगर DEBUG_TRACE
EXTERN पूर्णांक verboseflags;
#पूर्ण_अगर

#अगर_घोषित USE_APIC
EXTERN पूर्णांक config_no_apic; /* optionally turn off apic */
EXTERN पूर्णांक config_apic_timer_x; /* apic timer slowdown factor */
#पूर्ण_अगर

EXTERN u64_t cpu_hz[CONFIG_MAX_CPUS];

#घोषणा cpu_set_freq(cpu, freq)	करो {cpu_hz[cpu] = freq;} जबतक (0)
#घोषणा cpu_get_freq(cpu)	cpu_hz[cpu]

#अगर_घोषित CONFIG_SMP
EXTERN पूर्णांक config_no_smp; /* optionally turn off SMP */
#पूर्ण_अगर

/* VM */
EXTERN पूर्णांक vm_running;
EXTERN पूर्णांक catch_pagefaults;
EXTERN पूर्णांक kernel_may_alloc;

/* Variables that are initialized elsewhere are just extern here. */
बाह्य काष्ठा boot_image image[NR_BOOT_PROCS]; 	/* system image processes */

EXTERN अस्थिर पूर्णांक serial_debug_active;

EXTERN काष्ठा cpu_info cpu_info[CONFIG_MAX_CPUS];

/* BKL stats */
EXTERN u64_t kernel_ticks[CONFIG_MAX_CPUS];
EXTERN u64_t bkl_ticks[CONFIG_MAX_CPUS];
EXTERN अचिन्हित bkl_tries[CONFIG_MAX_CPUS];
EXTERN अचिन्हित bkl_succ[CONFIG_MAX_CPUS];

/* Feature flags */
EXTERN पूर्णांक minix_feature_flags;

#पूर्ण_अगर /* GLO_H */
