#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਜੇ_ਅੰਤ

/* Debug info via serial (see ser_debug()) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_IPCSTATS			0

#ਜੇ !USE_SYSDEBUG
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUG_SERIAL
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUG_ENABLE_IPC_WARNINGS
#ਜੇ_ਅੰਤ

#ਜੇ DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_IPC_HOOK			1
#ਜੇ_ਅੰਤ

#ਜੇ DEBUG_TRACE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VF_SCHEDULING		(1L << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VF_PICKPROC		(1L << 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRACE(ਕੋਡ, statement) ਜੇ(verboseflags & ਕੋਡ) { ਲਿਖੋ("%s:%d: ", __FILE__, __LINE__); statement }

#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRACE(ਕੋਡ, statement)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BOOT_VERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOOT_VERBOSE(x)	x
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOOT_VERBOSE(x)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _SYSTEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_PRINT(params, level) ਕਰੋ { \
	ਜੇ (verboseboot >= (level)) ਲਿਖੋ params; } ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* DEBUG_H */
