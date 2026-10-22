#اگر_تعریف_نشده CONFIG_H
#تعریف_کن CONFIG_H

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
#تعریف_کن USE_FORK       	   1	/* fork a new process */
#تعریف_کن USE_NEWMAP     	   1	/* set a new memory map */
#تعریف_کن USE_EXEC       	   1	/* update process after execute */
#تعریف_کن USE_CLEAR	   1	/* clean up after process exit */
#تعریف_کن USE_EXIT	   1	/* a system process wants to exit */
#تعریف_کن USE_TRACE          1	/* process information and tracing */
#تعریف_کن USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#تعریف_کن USE_ENDKSIG    	   1	/* finish pending kernel signals */
#تعریف_کن USE_KILL       	   1 	/* send a signal to a process */
#تعریف_کن USE_SIGSEND    	   1	/* send POSIX-style signal */
#تعریف_کن USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#تعریف_کن USE_ABORT      	   1	/* shut down MINIX */
#تعریف_کن USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#تعریف_کن USE_TIMES 	   1	/* get process and system time info */
#تعریف_کن USE_SETALARM	   1	/* schedule a synchronous alarm */
#تعریف_کن USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#تعریف_کن USE_DEVIO      	   1	/* read or write a single I/O port */
#تعریف_کن USE_VDEVIO     	   1	/* process vector with I/O requests */
#تعریف_کن USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#تعریف_کن USE_IRQCTL     	   1	/* set an interrupt policy */
#تعریف_کن USE_PRIVCTL    	   1	/* system privileges control */
#تعریف_کن USE_UMAP       	   1	/* map virtual to physical address */
#تعریف_کن USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#تعریف_کن USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#تعریف_کن USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#تعریف_کن USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#تعریف_کن USE_MEMSET  	   1	/* write char to a given memory area */
#تعریف_کن USE_RUNCTL         1	/* control stop flags of a process */
#تعریف_کن USE_STATECTL       1	/* let a process control its state */
#تعریف_کن USE_MCONTEXT       1	/* enable getting/setting of machine context */

#إذا defined(__arm__)
#تعریف_کن USE_PADCONF        1	/* configure pinmux */
#پایان_اگر /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#اگر_تعریف_نشده USE_APIC
#تعریف_کن NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#وإلا
#تعریف_کن NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#پایان_اگر
#تعریف_کن VDEVIO_BUF_اندازه   64		/* max elements per VDEVIO request */

#تعریف_کن K_PARAM_اندازه     512

#پایان_اگر /* CONFIG_H */

