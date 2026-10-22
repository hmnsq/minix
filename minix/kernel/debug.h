#अगर_अघोषित DEBUG_H
#घोषणा DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#अगर_अघोषित __ASSEMBLY__
#समावेश <minix/debug.h>
#समावेश "config.h"
#पूर्ण_अगर

/* Debug info via serial (see ser_debug()) */
#घोषणा DEBUG_SERIAL			1

/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#घोषणा DEBUG_ENABLE_IPC_WARNINGS	1

/* Sanity checks. */
#घोषणा DEBUG_SANITYCHECKS		0

/* Verbose messages. */
#घोषणा DEBUG_TRACE			0

/* DEBUG_RACE makes every process preemptible, schedules
 * every process on the same priority queue, and randomizes
 * the next process to run, in order to help catch race
 * conditions that could otherwise be masked.
 */
#घोषणा DEBUG_RACE			0

/* DEBUG_DUMPIPC dumps all IPC to serial; due to the amount of logging it is 
 * strongly recommended to set "ctty 0" in the boot monitor and run inside a 
 * virtual machine if you enable this; on the hardware it would take forever 
 * just to boot
 */
#घोषणा DEBUG_DUMPIPC			0

/* DEBUG_DUMPIPCF dumps filtered IPC to serial.
 */
#घोषणा DEBUG_DUMPIPCF			0

/* If defined, restrict DEBUG_DUMPIPC to particular process names */
/* #define DEBUG_DUMPIPC_NAMES		{ "tty", "pty" } */

/* DEBUG_IPCSTATS collects information on who sends messages to whom. */
#घोषणा DEBUG_IPCSTATS			0

#अगर !USE_SYSDEBUG
#अघोषित DEBUG_SERIAL
#अघोषित DEBUG_ENABLE_IPC_WARNINGS
#पूर्ण_अगर

#अगर DEBUG_DUMPIPC || DEBUG_IPCSTATS	/* either of these needs the hook */
#घोषणा DEBUG_IPC_HOOK			1
#पूर्ण_अगर

#अगर DEBUG_TRACE

#घोषणा VF_SCHEDULING		(1L << 1)
#घोषणा VF_PICKPROC		(1L << 2)

#घोषणा TRACE(code, statement) अगर(verboseflags & code) { म_लिखो("%s:%d: ", __खाता__, __LINE__); statement }

#अन्यथा
#घोषणा TRACE(code, statement)
#पूर्ण_अगर

#अगर_घोषित CONFIG_BOOT_VERBOSE
#घोषणा BOOT_VERBOSE(x)	x
#अन्यथा
#घोषणा BOOT_VERBOSE(x)
#पूर्ण_अगर

#अगर_घोषित _SYSTEM
#घोषणा DEBUG_PRINT(params, level) करो { \
	अगर (verboseboot >= (level)) म_लिखो params; } जबतक (0)
#घोषणा DEBUGBASIC(params) DEBUG_PRINT(params, VERBOSEBOOT_BASIC)
#घोषणा DEBUGEXTRA(params) DEBUG_PRINT(params, VERBOSEBOOT_EXTRA)
#घोषणा DEBUGMAX(params)   DEBUG_PRINT(params, VERBOSEBOOT_MAX)
#पूर्ण_अगर

#पूर्ण_अगर /* DEBUG_H */
