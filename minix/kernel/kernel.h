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
#ଅନ୍ତର୍ଭାଵ <minix/config.h>	/* global configuration, MUST be first */
#ଅନ୍ତର୍ଭାଵ <sys/types.h>		/* general system types */
#ଅନ୍ତର୍ଭାଵ <minix/const.h>	/* MINIX specific constants */
#ଅନ୍ତର୍ଭାଵ <minix/type.h>		/* MINIX specific types, e.g. message */
#ଅନ୍ତର୍ଭାଵ <minix/ipc.h>		/* MINIX run-time system */
#ଅନ୍ତର୍ଭାଵ <minix/sysutil.h>	/* MINIX utility library functions */
#ଅନ୍ତର୍ଭାଵ <minix/timers.h>		/* watchdog timer management */
#ଅନ୍ତର୍ଭାଵ <errno.h>		/* return codes and error numbers */
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#ଅନ୍ତର୍ଭାଵ <minix/param.h>

/* Important kernel header files. */
#ଅନ୍ତର୍ଭାଵ "kernel/config.h"		/* configuration, MUST be first */
#ଅନ୍ତର୍ଭାଵ "kernel/const.h"		/* constants, MUST be second */
#ଅନ୍ତର୍ଭାଵ "kernel/type.h"		/* type definitions, MUST be third */
#ଅନ୍ତର୍ଭାଵ "kernel/proto.h"		/* function prototypes */
#ଅନ୍ତର୍ଭାଵ "kernel/glo.h"		/* global variables */
#ଅନ୍ତର୍ଭାଵ "kernel/ipc.h"		/* IPC constants */
#ଅନ୍ତର୍ଭାଵ "kernel/profile.h"		/* system profiling */
#ଅନ୍ତର୍ଭାଵ "kernel/proc.h"		/* process table */
#ଅନ୍ତର୍ଭାଵ "kernel/cpulocals.h"		/* CPU-local variables */
#ଅନ୍ତର୍ଭାଵ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#ଅନ୍ୟଥା

#ଅନ୍ତର୍ଭାଵ "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
