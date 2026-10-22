#اگر_تعریف_نشده KERNEL_H
#تعریف_کن KERNEL_H

/* boot verbose */
#تعریف_کن CONFIG_BOOT_VERBOSE

#اگر_تعریف_نشده CONFIG_MAX_CPUS
#تعریف_کن CONFIG_MAX_CPUS	1
#پایان_اگر

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#تعریف_کن CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#تعریف_کن _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#اگر_تعریف_نشده __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#ضمن <minix/config.h>	/* global configuration, MUST be first */
#ضمن <sys/types.h>		/* general system types */
#ضمن <minix/const.h>	/* MINIX specific constants */
#ضمن <minix/type.h>		/* MINIX specific types, e.g. message */
#ضمن <minix/ipc.h>		/* MINIX run-time system */
#ضمن <minix/sysutil.h>	/* MINIX utility library functions */
#ضمن <minix/timers.h>		/* watchdog timer management */
#ضمن <errno.h>		/* return codes and error numbers */
#ضمن <sys/param.h>
#ضمن <minix/param.h>

/* Important kernel header files. */
#ضمن "kernel/config.h"		/* configuration, MUST be first */
#ضمن "kernel/const.h"		/* constants, MUST be second */
#ضمن "kernel/type.h"		/* type definitions, MUST be third */
#ضمن "kernel/proto.h"		/* function prototypes */
#ضمن "kernel/glo.h"		/* global variables */
#ضمن "kernel/ipc.h"		/* IPC constants */
#ضمن "kernel/profile.h"		/* system profiling */
#ضمن "kernel/proc.h"		/* process table */
#ضمن "kernel/cpulocals.h"		/* CPU-local variables */
#ضمن "kernel/debug.h"		/* debugging, MUST be last kernel header */

#اگر_تعریف_نشده CONFIG_SMP
/* We only support 1 cpu now */
#تعریف_کن CONFIG_MAX_CPUS	1
#تعریف_کن cpuid		0
/* this is always true on an uniprocessor */
#تعریف_کن cpu_is_bsp(x) 1

#وإلا

#ضمن "kernel/smp.h"

#پایان_اگر

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* KERNEL_H */
