#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUG_H
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__
#ಅನ್ತರ್ಭಾವ <minix/debug.h>
#ಅನ್ತರ್ಭಾವ "config.h"
#ಆದರೆ_ಅಂತ್ಯ

/* Debug info via serial (see ser_debug()) */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_IPCSTATS			0

#ಆದರೆ !USE_SYSDEBUG
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUG_SERIAL
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUG_ENABLE_IPC_WARNINGS
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_IPC_HOOK			1
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ DEBUG_TRACE

#ವ್ಯಾಖ್ಯಾನಿಸು VF_SCHEDULING		(1L << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು VF_PICKPROC		(1L << 2)

#ವ್ಯಾಖ್ಯಾನಿಸು TRACE(ಸಂಕೇತ_2, statement) ಆದರೆ(verboseflags & ಸಂಕೇತ_2) { ಬರೆ("%s:%d: ", __FILE__, __LINE__); statement }

#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು TRACE(ಸಂಕೇತ_2, statement)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BOOT_VERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು BOOT_VERBOSE(x)	x
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು BOOT_VERBOSE(x)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _SYSTEM
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_PRINT(params, level) ಮಾಡು { \
	ಆದರೆ (verboseboot >= (level)) ಬರೆ params; } ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* DEBUG_H */
