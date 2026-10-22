#నిర్వచితం_కాకపోతే KERNEL_H
#నిర్వచించు KERNEL_H

/* boot verbose */
#నిర్వచించు CONFIG_BOOT_VERBOSE

#నిర్వచితం_కాకపోతే CONFIG_MAX_CPUS
#నిర్వచించు CONFIG_MAX_CPUS	1
#అయితే_ముగింపు

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#నిర్వచించు CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#నిర్వచించు _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#నిర్వచితం_కాకపోతే __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#అన్తర్భావ <minix/config.h>	/* global configuration, MUST be first */
#అన్తర్భావ <sys/types.h>		/* general system types */
#అన్తర్భావ <minix/const.h>	/* MINIX specific constants */
#అన్తర్భావ <minix/type.h>		/* MINIX specific types, e.g. message */
#అన్తర్భావ <minix/ipc.h>		/* MINIX run-time system */
#అన్తర్భావ <minix/sysutil.h>	/* MINIX utility library functions */
#అన్తర్భావ <minix/timers.h>		/* watchdog timer management */
#అన్తర్భావ <errno.h>		/* return codes and error numbers */
#అన్తర్భావ <sys/param.h>
#అన్తర్భావ <minix/param.h>

/* Important kernel header files. */
#అన్తర్భావ "kernel/config.h"		/* configuration, MUST be first */
#అన్తర్భావ "kernel/const.h"		/* constants, MUST be second */
#అన్తర్భావ "kernel/type.h"		/* type definitions, MUST be third */
#అన్తర్భావ "kernel/proto.h"		/* function prototypes */
#అన్తర్భావ "kernel/glo.h"		/* global variables */
#అన్తర్భావ "kernel/ipc.h"		/* IPC constants */
#అన్తర్భావ "kernel/profile.h"		/* system profiling */
#అన్తర్భావ "kernel/proc.h"		/* process table */
#అన్తర్భావ "kernel/cpulocals.h"		/* CPU-local variables */
#అన్తర్భావ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#నిర్వచితం_కాకపోతే CONFIG_SMP
/* We only support 1 cpu now */
#నిర్వచించు CONFIG_MAX_CPUS	1
#నిర్వచించు cpuid		0
/* this is always true on an uniprocessor */
#నిర్వచించు cpu_is_bsp(x) 1

#లేకపోతే

#అన్తర్భావ "kernel/smp.h"

#అయితే_ముగింపు

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* KERNEL_H */
