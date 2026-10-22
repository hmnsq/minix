#که_نه_تعریف_شوی DEBUG_H
#تعریف_کړه DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#که_نه_تعریف_شوی __ASSEMBLY__
#ضمن <minix/debug.h>
#ضمن "config.h"
#د_که_پای

/* Debug info via serial (see ser_debug()) */
#تعریف_کړه DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#تعریف_کړه DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#تعریف_کړه DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#تعریف_کړه DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#تعریف_کړه DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#تعریف_کړه DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#تعریف_کړه DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#تعریف_کړه DEBUG_IPCSTATS			0

#إذا !USE_SYSDEBUG
#تعریف_لغوه DEBUG_SERIAL
#تعریف_لغوه DEBUG_ENABLE_IPC_WARNINGS
#د_که_پای

#إذا DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#تعریف_کړه DEBUG_IPC_HOOK			1
#د_که_پای

#إذا DEBUG_TRACE

#تعریف_کړه VF_SCHEDULING		(1L << 1)
#تعریف_کړه VF_PICKPROC		(1L << 2)

#تعریف_کړه TRACE(کوډ, statement) إذا(verboseflags & کوډ) { چاپ_کړه("%s:%d: ", __FILE__, __LINE__); statement }

#وإلا
#تعریف_کړه TRACE(کوډ, statement)
#د_که_پای

#که_تعریف_شوی CONFIG_BOOT_VERBOSE
#تعریف_کړه BOOT_VERBOSE(x)	x
#وإلا
#تعریف_کړه BOOT_VERBOSE(x)
#د_که_پای

#که_تعریف_شوی _SYSTEM
#تعریف_کړه DEBUG_PRINT(params, level) افعل { \
	إذا (verboseboot >= (level)) چاپ_کړه params; } طالما (0)
#تعریف_کړه DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#تعریف_کړه DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#تعریف_کړه DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#د_که_پای

#د_که_پای /* DEBUG_H */
