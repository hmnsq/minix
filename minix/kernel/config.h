#جيڪڏهن_وصف_نه_ٿيل CONFIG_H
#وصف_ڪريو CONFIG_H

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
#وصف_ڪريو USE_FORK       	   1	/* fork a new process */
#وصف_ڪريو USE_NEWMAP     	   1	/* set a new memory map */
#وصف_ڪريو USE_EXEC       	   1	/* update process after execute */
#وصف_ڪريو USE_CLEAR	   1	/* clean up after process exit */
#وصف_ڪريو USE_EXIT	   1	/* a system process wants to exit */
#وصف_ڪريو USE_TRACE          1	/* process information and tracing */
#وصف_ڪريو USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#وصف_ڪريو USE_ENDKSIG    	   1	/* finish pending kernel signals */
#وصف_ڪريو USE_KILL       	   1 	/* send a signal to a process */
#وصف_ڪريو USE_SIGSEND    	   1	/* send POSIX-style signal */
#وصف_ڪريو USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#وصف_ڪريو USE_ABORT      	   1	/* shut down MINIX */
#وصف_ڪريو USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#وصف_ڪريو USE_TIMES 	   1	/* get process and system time info */
#وصف_ڪريو USE_SETALARM	   1	/* schedule a synchronous alarm */
#وصف_ڪريو USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#وصف_ڪريو USE_DEVIO      	   1	/* read or write a single I/O port */
#وصف_ڪريو USE_VDEVIO     	   1	/* process vector with I/O requests */
#وصف_ڪريو USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#وصف_ڪريو USE_IRQCTL     	   1	/* set an interrupt policy */
#وصف_ڪريو USE_PRIVCTL    	   1	/* system privileges control */
#وصف_ڪريو USE_UMAP       	   1	/* map virtual to physical address */
#وصف_ڪريو USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#وصف_ڪريو USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#وصف_ڪريو USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#وصف_ڪريو USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#وصف_ڪريو USE_MEMSET  	   1	/* write char to a given memory area */
#وصف_ڪريو USE_RUNCTL         1	/* control stop flags of a process */
#وصف_ڪريو USE_STATECTL       1	/* let a process control its state */
#وصف_ڪريو USE_MCONTEXT       1	/* enable getting/setting of machine context */

#إذا defined(__arm__)
#وصف_ڪريو USE_PADCONF        1	/* configure pinmux */
#جيڪڏهن_پورو /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#جيڪڏهن_وصف_نه_ٿيل USE_APIC
#وصف_ڪريو NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#وإلا
#وصف_ڪريو NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#جيڪڏهن_پورو
#وصف_ڪريو VDEVIO_BUF_سائيز   64		/* max elements per VDEVIO request */

#وصف_ڪريو K_PARAM_سائيز     512

#جيڪڏهن_پورو /* CONFIG_H */

