#اگر_غیر_تعریف_شدہ KERNEL_H
#تعریف_کرو KERNEL_H

/* boot verbose */
#تعریف_کرو CONFIG_BOOT_VERBOSE

#اگر_غیر_تعریف_شدہ CONFIG_MAX_CPUS
#تعریف_کرو CONFIG_MAX_CPUS	1
#اگر_ختم

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#تعریف_کرو CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#تعریف_کرو _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#اگر_غیر_تعریف_شدہ __ASSEMBLY__

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

#اگر_غیر_تعریف_شدہ CONFIG_SMP
/* We only support 1 cpu now */
#تعریف_کرو CONFIG_MAX_CPUS	1
#تعریف_کرو cpuid		0
/* this is always true on an uniprocessor */
#تعریف_کرو cpu_is_bsp(x) 1

#ورنہ

#شامل "kernel/smp.h"

#اگر_ختم

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* KERNEL_H */
