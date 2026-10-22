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
#અન્તર્ભાવ <minix/config.h>	/* global configuration, MUST be first */
#અન્તર્ભાવ <sys/types.h>		/* general system types */
#અન્તર્ભાવ <minix/const.h>	/* MINIX specific constants */
#અન્તર્ભાવ <minix/type.h>		/* MINIX specific types, e.g. message */
#અન્તર્ભાવ <minix/ipc.h>		/* MINIX run-time system */
#અન્તર્ભાવ <minix/sysutil.h>	/* MINIX utility library functions */
#અન્તર્ભાવ <minix/timers.h>		/* watchdog timer management */
#અન્તર્ભાવ <errno.h>		/* return codes and error numbers */
#અન્તર્ભાવ <sys/param.h>
#અન્તર્ભાવ <minix/param.h>

/* Important kernel header files. */
#અન્તર્ભાવ "kernel/config.h"		/* configuration, MUST be first */
#અન્તર્ભાવ "kernel/const.h"		/* constants, MUST be second */
#અન્તર્ભાવ "kernel/type.h"		/* type definitions, MUST be third */
#અન્તર્ભાવ "kernel/proto.h"		/* function prototypes */
#અન્તર્ભાવ "kernel/glo.h"		/* global variables */
#અન્તર્ભાવ "kernel/ipc.h"		/* IPC constants */
#અન્તર્ભાવ "kernel/profile.h"		/* system profiling */
#અન્તર્ભાવ "kernel/proc.h"		/* process table */
#અન્તર્ભાવ "kernel/cpulocals.h"		/* CPU-local variables */
#અન્તર્ભાવ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#નહીંતર

#અન્તર્ભાવ "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
