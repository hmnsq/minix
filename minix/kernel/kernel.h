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
#അന്തര്ഭാവ <minix/config.h>	/* global configuration, MUST be first */
#അന്തര്ഭാവ <sys/types.h>		/* general system types */
#അന്തര്ഭാവ <minix/const.h>	/* MINIX specific constants */
#അന്തര്ഭാവ <minix/type.h>		/* MINIX specific types, e.g. message */
#അന്തര്ഭാവ <minix/ipc.h>		/* MINIX run-time system */
#അന്തര്ഭാവ <minix/sysutil.h>	/* MINIX utility library functions */
#അന്തര്ഭാവ <minix/timers.h>		/* watchdog timer management */
#അന്തര്ഭാവ <errno.h>		/* return codes and error numbers */
#അന്തര്ഭാവ <sys/param.h>
#അന്തര്ഭാവ <minix/param.h>

/* Important kernel header files. */
#അന്തര്ഭാവ "kernel/config.h"		/* configuration, MUST be first */
#അന്തര്ഭാവ "kernel/const.h"		/* constants, MUST be second */
#അന്തര്ഭാവ "kernel/type.h"		/* type definitions, MUST be third */
#അന്തര്ഭാവ "kernel/proto.h"		/* function prototypes */
#അന്തര്ഭാവ "kernel/glo.h"		/* global variables */
#അന്തര്ഭാവ "kernel/ipc.h"		/* IPC constants */
#അന്തര്ഭാവ "kernel/profile.h"		/* system profiling */
#അന്തര്ഭാവ "kernel/proc.h"		/* process table */
#അന്തര്ഭാവ "kernel/cpulocals.h"		/* CPU-local variables */
#അന്തര്ഭാവ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#അല്ലെങ്കിൽ

#അന്തര്ഭാവ "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
