#यदि_अपरिभाषितम् CONFIG_H
#परिभाषय CONFIG_H

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
#परिभाषय USE_FORK       	   1	/* fork a new process */
#परिभाषय USE_NEWMAP     	   1	/* set a new memory map */
#परिभाषय USE_EXEC       	   1	/* update process after execute */
#परिभाषय USE_CLEAR	   1	/* clean up after process exit */
#परिभाषय USE_EXIT	   1	/* a system process wants to exit */
#परिभाषय USE_TRACE          1	/* process information and tracing */
#परिभाषय USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#परिभाषय USE_ENDKSIG    	   1	/* finish pending kernel signals */
#परिभाषय USE_KILL       	   1 	/* send a signal to a process */
#परिभाषय USE_SIGSEND    	   1	/* send POSIX-style signal */
#परिभाषय USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#परिभाषय USE_ABORT      	   1	/* shut down MINIX */
#परिभाषय USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#परिभाषय USE_TIMES 	   1	/* get process and system time info */
#परिभाषय USE_SETALARM	   1	/* schedule a synchronous alarm */
#परिभाषय USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#परिभाषय USE_DEVIO      	   1	/* read or write a single I/O port */
#परिभाषय USE_VDEVIO     	   1	/* process vector with I/O requests */
#परिभाषय USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#परिभाषय USE_IRQCTL     	   1	/* set an interrupt policy */
#परिभाषय USE_PRIVCTL    	   1	/* system privileges control */
#परिभाषय USE_UMAP       	   1	/* map virtual to physical address */
#परिभाषय USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#परिभाषय USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#परिभाषय USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#परिभाषय USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#परिभाषय USE_MEMSET  	   1	/* write char to a given memory area */
#परिभाषय USE_RUNCTL         1	/* control stop flags of a process */
#परिभाषय USE_STATECTL       1	/* let a process control its state */
#परिभाषय USE_MCONTEXT       1	/* enable getting/setting of machine context */

#यदि defined(__arm__)
#परिभाषय USE_PADCONF        1	/* configure pinmux */
#यदि_अन्तः /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#यदि_अपरिभाषितम् USE_APIC
#परिभाषय NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#अन्यथा
#परिभाषय NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#यदि_अन्तः
#परिभाषय VDEVIO_BUF_आकार   64		/* max elements per VDEVIO request */

#परिभाषय K_PARAM_आकार     512

#यदि_अन्तः /* CONFIG_H */

