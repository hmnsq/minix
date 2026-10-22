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
#अन्तर्भाव <minix/config.h>	/* global configuration, MUST be first */
#अन्तर्भाव <sys/types.h>		/* general system types */
#अन्तर्भाव <minix/const.h>	/* MINIX specific constants */
#अन्तर्भाव <minix/type.h>		/* MINIX specific types, e.g. message */
#अन्तर्भाव <minix/ipc.h>		/* MINIX run-time system */
#अन्तर्भाव <minix/sysutil.h>	/* MINIX utility library functions */
#अन्तर्भाव <minix/timers.h>		/* watchdog timer management */
#अन्तर्भाव <errno.h>		/* return codes and error numbers */
#अन्तर्भाव <sys/param.h>
#अन्तर्भाव <minix/param.h>

/* Important kernel header files. */
#अन्तर्भाव "kernel/config.h"		/* configuration, MUST be first */
#अन्तर्भाव "kernel/const.h"		/* constants, MUST be second */
#अन्तर्भाव "kernel/type.h"		/* type definitions, MUST be third */
#अन्तर्भाव "kernel/proto.h"		/* function prototypes */
#अन्तर्भाव "kernel/glo.h"		/* global variables */
#अन्तर्भाव "kernel/ipc.h"		/* IPC constants */
#अन्तर्भाव "kernel/profile.h"		/* system profiling */
#अन्तर्भाव "kernel/proc.h"		/* process table */
#अन्तर्भाव "kernel/cpulocals.h"		/* CPU-local variables */
#अन्तर्भाव "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ifndef CONFIG_SMP
/* We only support 1 cpu now */
#define CONFIG_MAX_CPUS	1
#define cpuid		0
/* this is always true on an uniprocessor */
#define cpu_is_bsp(x) 1

#अञ्ञथा

#अन्तर्भाव "kernel/smp.h"

#endif

#endif /* __ASSEMBLY__ */

#endif /* KERNEL_H */
