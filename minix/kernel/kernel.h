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
#অন্তর্ভাব <minix/config.h>	/* global configuration, MUST be first */
#অন্তর্ভাব <sys/types.h>		/* general system types */
#অন্তর্ভাব <minix/const.h>	/* MINIX specific constants */
#অন্তর্ভাব <minix/type.h>		/* MINIX specific types, e.g. message */
#অন্তর্ভাব <minix/ipc.h>		/* MINIX run-time system */
#অন্তর্ভাব <minix/sysutil.h>	/* MINIX utility library functions */
#অন্তর্ভাব <minix/timers.h>		/* watchdog timer management */
#অন্তর্ভাব <errno.h>		/* return codes and error numbers */
#অন্তর্ভাব <sys/param.h>
#অন্তর্ভাব <minix/param.h>

/* Important kernel header files. */
#অন্তর্ভাব "kernel/config.h"		/* configuration, MUST be first */
#অন্তর্ভাব "kernel/const.h"		/* constants, MUST be second */
#অন্তর্ভাব "kernel/type.h"		/* type definitions, MUST be third */
#অন্তর্ভাব "kernel/proto.h"		/* function prototypes */
#অন্তর্ভাব "kernel/glo.h"		/* global variables */
#অন্তর্ভাব "kernel/ipc.h"		/* IPC constants */
#অন্তর্ভাব "kernel/profile.h"		/* system profiling */
#অন্তর্ভাব "kernel/proc.h"		/* process table */
#অন্তর্ভাব "kernel/cpulocals.h"		/* CPU-local variables */
#অন্তর্ভাব "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#অন্যথা

#অন্তর্ভাব "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
