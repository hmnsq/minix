#جيڪڏهن_وصف_نه_ٿيل DEBUG_H
#وصف_ڪريو DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__
#ضمن <minix/debug.h>
#ضمن "config.h"
#جيڪڏهن_پورو

/* Debug info via serial (see ser_debug()) */
#وصف_ڪريو DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#وصف_ڪريو DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#وصف_ڪريو DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#وصف_ڪريو DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#وصف_ڪريو DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#وصف_ڪريو DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#وصف_ڪريو DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#وصف_ڪريو DEBUG_IPCSTATS			0

#إذا !USE_SYSDEBUG
#وصف_رد DEBUG_SERIAL
#وصف_رد DEBUG_ENABLE_IPC_WARNINGS
#جيڪڏهن_پورو

#إذا DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#وصف_ڪريو DEBUG_IPC_HOOK			1
#جيڪڏهن_پورو

#إذا DEBUG_TRACE

#وصف_ڪريو VF_SCHEDULING		(1L << 1)
#وصف_ڪريو VF_PICKPROC		(1L << 2)

#وصف_ڪريو TRACE(ڪوڊ, statement) إذا(verboseflags & ڪوڊ) { لکو("%s:%d: ", __FILE__, __LINE__); statement }

#وإلا
#وصف_ڪريو TRACE(ڪوڊ, statement)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_BOOT_VERBOSE
#وصف_ڪريو BOOT_VERBOSE(x)	x
#وإلا
#وصف_ڪريو BOOT_VERBOSE(x)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل _SYSTEM
#وصف_ڪريو DEBUG_PRINT(params, level) افعل { \
	إذا (verboseboot >= (level)) لکو params; } طالما (0)
#وصف_ڪريو DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#وصف_ڪريو DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#وصف_ڪريو DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* DEBUG_H */
