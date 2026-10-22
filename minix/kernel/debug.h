#నిర్వచితం_కాకపోతే DEBUG_H
#నిర్వచించు DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#నిర్వచితం_కాకపోతే __ASSEMBLY__
#అన్తర్భావ <minix/debug.h>
#అన్తర్భావ "config.h"
#అయితే_ముగింపు

/* Debug info via serial (see ser_debug()) */
#నిర్వచించు DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#నిర్వచించు DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#నిర్వచించు DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#నిర్వచించు DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#నిర్వచించు DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#నిర్వచించు DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#నిర్వచించు DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#నిర్వచించు DEBUG_IPCSTATS			0

#ఒకవేళ !USE_SYSDEBUG
#నిర్వచనం_రద్దు DEBUG_SERIAL
#నిర్వచనం_రద్దు DEBUG_ENABLE_IPC_WARNINGS
#అయితే_ముగింపు

#ఒకవేళ DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#నిర్వచించు DEBUG_IPC_HOOK			1
#అయితే_ముగింపు

#ఒకవేళ DEBUG_TRACE

#నిర్వచించు VF_SCHEDULING		(1L << 1)
#నిర్వచించు VF_PICKPROC		(1L << 2)

#నిర్వచించు TRACE(సంకేతం_2, statement) ఒకవేళ(verboseflags & సంకేతం_2) { రాయి("%s:%d: ", __FILE__, __LINE__); statement }

#లేకపోతే
#నిర్వచించు TRACE(సంకేతం_2, statement)
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_BOOT_VERBOSE
#నిర్వచించు BOOT_VERBOSE(x)	x
#లేకపోతే
#నిర్వచించు BOOT_VERBOSE(x)
#అయితే_ముగింపు

#నిర్వచితమైతే _SYSTEM
#నిర్వచించు DEBUG_PRINT(params, level) చేయి { \
	ఒకవేళ (verboseboot >= (level)) రాయి params; } అయితే (0)
#నిర్వచించు DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#నిర్వచించు DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#నిర్వచించు DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#అయితే_ముగింపు

#అయితే_ముగింపు /* DEBUG_H */
