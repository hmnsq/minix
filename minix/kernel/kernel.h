#که_نه_تعریف_شوی KERNEL_H
#تعریف_کړه KERNEL_H

/* boot verbose */
#تعریف_کړه CONFIG_BOOT_VERBOSE

#که_نه_تعریف_شوی CONFIG_MAX_CPUS
#تعریف_کړه CONFIG_MAX_CPUS	1
#د_که_پای

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#تعریف_کړه CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#تعریف_کړه _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#که_نه_تعریف_شوی __ASSEMBLY__

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

#که_نه_تعریف_شوی CONFIG_SMP
/* We only support 1 cpu now */
#تعریف_کړه CONFIG_MAX_CPUS	1
#تعریف_کړه cpuid		0
/* this is always true on an uniprocessor */
#تعریف_کړه cpu_is_bsp(x) 1

#وإلا

#ضمن "kernel/smp.h"

#د_که_پای

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای /* KERNEL_H */
