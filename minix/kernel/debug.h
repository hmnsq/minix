#اگر_تعریف_نشده DEBUG_H
#تعریف_کن DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#اگر_تعریف_نشده __ASSEMBLY__
#ضمن <minix/debug.h>
#ضمن "config.h"
#پایان_اگر

/* Debug info via serial (see ser_debug()) */
#تعریف_کن DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#تعریف_کن DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#تعریف_کن DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#تعریف_کن DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#تعریف_کن DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#تعریف_کن DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#تعریف_کن DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#تعریف_کن DEBUG_IPCSTATS			0

#إذا !USE_SYSDEBUG
#لغو_تعریف DEBUG_SERIAL
#لغو_تعریف DEBUG_ENABLE_IPC_WARNINGS
#پایان_اگر

#إذا DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#تعریف_کن DEBUG_IPC_HOOK			1
#پایان_اگر

#إذا DEBUG_TRACE

#تعریف_کن VF_SCHEDULING		(1L << 1)
#تعریف_کن VF_PICKPROC		(1L << 2)

#تعریف_کن TRACE(کد, statement) إذا(verboseflags & کد) { چاپ_کن("%s:%d: ", __FILE__, __LINE__); statement }

#وإلا
#تعریف_کن TRACE(کد, statement)
#پایان_اگر

#اگر_تعریف_شده CONFIG_BOOT_VERBOSE
#تعریف_کن BOOT_VERBOSE(x)	x
#وإلا
#تعریف_کن BOOT_VERBOSE(x)
#پایان_اگر

#اگر_تعریف_شده _SYSTEM
#تعریف_کن DEBUG_PRINT(params, level) افعل { \
	إذا (verboseboot >= (level)) چاپ_کن params; } طالما (0)
#تعریف_کن DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#تعریف_کن DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#تعریف_کن DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#پایان_اگر

#پایان_اگر /* DEBUG_H */
