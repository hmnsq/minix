#अगर_अघोषित CONFIG_H
#घोषणा CONFIG_H

/* This file defines the kernel configuration. It allows to set sizes of some
 * kernel buffers and to enable or disable debugging code, timing features, 
 * and individual kernel calls.
 *
 * Changes:
 *   Jul 11, 2005	Created.  (Jorrit N. Herder)
 */

/* In embedded and sensor applications, not all the kernel calls may be
 * needed. In this section you can specify which kernel calls are needed
 * and which are not. The code for unneeded kernel calls is not included in
 * the system binary, making it smaller. If you are not sure, it is best
 * to keep all kernel calls enabled.
 */
#घोषणा USE_FORK       	   1	/* fork a new process */
#घोषणा USE_NEWMAP     	   1	/* set a new memory map */
#घोषणा USE_EXEC       	   1	/* update process after execute */
#घोषणा USE_CLEAR	   1	/* clean up after process exit */
#घोषणा USE_EXIT	   1	/* a system process wants to exit */
#घोषणा USE_TRACE          1	/* process information and tracing */
#घोषणा USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#घोषणा USE_ENDKSIG    	   1	/* finish pending kernel signals */
#घोषणा USE_KILL       	   1 	/* send a signal to a process */
#घोषणा USE_SIGSEND    	   1	/* send POSIX-style signal */
#घोषणा USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#घोषणा USE_ABORT      	   1	/* shut down MINIX */
#घोषणा USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#घोषणा USE_TIMES 	   1	/* get process and system time info */
#घोषणा USE_SETALARM	   1	/* schedule a synchronous alarm */
#घोषणा USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#घोषणा USE_DEVIO      	   1	/* read or write a single I/O port */
#घोषणा USE_VDEVIO     	   1	/* process vector with I/O requests */
#घोषणा USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#घोषणा USE_IRQCTL     	   1	/* set an interrupt policy */
#घोषणा USE_PRIVCTL    	   1	/* system privileges control */
#घोषणा USE_UMAP       	   1	/* map virtual to physical address */
#घोषणा USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#घोषणा USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#घोषणा USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#घोषणा USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#घोषणा USE_MEMSET  	   1	/* write char to a given memory area */
#घोषणा USE_RUNCTL         1	/* control stop flags of a process */
#घोषणा USE_STATECTL       1	/* let a process control its state */
#घोषणा USE_MCONTEXT       1	/* enable getting/setting of machine context */

#अगर defined(__arm__)
#घोषणा USE_PADCONF        1	/* configure pinmux */
#पूर्ण_अगर /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#अगर_अघोषित USE_APIC
#घोषणा NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#अन्यथा
#घोषणा NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#पूर्ण_अगर
#घोषणा VDEVIO_BUF_आकार   64		/* max elements per VDEVIO request */

#घोषणा K_PARAM_आकार     512

#पूर्ण_अगर /* CONFIG_H */

