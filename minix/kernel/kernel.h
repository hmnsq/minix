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
#அந்தர்பாவ <minix/config.h>	/* global configuration, MUST be first */
#அந்தர்பாவ <sys/types.h>		/* general system types */
#அந்தர்பாவ <minix/const.h>	/* MINIX specific constants */
#அந்தர்பாவ <minix/type.h>		/* MINIX specific types, e.g. message */
#அந்தர்பாவ <minix/ipc.h>		/* MINIX run-time system */
#அந்தர்பாவ <minix/sysutil.h>	/* MINIX utility library functions */
#அந்தர்பாவ <minix/timers.h>		/* watchdog timer management */
#அந்தர்பாவ <errno.h>		/* return codes and error numbers */
#அந்தர்பாவ <sys/param.h>
#அந்தர்பாவ <minix/param.h>

/* Important kernel header files. */
#அந்தர்பாவ "kernel/config.h"		/* configuration, MUST be first */
#அந்தர்பாவ "kernel/const.h"		/* constants, MUST be second */
#அந்தர்பாவ "kernel/type.h"		/* type definitions, MUST be third */
#அந்தர்பாவ "kernel/proto.h"		/* function prototypes */
#அந்தர்பாவ "kernel/glo.h"		/* global variables */
#அந்தர்பாவ "kernel/ipc.h"		/* IPC constants */
#அந்தர்பாவ "kernel/profile.h"		/* system profiling */
#அந்தர்பாவ "kernel/proc.h"		/* process table */
#அந்தர்பாவ "kernel/cpulocals.h"		/* CPU-local variables */
#அந்தர்பாவ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#இல்லையெனில்

#அந்தர்பாவ "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
