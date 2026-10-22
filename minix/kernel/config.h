#నిర్వచితం_కాకపోతే CONFIG_H
#నిర్వచించు CONFIG_H

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
#నిర్వచించు USE_FORK       	   1	/* fork a new process */
#నిర్వచించు USE_NEWMAP     	   1	/* set a new memory map */
#నిర్వచించు USE_EXEC       	   1	/* update process after execute */
#నిర్వచించు USE_CLEAR	   1	/* clean up after process exit */
#నిర్వచించు USE_EXIT	   1	/* a system process wants to exit */
#నిర్వచించు USE_TRACE          1	/* process information and tracing */
#నిర్వచించు USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#నిర్వచించు USE_ENDKSIG    	   1	/* finish pending kernel signals */
#నిర్వచించు USE_KILL       	   1 	/* send a signal to a process */
#నిర్వచించు USE_SIGSEND    	   1	/* send POSIX-style signal */
#నిర్వచించు USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#నిర్వచించు USE_ABORT      	   1	/* shut down MINIX */
#నిర్వచించు USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#నిర్వచించు USE_TIMES 	   1	/* get process and system time info */
#నిర్వచించు USE_SETALARM	   1	/* schedule a synchronous alarm */
#నిర్వచించు USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#నిర్వచించు USE_DEVIO      	   1	/* read or write a single I/O port */
#నిర్వచించు USE_VDEVIO     	   1	/* process vector with I/O requests */
#నిర్వచించు USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#నిర్వచించు USE_IRQCTL     	   1	/* set an interrupt policy */
#నిర్వచించు USE_PRIVCTL    	   1	/* system privileges control */
#నిర్వచించు USE_UMAP       	   1	/* map virtual to physical address */
#నిర్వచించు USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#నిర్వచించు USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#నిర్వచించు USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#నిర్వచించు USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#నిర్వచించు USE_MEMSET  	   1	/* write char to a given memory area */
#నిర్వచించు USE_RUNCTL         1	/* control stop flags of a process */
#నిర్వచించు USE_STATECTL       1	/* let a process control its state */
#నిర్వచించు USE_MCONTEXT       1	/* enable getting/setting of machine context */

#ఒకవేళ defined(__arm__)
#నిర్వచించు USE_PADCONF        1	/* configure pinmux */
#అయితే_ముగింపు /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#నిర్వచితం_కాకపోతే USE_APIC
#నిర్వచించు NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#లేకపోతే
#నిర్వచించు NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#అయితే_ముగింపు
#నిర్వచించు VDEVIO_BUF_కొలత   64		/* max elements per VDEVIO request */

#నిర్వచించు K_PARAM_కొలత     512

#అయితే_ముగింపు /* CONFIG_H */

