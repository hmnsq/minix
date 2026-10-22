#अगर_अघोषित KERNEL_H
#घोषणा KERNEL_H

/* boot verbose */
#घोषणा CONFIG_BOOT_VERBOSE

#अगर_अघोषित CONFIG_MAX_CPUS
#घोषणा CONFIG_MAX_CPUS	1
#पूर्ण_अगर

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#घोषणा CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#घोषणा _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#अगर_अघोषित __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#समावेश <minix/config.h>	/* global configuration, MUST be first */
#समावेश <sys/types.h>		/* general system types */
#समावेश <minix/const.h>	/* MINIX specific constants */
#समावेश <minix/type.h>		/* MINIX specific types, e.g. message */
#समावेश <minix/ipc.h>		/* MINIX run-time system */
#समावेश <minix/sysutil.h>	/* MINIX utility library functions */
#समावेश <minix/timers.h>		/* watchdog timer management */
#समावेश <errno.h>		/* return codes and error numbers */
#समावेश <sys/param.h>
#समावेश <minix/param.h>

/* Important kernel header files. */
#समावेश "kernel/config.h"		/* configuration, MUST be first */
#समावेश "kernel/const.h"		/* constants, MUST be second */
#समावेश "kernel/type.h"		/* type definitions, MUST be third */
#समावेश "kernel/proto.h"		/* function prototypes */
#समावेश "kernel/glo.h"		/* global variables */
#समावेश "kernel/ipc.h"		/* IPC constants */
#समावेश "kernel/profile.h"		/* system profiling */
#समावेश "kernel/proc.h"		/* process table */
#समावेश "kernel/cpulocals.h"		/* CPU-local variables */
#समावेश "kernel/debug.h"		/* debugging, MUST be last kernel header */

#अगर_अघोषित CONFIG_SMP
/* We only support 1 cpu now */
#घोषणा CONFIG_MAX_CPUS	1
#घोषणा cpuid		0
/* this is always true on an uniprocessor */
#घोषणा cpu_is_bsp(x) 1

#अन्यथा

#समावेश "kernel/smp.h"

#पूर्ण_अगर

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* KERNEL_H */
