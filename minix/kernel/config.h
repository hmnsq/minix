#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_H
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_H

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
#ವ್ಯಾಖ್ಯಾನಿಸು USE_FORK       	   1	/* fork a new process */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_NEWMAP     	   1	/* set a new memory map */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_EXEC       	   1	/* update process after execute */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_CLEAR	   1	/* clean up after process exit */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_EXIT	   1	/* a system process wants to exit */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_TRACE          1	/* process information and tracing */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ENDKSIG    	   1	/* finish pending kernel signals */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_KILL       	   1 	/* send a signal to a process */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SIGSEND    	   1	/* send POSIX-style signal */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ABORT      	   1	/* shut down MINIX */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_TIMES 	   1	/* get process and system time info */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SETALARM	   1	/* schedule a synchronous alarm */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_DEVIO      	   1	/* read or write a single I/O port */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_VDEVIO     	   1	/* process vector with I/O requests */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_IRQCTL     	   1	/* set an interrupt policy */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_PRIVCTL    	   1	/* system privileges control */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_UMAP       	   1	/* map virtual to physical address */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#ವ್ಯಾಖ್ಯಾನಿಸು USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_MEMSET  	   1	/* write char to a given memory area */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_RUNCTL         1	/* control stop flags of a process */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_STATECTL       1	/* let a process control its state */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_MCONTEXT       1	/* enable getting/setting of machine context */

#ಆದರೆ defined(__arm__)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_PADCONF        1	/* configure pinmux */
#ಆದರೆ_ಅಂತ್ಯ /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_APIC
#ವ್ಯಾಖ್ಯಾನಿಸು NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು VDEVIO_BUF_ಗಾತ್ರ   64		/* max elements per VDEVIO request */

#ವ್ಯಾಖ್ಯಾನಿಸು K_PARAM_ಗಾತ್ರ     512

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_H */

