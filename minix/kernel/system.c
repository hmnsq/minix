/* This task handles the interface between the kernel and user-level servers.
 * System services can be accessed by doing a system call. System calls are
 * transformed into request messages, which are handled by this task. By
 * convention, a sys_call() is transformed in a SYS_CALL request message that
 * is handled in a function named do_call().
 *
 * A private call vector is used to map all system calls to the functions that
 * handle them. The actual handler functions are contained in separate files
 * to keep this file clean. The call vector is used in the system task's main
 * loop to handle all incoming requests.
 *
 * In addition to the main sys_task() entry point, which starts the main loop,
 * there are several other minor entry points:
 *   get_priv:		assign privilege structure to user or system process
 *   set_sendto_bit:	allow a process to send messages to a new target
 *   unset_sendto_bit:	disallow a process from sending messages to a target
 *   fill_sendto_mask:	fill the target mask of a given process
 *   send_sig:		send a signal directly to a system process
 *   cause_sig:		take action to cause a signal to occur via a signal mgr
 *   sig_delay_done:	tell PM that a process is not sending
 *   send_diag_sig:	send a diagnostics signal to interested processes
 *   get_randomness:	accumulate randomness in a buffer
 *   clear_endpoint:	remove a process' ability to send and receive messages
 *   sched_proc:	schedule a process
 *
 * Changes:
*    Nov 22, 2009   get_priv supports static priv ids (Cristiano Giuffrida)
 *   Aug 04, 2005   check if system call is allowed  (Jorrit N. Herder)
 *   Jul 20, 2005   send signal to services with message  (Jorrit N. Herder)
 *   Jan 15, 2005   new, generalized virtual copy function  (Jorrit N. Herder)
 *   Oct 10, 2004   dispatch system calls from call vector  (Jorrit N. Herder)
 *   Sep 30, 2004   source code documentation updated  (Jorrit N. Herder)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>
#ಅನ್ತರ್ಭಾವ <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*call_vec[NR_SYS_CALLS])(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr);

#ವ್ಯಾಖ್ಯಾನಿಸು map(call_nr, handler) 					\
    {	ಪೂರ್ಣಾಂಕ call_ಸೂಚ್ಯಂಕ = call_nr-KERNEL_CALL; 				\
    	ಪ್ರತಿಜ್ಞಾ(call_ಸೂಚ್ಯಂಕ >= 0 && call_ಸೂಚ್ಯಂಕ < NR_SYS_CALLS);			\
    call_vec[call_ಸೂಚ್ಯಂಕ] = (handler)  ; }

ಸ್ಥಿರ ಶೂನ್ಯ kernel_call_ಮುಕ್ತಾಯ(ರಚನೆ proc * caller, ಸಂದೇಶ *ಸಂದೇಶ_2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ)
{
  ಆದರೆ(ಫಲಿತಾಂಶ == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(caller, RTS_VMREQUEST));
	  ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *ಸಂದೇಶ_2;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } ಇಲ್ಲದಿದ್ದರೆ {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_ಮೂಲ = NONE;
	  ಆದರೆ (ಫಲಿತಾಂಶ != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  ಸಂದೇಶ_2->m_ಮೂಲ = SYSTEM;
		  ಸಂದೇಶ_2->m_type = ಫಲಿತಾಂಶ;		/* report status of call */
#ಆದರೆ DEBUG_IPC_HOOK
	hook_ipc_msgkresult(ಸಂದೇಶ_2, caller);
