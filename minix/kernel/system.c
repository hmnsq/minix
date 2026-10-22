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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*call_vec[NR_SYS_CALLS])(ಸಂರಚನಾ proc * caller, message *m_ptr);

#define map(call_nr, handler) 					\
    {	ಪೂರ್ಣಾಙ್ಕ call_index = call_nr-KERNEL_CALL; 				\
    	ಪ್ರತಿಜ್ಞಾ(call_index >= 0 && call_index < NR_SYS_CALLS);			\
    call_vec[call_index] = (handler)  ; }

ಸ್ಥಿರ ಶೂನ್ಯ kernel_call_finish(ಸಂರಚನಾ proc * caller, message *ಸಂದೇಶ, ಪೂರ್ಣಾಙ್ಕ result)
{
  ಯದಿ(result == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(caller, RTS_VMREQUEST));
	  ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *ಸಂದೇಶ;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } ಅನ್ಯಥಾ {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_source = NONE;
	  ಯದಿ (result != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  ಸಂದೇಶ->m_source = SYSTEM;
		  ಸಂದೇಶ->m_type = result;		/* report status of call */
#ಯದಿ DEBUG_IPC_HOOK
	hook_ipc_msgkresult(ಸಂದೇಶ, caller);
#endif
		  ಯದಿ (copy_ಸಂದೇಶ_to_user(ಸಂದೇಶ, (message *)caller->p_delivermsg_vir)) {
			  printf("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ kernel_call_dispatch(ಸಂರಚನಾ proc * caller, message *ಸಂದೇಶ)
{
  ಪೂರ್ಣಾಙ್ಕ result = OK;
  ಪೂರ್ಣಾಙ್ಕ call_nr;

#ಯದಿ DEBUG_IPC_HOOK
	hook_ipc_msgkcall(ಸಂದೇಶ, caller);
#endif
  call_nr = ಸಂದೇಶ->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  ಯದಿ (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  printf("SYSTEM: illegal request %d from %d.\n",
			  call_nr,ಸಂದೇಶ->m_source);
	  result = EBADREQUEST;			/* illegal message type */
  }
  ಅನ್ಯಥಾ ಯದಿ (!GET_BIT(priv(caller)->s_k_call_mask, call_nr)) {
	  printf("SYSTEM: denied request %d from %d.\n",
			  call_nr,ಸಂದೇಶ->m_source);
	  result = ECALLDENIED;			/* illegal message type */
  } ಅನ್ಯಥಾ {
	  /* handle the system call */
	  ಯದಿ (call_vec[call_nr])
		  result = (*call_vec[call_nr])(caller, ಸಂದೇಶ);
	  ಅನ್ಯಥಾ {
		  printf("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  result = EBADREQUEST;
	  }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
ಶೂನ್ಯ kernel_call(message *m_user, ಸಂರಚನಾ proc * caller)
{
  ಪೂರ್ಣಾಙ್ಕ result = OK;
  message ಸಂದೇಶ;

  caller->p_delivermsg_vir = (vir_bytes) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  ಯದಿ (copy_ಸಂದೇಶ_from_user(m_user, &ಸಂದೇಶ) == 0) {
	  ಸಂದೇಶ.m_source = caller->p_endpoint;
	  result = kernel_call_dispatch(caller, &ಸಂದೇಶ);
  }
  ಅನ್ಯಥಾ {
	  printf("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  ಪ್ರತ್ಯಾಗಮ;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_finish(caller, &ಸಂದೇಶ, result);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
ಶೂನ್ಯ system_init(ಶೂನ್ಯ)
{
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ priv *sp;
  ಪೂರ್ಣಾಙ್ಕ i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  ಪ್ರತಿ (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  ಪ್ರತಿ (sp=BEG_PRIV_ADDR; sp < END_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  ಪ್ರತಿ (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = ರಿಕ್ತ;
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
#ಯದಿ defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#endif

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
  map(SYS_SAFECOPYTO, do_safecopy_to);	/* copy with pre-granted permission */
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
#ಯದಿ defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#endif

  /* i386-specific. */
#ಯದಿ defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#endif

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_SCHEDULE, do_schedule);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ get_priv(
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rc,		/* new (child) process pointer */
  ಪೂರ್ಣಾಙ್ಕ priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ priv *sp;                 /* privilege structure */

  ಯದಿ(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      ಪ್ರತಿ (sp = BEG_DYN_PRIV_ADDR; sp < END_DYN_PRIV_ADDR; ++sp)
          ಯದಿ (sp->s_proc_nr == NONE) ಭಙ್ಗ;
      ಯದಿ (sp >= END_DYN_PRIV_ADDR) ಪ್ರತ್ಯಾಗಮ(ENOSPC);
  }
  ಅನ್ಯಥಾ {                                    /* allocate slot from id */
      ಯದಿ(!is_static_priv_id(priv_id)) {
          ಪ್ರತ್ಯಾಗಮ EINVAL;                    /* invalid static priv id */
      }
      ಯದಿ(priv[priv_id].s_proc_nr != NONE) {
          ಪ್ರತ್ಯಾಗಮ EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  ಪ್ರತ್ಯಾಗಮ(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
ಶೂನ್ಯ set_sendto_bit(ಧ್ರುವ ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  ಯದಿ (id_to_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_bit(priv(rp)->s_ipc_to, id);
	ಪ್ರತ್ಯಾಗಮ;
  }

  set_sys_bit(priv(rp)->s_ipc_to, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  ಯದಿ (priv_addr(id)->s_trap_mask & ~((1 << RECEIVE)))
      set_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
ಶೂನ್ಯ unset_sendto_bit(ಧ್ರುವ ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_bit(priv(rp)->s_ipc_to, id);

  unset_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
ಶೂನ್ಯ fill_sendto_mask(ಧ್ರುವ ಸಂರಚನಾ proc *rp, sys_map_t *map)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ (i=0; i < NR_SYS_PROCS; i++) {
  	ಯದಿ (get_sys_bit(*map, i))
  		set_sendto_bit(rp, i);
  	ಅನ್ಯಥಾ
  		unset_sendto_bit(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ send_sig(endpoint_t ep, ಪೂರ್ಣಾಙ್ಕ sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp;
  ಸಂರಚನಾ priv *priv;
  ಪೂರ್ಣಾಙ್ಕ proc_nr;

  ಯದಿ(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	ಪ್ರತ್ಯಾಗಮ EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  ಯದಿ(!priv) ಪ್ರತ್ಯಾಗಮ ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
ಶೂನ್ಯ cause_sig(proc_nr_t proc_nr, ಪೂರ್ಣಾಙ್ಕ sig_nr)
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
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  ಪೂರ್ಣಾಙ್ಕ sig_mgr_proc_nr;
  ಪೂರ್ಣಾಙ್ಕ s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  ಯದಿ(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  ಯದಿ(rp->p_endpoint == sig_mgr) {
       ಯದಿ(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           ಯದಿ(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               ಪ್ರತ್ಯಾಗಮ;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       ಯದಿ(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       ಪ್ರತ್ಯಾಗಮ;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  ಯದಿ (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      ಯದಿ (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          ಯದಿ(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
ಶೂನ್ಯ sig_delay_done(ಸಂರಚನಾ proc *rp)
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
  ಸಂರಚನಾ priv *privp;
  endpoint_t ep;

  ಪ್ರತಿ (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; privp++) {
	ಯದಿ (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ clear_memreq(ಸಂರಚನಾ proc *rp)
{
  ಸಂರಚನಾ proc **rpp;

  ಯದಿ (!RTS_ISSET(rp, RTS_VMREQUEST))
	ಪ್ರತ್ಯಾಗಮ; /* nothing to do */

  ಪ್ರತಿ (rpp = &vmrequest; *rpp != ರಿಕ್ತ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ಯದಿ (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		ಭಙ್ಗ;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ clear_ipc(
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  ಸಂರಚನಾ proc **xpp;			/* iterate over caller queue */

  ಯದಿ (RTS_ISSET(rc, RTS_SENDING)) {
      ಪೂರ್ಣಾಙ್ಕ target_proc;

      okendpt(rc->p_sendto_e, &target_proc);
      xpp = &proc_addr(target_proc)->p_caller_q; /* destination's queue */
      ಯಾವತ್ (*xpp) {		/* check entire queue */
          ಯದಿ (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
	      printf("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#endif
              ಭಙ್ಗ;				/* can only be queued once */
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
ಶೂನ್ಯ clear_endpoint(ಸಂರಚನಾ proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  ಯದಿ(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#ಯದಿ DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#endif

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_SET(rc, RTS_NO_ENDPOINT);
  ಯದಿ (priv(rc)->s_flags & SYS_PROC)
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
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rc,		/* slot of process to clean up */
  ಪೂರ್ಣಾಙ್ಕ caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  ಸಂರಚನಾ proc *rp;			/* iterate over process table */
  ಪೂರ್ಣಾಙ್ಕ src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  ಯಾವತ್ ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_to_nr(src_id)), rc);

  ಪ್ರತಿ (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
      ಯದಿ(isemptyp(rp))
	ಅನುವರ್ತ;

      /* Unset pending notification bits. */
      unset_sys_bit(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_bit(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      ಯದಿ (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
ಶೂನ್ಯ kernel_call_resume(ಸಂರಚನಾ proc *caller)
{
	ಪೂರ್ಣಾಙ್ಕ result;

	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_SLOT_FREE));
	ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_VMREQUEST));

	ಪ್ರತಿಜ್ಞಾ(caller->p_vmrequest.saved.reqmsg.m_source == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	result = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_finish(caller, &caller->p_vmrequest.saved.reqmsg, result);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ sched_proc(ಸಂರಚನಾ proc *p, ಪೂರ್ಣಾಙ್ಕ priority, ಪೂರ್ಣಾಙ್ಕ quantum, ಪೂರ್ಣಾಙ್ಕ cpu, ಪೂರ್ಣಾಙ್ಕ niced)
{
	/* Make sure the values given are within the allowed range.*/
	ಯದಿ ((priority < TASK_Q && priority != -1) || priority > NR_SCHED_QUEUES)
		ಪ್ರತ್ಯಾಗಮ(EINVAL);

	ಯದಿ (quantum < 1 && quantum != -1)
		ಪ್ರತ್ಯಾಗಮ(EINVAL);

#ifdef CONFIG_SMP
	ಯದಿ ((cpu < 0 && cpu != -1) || (cpu > 0 && (unsigned) cpu >= ncpus))
		ಪ್ರತ್ಯಾಗಮ(EINVAL);
	ಯದಿ (cpu != -1 && !(cpu_is_ready(cpu)))
		ಪ್ರತ್ಯಾಗಮ EBADCPU;
#endif

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	ಯದಿ (proc_is_runnable(p)) {
#ifdef CONFIG_SMP
		ಯದಿ (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_schedule_migrate_proc(p, cpu);
		}
#endif

		RTS_SET(p, RTS_NO_QUANTUM);
	}

	ಯದಿ (proc_is_runnable(p))
		RTS_SET(p, RTS_NO_QUANTUM);

	ಯದಿ (priority != -1)
		p->p_priority = priority;
	ಯದಿ (quantum != -1) {
		p->p_quantum_size_ms = quantum;
		p->p_cpu_time_ಎಡ = ms_2_cpu_time(quantum);
	}
#ifdef CONFIG_SMP
	ಯದಿ (cpu != -1)
		p->p_cpu = cpu;
#endif

	ಯದಿ (niced)
		p->p_misc_flags |= MF_NICED;
	ಅನ್ಯಥಾ
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_QUANTUM);

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ add_ipc_filter(ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ type, vir_bytes address,
	size_t length)
{
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	ಯದಿ (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	ಯದಿ (length % ಪರಿಮಾಣ(ipc_filter_el_t) != 0)
		ಪ್ರತ್ಯಾಗಮ EINVAL;

	ಅಂಕೆ_elements = length / ಪರಿಮಾಣ(ipc_filter_el_t);
	ಯದಿ (ಅಂಕೆ_elements <= 0 || ಅಂಕೆ_elements > IPCF_MAX_ELEMENTS)
		ಪ್ರತ್ಯಾಗಮ E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_SLOT(type, &ipcf);
	ಯದಿ (ipcf == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ENOMEM;

	/* Fill details. */
	ipcf->ಅಂಕೆ_elements = ಅಂಕೆ_elements;
	ipcf->next = ರಿಕ್ತ;
	r = data_copy(rp->p_endpoint, address,
		KERNEL, (vir_bytes)ipcf->elements, length);
	ಯದಿ (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	ಯದಿ (r != OK) {
		IPCF_POOL_FREE_SLOT(ipcf);
		ಪ್ರತ್ಯಾಗಮ r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	ಪ್ರತಿ (ipcfp = &priv(rp)->s_ipcf; *ipcfp != ರಿಕ್ತ;
	    ipcfp = &(*ipcfp)->next)
		;
	*ipcfp = ipcf;

	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
ಶೂನ್ಯ clear_ipc_filters(ಸಂರಚನಾ proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	ಯಾವತ್ (ipcf != ರಿಕ್ತ) {
		curr_ipcf = ipcf;
		ipcf = ipcf->next;
		IPCF_POOL_FREE_SLOT(curr_ipcf);
	}

	priv(rp)->s_ipcf = ರಿಕ್ತ;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	ಯದಿ (rp->p_endpoint == VM_PROC_NR && vmrequest != ರಿಕ್ತ)
		ಯದಿ (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ check_ipc_filter(ipc_filter_t *ipcf, ಪೂರ್ಣಾಙ್ಕ fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	ಪೂರ್ಣಾಙ್ಕ i, ಅಂಕೆ_elements, flags;

	ಯದಿ (ipcf == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ OK;

	ಅಂಕೆ_elements = ipcf->ಅಂಕೆ_elements;
	flags = 0;
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		ಯದಿ (!IPCF_EL_CHECK(ipcf_el))
			ಪ್ರತ್ಯಾಗಮ EINVAL;
		flags |= ipcf_el->flags;
	}

	ಯದಿ (fill_flags)
		ipcf->flags = flags;
	ಅನ್ಯಥಾ ಯದಿ (ipcf->flags != flags)
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ allow_ipc_filtered_ಸಂದೇಶ(ಸಂರಚನಾ proc *rp, endpoint_t src_e,
	vir_bytes m_src_v, message *m_src_p)
{
	ಪೂರ್ಣಾಙ್ಕ i, r, ಅಂಕೆ_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	message m_buff;

	ipcf = priv(rp)->s_ipcf;
	ಯದಿ (ipcf == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE; /* no IPC filters, always allow */

	ಯದಿ (m_src_p == ರಿಕ್ತ) {
		ಪ್ರತಿಜ್ಞಾ(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		ಕುರು {
#ಯದಿ DEBUG_DUMPIPCF
			ಯದಿ (TRUE) {
#ಅನ್ಯಥಾ
			ಯದಿ (ipcf->flags & IPCF_MATCH_M_TYPE) {
#endif
				get_mtype = TRUE;
				ಭಙ್ಗ;
			}
			ipcf = ipcf->next;
		} ಯಾವತ್ (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		ಯದಿ (get_mtype) {
			r = data_copy(src_e,
			    m_src_v + offsetof(message, m_type), KERNEL,
			    (vir_bytes)&m_buff.m_type, ಪರಿಮಾಣ(m_buff.m_type));
			ಯದಿ (r != OK) {
				/* allow for now, this will fail later anyway */
#ಯದಿ DEBUG_DUMPIPCF
				printf("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#endif
				ಪ್ರತ್ಯಾಗಮ TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_source = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	ಕುರು {
		ಯದಿ (allow != (ipcf->type == IPCF_WHITELIST)) {
			ಅಂಕೆ_elements = ipcf->ಅಂಕೆ_elements;
			ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				ಯದಿ (IPCF_EL_MATCH(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					ಭಙ್ಗ;
				}
			}
		}
		ipcf = ipcf->next;
	} ಯಾವತ್ (ipcf);

#ಯದಿ DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#endif

	ಪ್ರತ್ಯಾಗಮ allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ allow_ipc_filtered_memreq(ಸಂರಚನಾ proc *src_rp, ಸಂರಚನಾ proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	ಸಂರಚನಾ proc *vmp;
	message m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	ಯದಿ (priv(vmp)->s_ipcf == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_MESSAGE;
	ಯದಿ (!allow_ipc_filtered_ಸಂದೇಶ(vmp, SYSTEM, 0, &m_buf))
		ಪ್ರತ್ಯಾಗಮ FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ priv_add_irq(ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ irq)
{
        ಸಂರಚನಾ priv *priv = priv(rp);
        ಪೂರ್ಣಾಙ್ಕ i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	ಪ್ರತಿ (i = 0; i < priv->s_nr_irq; i++) {
		ಯದಿ (priv->s_irq_tab[i] == irq)
			ಪ್ರತ್ಯಾಗಮ OK;
	}

	i= priv->s_nr_irq;
	ಯದಿ (i >= NR_IRQ) {
		printf("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		ಪ್ರತ್ಯಾಗಮ ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ priv_add_io(ಸಂರಚನಾ proc *rp, ಸಂರಚನಾ io_range *ior)
{
        ಸಂರಚನಾ priv *priv = priv(rp);
        ಪೂರ್ಣಾಙ್ಕ i;

	priv->s_flags |= CHECK_IO_PORT;	/* Check I/O accesses */

	ಪ್ರತಿ (i = 0; i < priv->s_nr_io_range; i++) {
		ಯದಿ (priv->s_io_tab[i].ior_base == ior->ior_base &&
			priv->s_io_tab[i].ior_limit == ior->ior_limit)
			ಪ್ರತ್ಯಾಗಮ OK;
	}

	i= priv->s_nr_io_range;
	ಯದಿ (i >= NR_IO_RANGE) {
		printf("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		ಪ್ರತ್ಯಾಗಮ ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	ಪ್ರತ್ಯಾಗಮ OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ priv_add_mem(ಸಂರಚನಾ proc *rp, ಸಂರಚನಾ minix_mem_range *memr)
{
        ಸಂರಚನಾ priv *priv = priv(rp);
        ಪೂರ್ಣಾಙ್ಕ i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	ಪ್ರತಿ (i = 0; i < priv->s_nr_mem_range; i++) {
		ಯದಿ (priv->s_mem_tab[i].mr_base == memr->mr_base &&
			priv->s_mem_tab[i].mr_limit == memr->mr_limit)
			ಪ್ರತ್ಯಾಗಮ OK;
	}

	i= priv->s_nr_mem_range;
	ಯದಿ (i >= NR_MEM_RANGE) {
		printf("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		ಪ್ರತ್ಯಾಗಮ ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	ಪ್ರತ್ಯಾಗಮ OK;
}

