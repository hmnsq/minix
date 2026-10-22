#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_H

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_FORK       	   1	/* fork a new process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_NEWMAP     	   1	/* set a new memory map */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_EXEC       	   1	/* update process after execute */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_CLEAR	   1	/* clean up after process exit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_EXIT	   1	/* a system process wants to exit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_TRACE          1	/* process information and tracing */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_GETKSIG    	   1	/* retrieve pending kernel signals */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ENDKSIG    	   1	/* finish pending kernel signals */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_KILL       	   1 	/* send a signal to a process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SIGSEND    	   1	/* send POSIX-style signal */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SIGRETURN  	   1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ABORT      	   1	/* shut down MINIX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_GETINFO    	   1 	/* retrieve a copy of kernel data */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_TIMES 	   1	/* get process and system time info */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SETALARM	   1	/* schedule a synchronous alarm */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_VTIMER         1	/* set or retrieve a process-virtual timer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_DEVIO      	   1	/* read or write a single I/O port */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_VDEVIO     	   1	/* process vector with I/O requests */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SDEVIO     	   1	/* perform I/O request on a buffer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_IRQCTL     	   1	/* set an interrupt policy */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_PRIVCTL    	   1	/* system privileges control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UMAP       	   1	/* map virtual to physical address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UMAP_REMOTE	   1	/* sys_umap on behalf of another process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_VUMAP      	   1	/* vectored virtual to physical mapping */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_VIRCOPY   	   1	/* copy using virtual addressing */ 
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_PHYSCOPY  	   1 	/* copy using physical addressing */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_MEMSET  	   1	/* write char to a given memory area */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_RUNCTL         1	/* control stop flags of a process */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_STATECTL       1	/* let a process control its state */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_MCONTEXT       1	/* enable getting/setting of machine context */

#ਜੇ defined(__arm__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_PADCONF        1	/* configure pinmux */
#ਜੇ_ਅੰਤ /* __arm__ */

/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by 
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers. 
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_APIC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NR_IRQ_HOOKS	  16		/* number of interrupt hooks */
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NR_IRQ_HOOKS	  64		/* number of interrupt hooks */
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VDEVIO_BUF_ਮਾਪ   64		/* max elements per VDEVIO request */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PARAM_ਮਾਪ     512

#ਜੇ_ਅੰਤ /* CONFIG_H */

