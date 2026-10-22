#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ KERNEL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERNEL_H

/* boot verbose */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_BOOT_VERBOSE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_MAX_CPUS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_MAX_CPUS	1
#ਜੇ_ਅੰਤ

/* OXPCIe952 PCIe with 2 UARTs in-kernel support */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_OXPCIE	0

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SYSTEM            1	/* tell headers that this is the kernel */

/*
 * we need the defines above in assembly files to configure the kernel
 * correctly. However we don't need the rest
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

/* The following are so basic, all the *.c files get them automatically. */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/config.h>	/* global configuration, MUST be first */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>		/* general system types */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/const.h>	/* MINIX specific constants */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>		/* MINIX specific types, e.g. message */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/ipc.h>		/* MINIX run-time system */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/sysutil.h>	/* MINIX utility library functions */
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/timers.h>		/* watchdog timer management */
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>		/* return codes and error numbers */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/param.h>

/* Important kernel header files. */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/config.h"		/* configuration, MUST be first */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/const.h"		/* constants, MUST be second */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/type.h"		/* type definitions, MUST be third */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"		/* function prototypes */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/glo.h"		/* global variables */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ipc.h"		/* IPC constants */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/profile.h"		/* system profiling */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proc.h"		/* process table */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/cpulocals.h"		/* CPU-local variables */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/debug.h"		/* debugging, MUST be last kernel header */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
/* We only support 1 cpu now */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_MAX_CPUS	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpuid		0
/* this is always true on an uniprocessor */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_is_bsp(x) 1

#ਨਹੀਂਤਾਂ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/smp.h"

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* KERNEL_H */
