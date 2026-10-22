#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ KERNEL_H
#ವ್ಯಾಖ್ಯಾನಿಸು KERNEL_H

/* boot verbose */
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_BOOT_VERBOSE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_MAX_CPUS
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_MAX_CPUS	1
#ಆದರೆ_ಅಂತ್ಯ

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#ಅನ್ತರ್ಭಾವ <minix/config.h>	/* global configuration, MUST be first */
#ಅನ್ತರ್ಭಾವ <sys/types.h>		/* general system types */
#ಅನ್ತರ್ಭಾವ <minix/const.h>	/* MINIX specific constants */
#ಅನ್ತರ್ಭಾವ <minix/type.h>		/* MINIX specific types, e.g. message */
#ಅನ್ತರ್ಭಾವ <minix/ipc.h>		/* MINIX run-time system */
#ಅನ್ತರ್ಭಾವ <minix/sysutil.h>	/* MINIX utility library functions */
#ಅನ್ತರ್ಭಾವ <minix/timers.h>		/* watchdog timer management */
#ಅನ್ತರ್ಭಾವ <errno.h>		/* return codes and error numbers */
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಅನ್ತರ್ಭಾವ <minix/param.h>

/* Important kernel header files. */
#ಅನ್ತರ್ಭಾವ "kernel/config.h"		/* configuration, MUST be first */
#ಅನ್ತರ್ಭಾವ "kernel/const.h"		/* constants, MUST be second */
#ಅನ್ತರ್ಭಾವ "kernel/type.h"		/* type definitions, MUST be third */
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"		/* function prototypes */
#ಅನ್ತರ್ಭಾವ "kernel/glo.h"		/* global variables */
#ಅನ್ತರ್ಭಾವ "kernel/ipc.h"		/* IPC constants */
#ಅನ್ತರ್ಭಾವ "kernel/profile.h"		/* system profiling */
#ಅನ್ತರ್ಭಾವ "kernel/proc.h"		/* process table */
#ಅನ್ತರ್ಭಾವ "kernel/cpulocals.h"		/* CPU-local variables */
#ಅನ್ತರ್ಭಾವ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_SMP
/* We only support 1 cpu now */
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_MAX_CPUS	1
#ವ್ಯಾಖ್ಯಾನಿಸು cpuid		0
/* this is always true on an uniprocessor */
#ವ್ಯಾಖ್ಯಾನಿಸು cpu_is_bsp(x) 1

#ಇಲ್ಲದಿದ್ದರೆ

#ಅನ್ತರ್ಭಾವ "kernel/smp.h"

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* KERNEL_H */
