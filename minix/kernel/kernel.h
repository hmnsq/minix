#ifndef KERNEL_H
#define KERNEL_H

/* boot verbose */
#define CONFIG_BOOT_VERBOSE

#ifndef CONFIG_MAX_CPUS
#define CONFIG_MAX_CPUS	1
#endif

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#define CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#define _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#ifndef __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#شامل <minix/config.h>	/* global configuration, MUST be first */
#شامل <sys/types.h>		/* general system types */
#شامل <minix/const.h>	/* MINIX specific constants */
#شامل <minix/type.h>		/* MINIX specific types, e.g. message */
#شامل <minix/ipc.h>		/* MINIX run-time system */
#شامل <minix/sysutil.h>	/* MINIX utility library functions */
#شامل <minix/timers.h>		/* watchdog timer management */
#شامل <errno.h>		/* return codes and error numbers */
#شامل <sys/param.h>
#شامل <minix/param.h>

/* Important kernel header files. */
#شامل "kernel/config.h"		/* configuration, MUST be first */
#شامل "kernel/const.h"		/* constants, MUST be second */
#شامل "kernel/type.h"		/* type definitions, MUST be third */
#شامل "kernel/proto.h"		/* function prototypes */
#شامل "kernel/glo.h"		/* global variables */
#شامل "kernel/ipc.h"		/* IPC constants */
#شامل "kernel/profile.h"		/* system profiling */
#شامل "kernel/proc.h"		/* process table */
#شامل "kernel/cpulocals.h"		/* CPU-local variables */
#شامل "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#وگرنه

#شامل "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
