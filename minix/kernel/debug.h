#جے_بنا_تعریف DEBUG_H
#تعریف_کرو DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#جے_بنا_تعریف __ASSEMBLY__
#ضمن <minix/debug.h>
#ضمن "config.h"
#جے_انت

/* Debug info via serial (see ser_debug()) */
#تعریف_کرو DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#تعریف_کرو DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#تعریف_کرو DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#تعریف_کرو DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#تعریف_کرو DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#تعریف_کرو DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#تعریف_کرو DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#تعریف_کرو DEBUG_IPCSTATS			0

#إذا !USE_SYSDEBUG
#تعریف_رد DEBUG_SERIAL
#تعریف_رد DEBUG_ENABLE_IPC_WARNINGS
#جے_انت

#إذا DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#تعریف_کرو DEBUG_IPC_HOOK			1
#جے_انت

#إذا DEBUG_TRACE

#تعریف_کرو VF_SCHEDULING		(1L << 1)
#تعریف_کرو VF_PICKPROC		(1L << 2)

#تعریف_کرو TRACE(کوڈ, statement) إذا(verboseflags & کوڈ) { لکھو("%s:%d: ", __FILE__, __LINE__); statement }

#وإلا
#تعریف_کرو TRACE(کوڈ, statement)
#جے_انت

#جے_تعریف_شدہ CONFIG_BOOT_VERBOSE
#تعریف_کرو BOOT_VERBOSE(x)	x
#وإلا
#تعریف_کرو BOOT_VERBOSE(x)
#جے_انت

#جے_تعریف_شدہ _SYSTEM
#تعریف_کرو DEBUG_PRINT(params, level) افعل { \
	إذا (verboseboot >= (level)) لکھو params; } طالما (0)
#تعریف_کرو DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#تعریف_کرو DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#تعریف_کرو DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#جے_انت

#جے_انت /* DEBUG_H */
