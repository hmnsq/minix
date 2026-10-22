#यदि_अपरिभाषितम् DEBUG_H
#परिभाषय DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#यदि_अपरिभाषितम् __ASSEMBLY__
#अन्तर्भाव <minix/debug.h>
#अन्तर्भाव "config.h"
#यदि_अन्तः

/* Debug info via serial (see ser_debug()) */
#परिभाषय DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#परिभाषय DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#परिभाषय DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#परिभाषय DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#परिभाषय DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#परिभाषय DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#परिभाषय DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#परिभाषय DEBUG_IPCSTATS			0

#यदि !USE_SYSDEBUG
#अपरिभाषय DEBUG_SERIAL
#अपरिभाषय DEBUG_ENABLE_IPC_WARNINGS
#यदि_अन्तः

#यदि DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#परिभाषय DEBUG_IPC_HOOK			1
#यदि_अन्तः

#यदि DEBUG_TRACE

#परिभाषय VF_SCHEDULING		(1L << 1)
#परिभाषय VF_PICKPROC		(1L << 2)

#परिभाषय TRACE(संहिता, statement) यदि(verboseflags & संहिता) { प्रारूप_लिख("%s:%d: ", __FILE__, __LINE__); statement }

#अन्यथा
#परिभाषय TRACE(संहिता, statement)
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_BOOT_VERBOSE
#परिभाषय BOOT_VERBOSE(x)	x
#अन्यथा
#परिभाषय BOOT_VERBOSE(x)
#यदि_अन्तः

#यदि_परिभाषितम् _SYSTEM
#परिभाषय DEBUG_PRINT(params, level) कुरु { \
	यदि (verboseboot >= (level)) प्रारूप_लिख params; } यावत् (0)
#परिभाषय DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#परिभाषय DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#परिभाषय DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#यदि_अन्तः

#यदि_अन्तः /* DEBUG_H */