#ಆದರೆ_ಅಂತ್ಯ
		  ಆದರೆ (copy_ಸಂದೇಶ_ಗಮ್ಯ_user(ಸಂದೇಶ_2, (ಸಂದೇಶ *)caller->p_delivermsg_vir)) {
			  ಬರೆ("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ kernel_call_dispatch(ರಚನೆ proc * caller, ಸಂದೇಶ *ಸಂದೇಶ_2)
{
  ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = OK;
  ಪೂರ್ಣಾಂಕ call_nr;

#ಆದರೆ DEBUG_IPC_HOOK
	hook_ipc_msgkcall(ಸಂದೇಶ_2, caller);
#ಆದರೆ_ಅಂತ್ಯ
  call_nr = ಸಂದೇಶ_2->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  ಆದರೆ (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  ಬರೆ("SYSTEM: illegal request %d from %d.\n",
			  call_nr,ಸಂದೇಶ_2->m_ಮೂಲ);
	  ಫಲಿತಾಂಶ = EBADREQUEST;			/* illegal message type */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!GET_ಬಿಟ್(priv(caller)->s_k_call_ಮುಖವಾಡ, call_nr)) {
	  ಬರೆ("SYSTEM: denied request %d from %d.\n",
			  call_nr,ಸಂದೇಶ_2->m_ಮೂಲ);
	  ಫಲಿತಾಂಶ = ECALLDENIED;			/* illegal message type */
  } ಇಲ್ಲದಿದ್ದರೆ {
	  /* handle the system call */
	  ಆದರೆ (call_vec[call_nr])
		  ಫಲಿತಾಂಶ = (*call_vec[call_nr])(caller, ಸಂದೇಶ_2);
	  ಇಲ್ಲದಿದ್ದರೆ {
		  ಬರೆ("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  ಫಲಿತಾಂಶ = EBADREQUEST;
	  }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
ಶೂನ್ಯ kernel_call(ಸಂದೇಶ *m_user, ರಚನೆ proc * caller)
{
  ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = OK;
  ಸಂದೇಶ ಸಂದೇಶ_2;

  caller->p_delivermsg_vir = (vir_ಬೈಟುಗಳು) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  ಆದರೆ (copy_ಸಂದೇಶ_from_user(m_user, &ಸಂದೇಶ_2) == 0) {
	  ಸಂದೇಶ_2.m_ಮೂಲ = caller->p_endpoint;
	  ಫಲಿತಾಂಶ = kernel_call_dispatch(caller, &ಸಂದೇಶ_2);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
	  ಬರೆ("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  ಹಿಂತಿರುಗಿ;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_ಮುಕ್ತಾಯ(caller, &ಸಂದೇಶ_2, ಫಲಿತಾಂಶ);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
ಶೂನ್ಯ system_init(ಶೂನ್ಯ)
{
  ನೋಂದಣಿ ರಚನೆ priv *sp;
  ಪೂರ್ಣಾಂಕ i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  ಗಾಗಿ (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  ಗಾಗಿ (sp=BEG_PRIV_ADDR; sp < ಕೊನೆ_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  ಗಾಗಿ (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = ಖಾಲಿ;
  }

  /* Process management. */
  map(SYS_FORK, do_fork); 		/* a process forked a new process */
  map(SYS_EXEC, do_exec);		/* update process after execute */
  map(SYS_CLEAR, do_clear);		/* clean up after process exit */
  map(SYS_EXIT, do_exit);		/* a system process wants to exit */
  map(SYS_PRIVCTL, do_privctl);		/* system privileges control */
  map(SYS_TRACE, do_trace);		/* request a trace operation */
  map(SYS_SETGRANT, do_setgrant);	/* get/set own parameters */
  map(SYS_RUNCTL, do_runctl);		/* set/clear stop flag of a process */
  map(SYS_UPDATE, do_update);		/* update a process into another */
  map(SYS_STATECTL, do_statectl);	/* let a process control its state */

  /* Signal handling. */
  map(SYS_KILL, do_kill); 		/* cause a process to be signaled */
  map(SYS_GETKSIG, do_getksig);		/* signal manager checks for signals */
  map(SYS_ENDKSIG, do_endksig);		/* signal manager finished signal */
  map(SYS_SIGSEND, do_sigsend);		/* start POSIX-style signal */
  map(SYS_SIGRETURN, do_sigreturn);	/* return from POSIX-style signal */

  /* Device I/O. */
  map(SYS_IRQCTL, do_irqctl);  		/* interrupt control operations */
#ಆದರೆ defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#ಆದರೆ_ಅಂತ್ಯ

  /* Memory management. */
  map(SYS_MEMSET, do_memset);		/* write char to memory area */
  map(SYS_VMCTL, do_vmctl);		/* various VM process settings */

  /* Copying. */
  map(SYS_UMAP, do_umap);		/* map virtual to physical address */
  map(SYS_UMAP_REMOTE, do_umap_remote);	/* do_umap for non-caller process */
  map(SYS_VUMAP, do_vumap);		/* vectored virtual to physical map */
  map(SYS_VIRCOPY, do_vircopy); 	/* use pure virtual addressing */
  map(SYS_PHYSCOPY, do_copy);	 	/* use physical addressing */
  map(SYS_SAFECOPYFROM, do_safecopy_from);/* copy with pre-granted permission */
  map(SYS_SAFECOPYTO, do_safecopy_ಗಮ್ಯ);	/* copy with pre-granted permission */
  map(SYS_VSAFECOPY, do_vsafecopy);	/* vectored safecopy */

  /* safe memset */
  map(SYS_SAFEMEMSET, do_safememset);	/* safememset */

  /* Clock functionality. */
  map(SYS_TIMES, do_times);		/* get uptime and process times */
  map(SYS_SETALARM, do_setalarm);	/* schedule a synchronous alarm */
  map(SYS_STIME, do_stime);		/* set the boottime */
  map(SYS_SETTIME, do_settime);		/* set the system time (realtime) */
  map(SYS_VTIMER, do_vtimer);		/* set or retrieve a virtual timer */

  /* System control. */
  map(SYS_ABORT, do_abort);		/* abort MINIX */
  map(SYS_GETINFO, do_getinfo); 	/* request system information */
  map(SYS_DIAGCTL, do_diagctl);		/* diagnostics-related functionality */

  /* Profiling. */
  map(SYS_SPROF, do_sprofile);         /* start/stop statistical profiling */

  /* arm-specific. */
#ಆದರೆ defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#ಆದರೆ_ಅಂತ್ಯ

  /* i386-specific. */
#ಆದರೆ defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#ಆದರೆ_ಅಂತ್ಯ

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_ವೇಳಾಪಟ್ಟಿ, do_ವೇಳಾಪಟ್ಟಿ);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ get_priv(
  ನೋಂದಣಿ ರಚನೆ proc *rc,		/* new (child) process pointer */
  ಪೂರ್ಣಾಂಕ priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  ನೋಂದಣಿ ರಚನೆ priv *sp;                 /* privilege structure */

  ಆದರೆ(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      ಗಾಗಿ (sp = BEG_DYN_PRIV_ADDR; sp < ಕೊನೆ_DYN_PRIV_ADDR; ++sp)
          ಆದರೆ (sp->s_proc_nr == NONE) ನಿಲ್ಲಿಸು;
      ಆದರೆ (sp >= ಕೊನೆ_DYN_PRIV_ADDR) ಹಿಂತಿರುಗಿ(ENOSPC);
  }
  ಇಲ್ಲದಿದ್ದರೆ {                                    /* allocate slot from id */
      ಆದರೆ(!is_static_priv_id(priv_id)) {
          ಹಿಂತಿರುಗಿ EINVAL;                    /* invalid static priv id */
      }
      ಆದರೆ(priv[priv_id].s_proc_nr != NONE) {
          ಹಿಂತಿರುಗಿ EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  ಹಿಂತಿರುಗಿ(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
ಶೂನ್ಯ ಗಣ_sendto_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  ಆದರೆ (id_ಗಮ್ಯ_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_ಬಿಟ್(priv(rp)->s_ipc_ಗಮ್ಯ, id);
	ಹಿಂತಿರುಗಿ;
  }

  ಗಣ_sys_ಬಿಟ್(priv(rp)->s_ipc_ಗಮ್ಯ, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  ಆದರೆ (priv_addr(id)->s_trap_ಮುಖವಾಡ & ~((1 << RECEIVE)))
      ಗಣ_sys_ಬಿಟ್(priv_addr(id)->s_ipc_ಗಮ್ಯ, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
ಶೂನ್ಯ unset_sendto_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_ಬಿಟ್(priv(rp)->s_ipc_ಗಮ್ಯ, id);

  unset_sys_ಬಿಟ್(priv_addr(id)->s_ipc_ಗಮ್ಯ, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
ಶೂನ್ಯ fill_sendto_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rp, sys_map_t *map)
{
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ (i=0; i < NR_SYS_PROCS; i++) {
  	ಆದರೆ (get_sys_ಬಿಟ್(*map, i))
  		ಗಣ_sendto_ಬಿಟ್(rp, i);
  	ಇಲ್ಲದಿದ್ದರೆ
  		unset_sendto_ಬಿಟ್(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ send_sig(endpoint_t ep, ಪೂರ್ಣಾಂಕ sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  ನೋಂದಣಿ ರಚನೆ proc *rp;
  ರಚನೆ priv *priv;
  ಪೂರ್ಣಾಂಕ proc_nr;

  ಆದರೆ(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	ಹಿಂತಿರುಗಿ EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  ಆದರೆ(!priv) ಹಿಂತಿರುಗಿ ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
ಶೂನ್ಯ cause_sig(proc_nr_t proc_nr, ಪೂರ್ಣಾಂಕ sig_nr)
{
/* A system process wants to send signal 'sig_nr' to process 'proc_nr'.
 * Examples are:
 *  - HARDWARE wanting to cause a SIGSEGV after a CPU exception
 *  - TTY wanting to cause SIGINT upon getting a DEL
 *  - FS wanting to cause SIGPIPE for a broken pipe
 * Signals are handled by sending a message to the signal manager assigned to
 * the process. This function handles the signals and makes sure the signal
 * manager gets them by sending a notification. The process being signaled
 * is blocked while the signal manager has not finished all signals for it.
 * Race conditions between calls to this function and the system calls that
 * process pending kernel signals cannot exist. Signal related functions are
 * only called when a user process causes a CPU exception and from the kernel
 * process level, which runs to completion.
 */
  ನೋಂದಣಿ ರಚನೆ proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  ಪೂರ್ಣಾಂಕ sig_mgr_proc_nr;
  ಪೂರ್ಣಾಂಕ s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  ಆದರೆ(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  ಆದರೆ(rp->p_endpoint == sig_mgr) {
       ಆದರೆ(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           ಆದರೆ(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               ಹಿಂತಿರುಗಿ;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       ಆದರೆ(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       ಹಿಂತಿರುಗಿ;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  ಆದರೆ (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      ಆದರೆ (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_ಗಣ(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          ಆದರೆ(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
ಶೂನ್ಯ sig_delay_ಮುಗಿದ(ರಚನೆ proc *rp)
{
/* A process is now known not to send any direct messages.
 * Tell PM that the stop delay has ended, by sending a signal to the process.
 * Used for actual signal delivery.
 */

  rp->p_misc_flags &= ~MF_SIG_DELAY;

  cause_sig(proc_nr(rp), SIGSNDELAY);
}

/*===========================================================================*
 *				send_diag_sig				     *
 *===========================================================================*/
ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  ರಚನೆ priv *privp;
  endpoint_t ep;

  ಗಾಗಿ (privp = BEG_PRIV_ADDR; privp < ಕೊನೆ_PRIV_ADDR; privp++) {
	ಆದರೆ (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ clear_memreq(ರಚನೆ proc *rp)
{
  ರಚನೆ proc **rpp;

  ಆದರೆ (!RTS_ISSET(rp, RTS_VMREQUEST))
	ಹಿಂತಿರುಗಿ; /* nothing to do */

  ಗಾಗಿ (rpp = &vmrequest; *rpp != ಖಾಲಿ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ಆದರೆ (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		ನಿಲ್ಲಿಸು;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ clear_ipc(
  ನೋಂದಣಿ ರಚನೆ proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  ರಚನೆ proc **xpp;			/* iterate over caller queue */

  ಆದರೆ (RTS_ISSET(rc, RTS_SENDING)) {
      ಪೂರ್ಣಾಂಕ ಗುರಿ_proc;

      okendpt(rc->p_sendto_e, &ಗುರಿ_proc);
      xpp = &proc_addr(ಗುರಿ_proc)->p_caller_q; /* destination's queue */
      ಆಗ (*xpp) {		/* check entire queue */
          ಆದರೆ (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
	      ಬರೆ("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#ಆದರೆ_ಅಂತ್ಯ
              ನಿಲ್ಲಿಸು;				/* can only be queued once */
          }
          xpp = &(*xpp)->p_q_link;		/* proceed to next queued */
      }
      RTS_UNSET(rc, RTS_SENDING);
  }
  RTS_UNSET(rc, RTS_RECEIVING);
}

/*===========================================================================*
 *			         clear_endpoint				     *
 *===========================================================================*/
ಶೂನ್ಯ clear_endpoint(ರಚನೆ proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  ಆದರೆ(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#ಆದರೆ DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#ಆದರೆ_ಅಂತ್ಯ

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_ಗಣ(rc, RTS_NO_ENDPOINT);
  ಆದರೆ (priv(rc)->s_flags & SYS_PROC)
  {
	priv(rc)->s_asynsize= 0;
  }

  /* If the process happens to be queued trying to send a
   * message, then it must be removed from the message queues.
   */
  clear_ipc(rc);

  /* Likewise, if another process was sending or receive a message to or from
   * the exiting process, it must be alerted that process no longer is alive.
   * Check all processes.
   */
  clear_ipc_refs(rc, EDEADSRCDST);

  /* Finally, if the process was blocked on a VM request, remove it from the
   * queue of processes waiting to be processed by VM.
   */
  clear_memreq(rc);
}

/*===========================================================================*
 *			       clear_ipc_refs				     *
 *===========================================================================*/
ಶೂನ್ಯ clear_ipc_refs(
  ನೋಂದಣಿ ರಚನೆ proc *rc,		/* slot of process to clean up */
  ಪೂರ್ಣಾಂಕ caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  ರಚನೆ proc *rp;			/* iterate over process table */
  ಪೂರ್ಣಾಂಕ src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  ಆಗ ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_ಗಮ್ಯ_nr(src_id)), rc);

  ಗಾಗಿ (rp = BEG_PROC_ADDR; rp < ಕೊನೆ_PROC_ADDR; rp++) {
      ಆದರೆ(isemptyp(rp))
	ಮುಂದುವರಿ;

      /* Unset pending notification bits. */
      unset_sys_ಬಿಟ್(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_ಬಿಟ್(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      ಆದರೆ (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
ಶೂನ್ಯ kernel_call_resume(ರಚನೆ proc *caller)
{
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;

	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_ಸ್ಥಾನ_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_VMREQUEST));

	ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.saved.reqmsg.m_ಮೂಲ == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	ಫಲಿತಾಂಶ = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_ಮುಕ್ತಾಯ(caller, &caller->p_vmrequest.saved.reqmsg, ಫಲಿತಾಂಶ);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ sched_proc(ರಚನೆ proc *p, ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಪೂರ್ಣಾಂಕ ಕಾಲಾಂಶ, ಪೂರ್ಣಾಂಕ cpu, ಪೂರ್ಣಾಂಕ niced)
{
	/* Make sure the values given are within the allowed range.*/
	ಆದರೆ ((ಆದ್ಯತೆ < TASK_Q && ಆದ್ಯತೆ != -1) || ಆದ್ಯತೆ > NR_SCHED_QUEUES)
		ಹಿಂತಿರುಗಿ(EINVAL);

	ಆದರೆ (ಕಾಲಾಂಶ < 1 && ಕಾಲಾಂಶ != -1)
		ಹಿಂತಿರುಗಿ(EINVAL);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ ((cpu < 0 && cpu != -1) || (cpu > 0 && (ಚಿಹ್ನೆರಹಿತ) cpu >= ncpus))
		ಹಿಂತಿರುಗಿ(EINVAL);
	ಆದರೆ (cpu != -1 && !(cpu_is_ready(cpu)))
		ಹಿಂತಿರುಗಿ EBADCPU;
#ಆದರೆ_ಅಂತ್ಯ

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	ಆದರೆ (proc_is_runnable(p)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
		ಆದರೆ (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_ವೇಳಾಪಟ್ಟಿ_migrate_proc(p, cpu);
		}
#ಆದರೆ_ಅಂತ್ಯ

		RTS_ಗಣ(p, RTS_NO_ಕಾಲಾಂಶ);
	}

	ಆದರೆ (proc_is_runnable(p))
		RTS_ಗಣ(p, RTS_NO_ಕಾಲಾಂಶ);

	ಆದರೆ (ಆದ್ಯತೆ != -1)
		p->p_ಆದ್ಯತೆ = ಆದ್ಯತೆ;
	ಆದರೆ (ಕಾಲಾಂಶ != -1) {
		p->p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms = ಕಾಲಾಂಶ;
		p->p_cpu_ಸಮಯ_ಎಡ = ms_2_cpu_ಸಮಯ(ಕಾಲಾಂಶ);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ (cpu != -1)
		p->p_cpu = cpu;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (niced)
		p->p_misc_flags |= MF_NICED;
	ಇಲ್ಲದಿದ್ದರೆ
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_ಕಾಲಾಂಶ);

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_ipc_filter(ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ type, vir_ಬೈಟುಗಳು ವಿಳಾಸ,
	size_t ಉದ್ದಳತೆ)
{
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	ಆದರೆ (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		ಹಿಂತಿರುಗಿ EINVAL;

	ಆದರೆ (ಉದ್ದಳತೆ % ಗಾತ್ರ(ipc_filter_el_t) != 0)
		ಹಿಂತಿರುಗಿ EINVAL;

	ಅಂಕೆ_elements = ಉದ್ದಳತೆ / ಗಾತ್ರ(ipc_filter_el_t);
	ಆದರೆ (ಅಂಕೆ_elements <= 0 || ಅಂಕೆ_elements > IPCF_MAX_ELEMENTS)
		ಹಿಂತಿರುಗಿ E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_ಸ್ಥಾನ(type, &ipcf);
	ಆದರೆ (ipcf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ENOMEM;

	/* Fill details. */
	ipcf->ಅಂಕೆ_elements = ಅಂಕೆ_elements;
	ipcf->ಮುಂದಿನ = ಖಾಲಿ;
	r = ದತ್ತಾಂಶ_copy(rp->p_endpoint, ವಿಳಾಸ,
		KERNEL, (vir_ಬೈಟುಗಳು)ipcf->elements, ಉದ್ದಳತೆ);
	ಆದರೆ (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	ಆದರೆ (r != OK) {
		IPCF_POOL_FREE_ಸ್ಥಾನ(ipcf);
		ಹಿಂತಿರುಗಿ r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	ಗಾಗಿ (ipcfp = &priv(rp)->s_ipcf; *ipcfp != ಖಾಲಿ;
	    ipcfp = &(*ipcfp)->ಮುಂದಿನ)
		;
	*ipcfp = ipcf;

	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
ಶೂನ್ಯ clear_ipc_filters(ರಚನೆ proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	ಆಗ (ipcf != ಖಾಲಿ) {
		curr_ipcf = ipcf;
		ipcf = ipcf->ಮುಂದಿನ;
		IPCF_POOL_FREE_ಸ್ಥಾನ(curr_ipcf);
	}

	priv(rp)->s_ipcf = ಖಾಲಿ;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	ಆದರೆ (rp->p_endpoint == VM_PROC_NR && vmrequest != ಖಾಲಿ)
		ಆದರೆ (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ check_ipc_filter(ipc_filter_t *ipcf, ಪೂರ್ಣಾಂಕ fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	ಪೂರ್ಣಾಂಕ i, ಅಂಕೆ_elements, flags;

	ಆದರೆ (ipcf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ OK;

	ಅಂಕೆ_elements = ipcf->ಅಂಕೆ_elements;
	flags = 0;
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		ಆದರೆ (!IPCF_EL_CHECK(ipcf_el))
			ಹಿಂತಿರುಗಿ EINVAL;
		flags |= ipcf_el->flags;
	}

	ಆದರೆ (fill_flags)
		ipcf->flags = flags;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ipcf->flags != flags)
		ಹಿಂತಿರುಗಿ EINVAL;
	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ allow_ipc_filtered_ಸಂದೇಶ(ರಚನೆ proc *rp, endpoint_t src_e,
	vir_ಬೈಟುಗಳು m_src_v, ಸಂದೇಶ *m_src_p)
{
	ಪೂರ್ಣಾಂಕ i, r, ಅಂಕೆ_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	ಸಂದೇಶ m_buff;

	ipcf = priv(rp)->s_ipcf;
	ಆದರೆ (ipcf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE; /* no IPC filters, always allow */

	ಆದರೆ (m_src_p == ಖಾಲಿ) {
		ಪ್ರತಿಜ್ಞಾ(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		ಮಾಡು {
#ಆದರೆ DEBUG_DUMPIPCF
			ಆದರೆ (TRUE) {
#ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (ipcf->flags & IPCF_ಹೊಂದಿಕೆ_M_TYPE) {
#ಆದರೆ_ಅಂತ್ಯ
				get_mtype = TRUE;
				ನಿಲ್ಲಿಸು;
			}
			ipcf = ipcf->ಮುಂದಿನ;
		} ಆಗ (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		ಆದರೆ (get_mtype) {
			r = ದತ್ತಾಂಶ_copy(src_e,
			    m_src_v + offsetof(ಸಂದೇಶ, m_type), KERNEL,
			    (vir_ಬೈಟುಗಳು)&m_buff.m_type, ಗಾತ್ರ(m_buff.m_type));
			ಆದರೆ (r != OK) {
				/* allow for now, this will fail later anyway */
#ಆದರೆ DEBUG_DUMPIPCF
				ಬರೆ("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#ಆದರೆ_ಅಂತ್ಯ
				ಹಿಂತಿರುಗಿ TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_ಮೂಲ = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	ಮಾಡು {
		ಆದರೆ (allow != (ipcf->type == IPCF_WHITELIST)) {
			ಅಂಕೆ_elements = ipcf->ಅಂಕೆ_elements;
			ಗಾಗಿ (i = 0; i < ಅಂಕೆ_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				ಆದರೆ (IPCF_EL_ಹೊಂದಿಕೆ(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					ನಿಲ್ಲಿಸು;
				}
			}
		}
		ipcf = ipcf->ಮುಂದಿನ;
	} ಆಗ (ipcf);

#ಆದರೆ DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ allow_ipc_filtered_memreq(ರಚನೆ proc *src_rp, ರಚನೆ proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	ರಚನೆ proc *vmp;
	ಸಂದೇಶ m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	ಆದರೆ (priv(vmp)->s_ipcf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_ಸಂದೇಶ;
	ಆದರೆ (!allow_ipc_filtered_ಸಂದೇಶ(vmp, SYSTEM, 0, &m_buf))
		ಹಿಂತಿರುಗಿ FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	ಹಿಂತಿರುಗಿ TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_irq(ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ irq)
{
        ರಚನೆ priv *priv = priv(rp);
        ಪೂರ್ಣಾಂಕ i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	ಗಾಗಿ (i = 0; i < priv->s_nr_irq; i++) {
		ಆದರೆ (priv->s_irq_tab[i] == irq)
			ಹಿಂತಿರುಗಿ OK;
	}

	i= priv->s_nr_irq;
	ಆದರೆ (i >= NR_IRQ) {
		ಬರೆ("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		ಹಿಂತಿರುಗಿ ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_io(ರಚನೆ proc *rp, ರಚನೆ io_range *ior)
{
        ರಚನೆ priv *priv = priv(rp);
        ಪೂರ್ಣಾಂಕ i;

	priv->s_flags |= CHECK_IO_ಪೋರ್ಟ್;	/* Check I/O accesses */

	ಗಾಗಿ (i = 0; i < priv->s_nr_io_range; i++) {
		ಆದರೆ (priv->s_io_tab[i].ior_ತಳ == ior->ior_ತಳ &&
			priv->s_io_tab[i].ior_ಎಲ್ಲೆ == ior->ior_ಎಲ್ಲೆ)
			ಹಿಂತಿರುಗಿ OK;
	}

	i= priv->s_nr_io_range;
	ಆದರೆ (i >= NR_IO_RANGE) {
		ಬರೆ("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		ಹಿಂತಿರುಗಿ ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_mem(ರಚನೆ proc *rp, ರಚನೆ minix_mem_range *memr)
{
        ರಚನೆ priv *priv = priv(rp);
        ಪೂರ್ಣಾಂಕ i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	ಗಾಗಿ (i = 0; i < priv->s_nr_mem_range; i++) {
		ಆದರೆ (priv->s_mem_tab[i].mr_ತಳ == memr->mr_ತಳ &&
			priv->s_mem_tab[i].mr_ಎಲ್ಲೆ == memr->mr_ಎಲ್ಲೆ)
			ಹಿಂತಿರುಗಿ OK;
	}

	i= priv->s_nr_mem_range;
	ಆದರೆ (i >= NR_MEM_RANGE) {
		ಬರೆ("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		ಹಿಂತಿರುಗಿ ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	ಹಿಂತಿರುಗಿ OK;
}

