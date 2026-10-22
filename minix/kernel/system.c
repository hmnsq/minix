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

#ضمن "kernel/system.h"
#ضمن "kernel/vm.h"
#ضمن "kernel/clock.h"
#ضمن <stdlib.h>
#ضمن <stddef.h>
#ضمن <assert.h>
#ضمن <signal.h>
#ضمن <unistd.h>
#ضمن <minix/endpoint.h>
#ضمن <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
ساكن صحيح (*call_vec[NR_SYS_CALLS])(بنية proc * caller, message *m_ptr);

#define map(call_nr, handler) 					\
    {	صحيح call_index = call_nr-KERNEL_CALL; 				\
    	أكد(call_index >= 0 && call_index < NR_SYS_CALLS);			\
    call_vec[call_index] = (handler)  ; }

ساكن فراغ kernel_call_finish(بنية proc * caller, message *msg, صحيح result)
{
  ܐܢ(result == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  أكد(RTS_ISSET(caller, RTS_VMREQUEST));
	  أكد(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *msg;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } ܐܚܪܢܐ {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_source = NONE;
	  ܐܢ (result != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  msg->m_source = SYSTEM;
		  msg->m_type = result;		/* report status of call */
#ܐܢ DEBUG_IPC_HOOK
	hook_ipc_msgkresult(msg, caller);
#endif
		  ܐܢ (copy_msg_to_user(msg, (message *)caller->p_delivermsg_vir)) {
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

ساكن صحيح kernel_call_dispatch(بنية proc * caller, message *msg)
{
  صحيح result = OK;
  صحيح call_nr;

#ܐܢ DEBUG_IPC_HOOK
	hook_ipc_msgkcall(msg, caller);
#endif
  call_nr = msg->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  ܐܢ (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  printf("SYSTEM: illegal request %d from %d.\n",
			  call_nr,msg->m_source);
	  result = EBADREQUEST;			/* illegal message type */
  }
  ܐܚܪܢܐ ܐܢ (!GET_BIT(priv(caller)->s_k_call_mask, call_nr)) {
	  printf("SYSTEM: denied request %d from %d.\n",
			  call_nr,msg->m_source);
	  result = ECALLDENIED;			/* illegal message type */
  } ܐܚܪܢܐ {
	  /* handle the system call */
	  ܐܢ (call_vec[call_nr])
		  result = (*call_vec[call_nr])(caller, msg);
	  ܐܚܪܢܐ {
		  printf("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  result = EBADREQUEST;
	  }
  }

  ܦܢܐ result;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
فراغ kernel_call(message *m_user, بنية proc * caller)
{
  صحيح result = OK;
  message msg;

  caller->p_delivermsg_vir = (vir_bytes) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  ܐܢ (copy_msg_from_user(m_user, &msg) == 0) {
	  msg.m_source = caller->p_endpoint;
	  result = kernel_call_dispatch(caller, &msg);
  }
  ܐܚܪܢܐ {
	  printf("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  ܦܢܐ;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_finish(caller, &msg, result);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
فراغ system_init(فراغ)
{
  سجل بنية priv *sp;
  صحيح i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  لكل (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  لكل (sp=BEG_PRIV_ADDR; sp < END_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  لكل (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = عدم;
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
#ܐܢ defined(__i386__)
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
#ܐܢ defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#endif

  /* i386-specific. */
#ܐܢ defined(__i386__)
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
صحيح get_priv(
  سجل بنية proc *rc,		/* new (child) process pointer */
  صحيح priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  سجل بنية priv *sp;                 /* privilege structure */

  ܐܢ(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      لكل (sp = BEG_DYN_PRIV_ADDR; sp < END_DYN_PRIV_ADDR; ++sp)
          ܐܢ (sp->s_proc_nr == NONE) اكسر;
      ܐܢ (sp >= END_DYN_PRIV_ADDR) ܦܢܐ(ENOSPC);
  }
  ܐܚܪܢܐ {                                    /* allocate slot from id */
      ܐܢ(!is_static_priv_id(priv_id)) {
          ܦܢܐ EINVAL;                    /* invalid static priv id */
      }
      ܐܢ(priv[priv_id].s_proc_nr != NONE) {
          ܦܢܐ EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  ܦܢܐ(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
فراغ set_sendto_bit(ثابت بنية proc *rp, صحيح id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  ܐܢ (id_to_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_bit(priv(rp)->s_ipc_to, id);
	ܦܢܐ;
  }

  set_sys_bit(priv(rp)->s_ipc_to, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  ܐܢ (priv_addr(id)->s_trap_mask & ~((1 << RECEIVE)))
      set_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
فراغ unset_sendto_bit(ثابت بنية proc *rp, صحيح id)
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
فراغ fill_sendto_mask(ثابت بنية proc *rp, sys_map_t *map)
{
  صحيح i;

  لكل (i=0; i < NR_SYS_PROCS; i++) {
  	ܐܢ (get_sys_bit(*map, i))
  		set_sendto_bit(rp, i);
  	ܐܚܪܢܐ
  		unset_sendto_bit(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
صحيح send_sig(endpoint_t ep, صحيح sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  سجل بنية proc *rp;
  بنية priv *priv;
  صحيح proc_nr;

  ܐܢ(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	ܦܢܐ EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  ܐܢ(!priv) ܦܢܐ ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  ܦܢܐ OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
فراغ cause_sig(proc_nr_t proc_nr, صحيح sig_nr)
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
  سجل بنية proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  صحيح sig_mgr_proc_nr;
  صحيح s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  ܐܢ(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  ܐܢ(rp->p_endpoint == sig_mgr) {
       ܐܢ(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           ܐܢ(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               ܦܢܐ;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       ܐܢ(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       ܦܢܐ;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  ܐܢ (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      ܐܢ (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          ܐܢ(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
فراغ sig_delay_done(بنية proc *rp)
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
فراغ send_diag_sig(فراغ)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  بنية priv *privp;
  endpoint_t ep;

  لكل (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; privp++) {
	ܐܢ (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
ساكن فراغ clear_memreq(بنية proc *rp)
{
  بنية proc **rpp;

  ܐܢ (!RTS_ISSET(rp, RTS_VMREQUEST))
	ܦܢܐ; /* nothing to do */

  لكل (rpp = &vmrequest; *rpp != عدم;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ܐܢ (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		اكسر;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
ساكن فراغ clear_ipc(
  سجل بنية proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  بنية proc **xpp;			/* iterate over caller queue */

  ܐܢ (RTS_ISSET(rc, RTS_SENDING)) {
      صحيح target_proc;

      okendpt(rc->p_sendto_e, &target_proc);
      xpp = &proc_addr(target_proc)->p_caller_q; /* destination's queue */
      ܟܡܐ (*xpp) {		/* check entire queue */
          ܐܢ (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#ܐܢ DEBUG_ENABLE_IPC_WARNINGS
	      printf("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#endif
              اكسر;				/* can only be queued once */
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
فراغ clear_endpoint(بنية proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  ܐܢ(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#ܐܢ DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#endif

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_SET(rc, RTS_NO_ENDPOINT);
  ܐܢ (priv(rc)->s_flags & SYS_PROC)
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
فراغ clear_ipc_refs(
  سجل بنية proc *rc,		/* slot of process to clean up */
  صحيح caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  بنية proc *rp;			/* iterate over process table */
  صحيح src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  ܟܡܐ ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_to_nr(src_id)), rc);

  لكل (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
      ܐܢ(isemptyp(rp))
	استمر;

      /* Unset pending notification bits. */
      unset_sys_bit(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_bit(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      ܐܢ (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
فراغ kernel_call_resume(بنية proc *caller)
{
	صحيح result;

	أكد(!RTS_ISSET(caller, RTS_SLOT_FREE));
	أكد(!RTS_ISSET(caller, RTS_VMREQUEST));

	أكد(caller->p_vmrequest.saved.reqmsg.m_source == caller->p_endpoint);

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
صحيح sched_proc(بنية proc *p, صحيح priority, صحيح quantum, صحيح cpu, صحيح niced)
{
	/* Make sure the values given are within the allowed range.*/
	ܐܢ ((priority < TASK_Q && priority != -1) || priority > NR_SCHED_QUEUES)
		ܦܢܐ(EINVAL);

	ܐܢ (quantum < 1 && quantum != -1)
		ܦܢܐ(EINVAL);

#ifdef CONFIG_SMP
	ܐܢ ((cpu < 0 && cpu != -1) || (cpu > 0 && (unsigned) cpu >= ncpus))
		ܦܢܐ(EINVAL);
	ܐܢ (cpu != -1 && !(cpu_is_ready(cpu)))
		ܦܢܐ EBADCPU;
#endif

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	ܐܢ (proc_is_runnable(p)) {
#ifdef CONFIG_SMP
		ܐܢ (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_schedule_migrate_proc(p, cpu);
		}
#endif

		RTS_SET(p, RTS_NO_QUANTUM);
	}

	ܐܢ (proc_is_runnable(p))
		RTS_SET(p, RTS_NO_QUANTUM);

	ܐܢ (priority != -1)
		p->p_priority = priority;
	ܐܢ (quantum != -1) {
		p->p_quantum_size_ms = quantum;
		p->p_cpu_time_left = ms_2_cpu_time(quantum);
	}
#ifdef CONFIG_SMP
	ܐܢ (cpu != -1)
		p->p_cpu = cpu;
#endif

	ܐܢ (niced)
		p->p_misc_flags |= MF_NICED;
	ܐܚܪܢܐ
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_QUANTUM);

	ܦܢܐ OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
صحيح add_ipc_filter(بنية proc *rp, صحيح type, vir_bytes address,
	size_t length)
{
	صحيح num_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	ܐܢ (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		ܦܢܐ EINVAL;

	ܐܢ (length % حجم(ipc_filter_el_t) != 0)
		ܦܢܐ EINVAL;

	num_elements = length / حجم(ipc_filter_el_t);
	ܐܢ (num_elements <= 0 || num_elements > IPCF_MAX_ELEMENTS)
		ܦܢܐ E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_SLOT(type, &ipcf);
	ܐܢ (ipcf == عدم)
		ܦܢܐ ENOMEM;

	/* Fill details. */
	ipcf->num_elements = num_elements;
	ipcf->next = عدم;
	r = data_copy(rp->p_endpoint, address,
		KERNEL, (vir_bytes)ipcf->elements, length);
	ܐܢ (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	ܐܢ (r != OK) {
		IPCF_POOL_FREE_SLOT(ipcf);
		ܦܢܐ r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	لكل (ipcfp = &priv(rp)->s_ipcf; *ipcfp != عدم;
	    ipcfp = &(*ipcfp)->next)
		;
	*ipcfp = ipcf;

	ܦܢܐ OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
فراغ clear_ipc_filters(بنية proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	ܟܡܐ (ipcf != عدم) {
		curr_ipcf = ipcf;
		ipcf = ipcf->next;
		IPCF_POOL_FREE_SLOT(curr_ipcf);
	}

	priv(rp)->s_ipcf = عدم;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	ܐܢ (rp->p_endpoint == VM_PROC_NR && vmrequest != عدم)
		ܐܢ (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
صحيح check_ipc_filter(ipc_filter_t *ipcf, صحيح fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	صحيح i, num_elements, flags;

	ܐܢ (ipcf == عدم)
		ܦܢܐ OK;

	num_elements = ipcf->num_elements;
	flags = 0;
	لكل (i = 0; i < num_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		ܐܢ (!IPCF_EL_CHECK(ipcf_el))
			ܦܢܐ EINVAL;
		flags |= ipcf_el->flags;
	}

	ܐܢ (fill_flags)
		ipcf->flags = flags;
	ܐܚܪܢܐ ܐܢ (ipcf->flags != flags)
		ܦܢܐ EINVAL;
	ܦܢܐ OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
صحيح allow_ipc_filtered_msg(بنية proc *rp, endpoint_t src_e,
	vir_bytes m_src_v, message *m_src_p)
{
	صحيح i, r, num_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	message m_buff;

	ipcf = priv(rp)->s_ipcf;
	ܐܢ (ipcf == عدم)
		ܦܢܐ TRUE; /* no IPC filters, always allow */

	ܐܢ (m_src_p == عدم) {
		أكد(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		ܥܒܕ {
#ܐܢ DEBUG_DUMPIPCF
			ܐܢ (TRUE) {
#ܐܚܪܢܐ
			ܐܢ (ipcf->flags & IPCF_MATCH_M_TYPE) {
#endif
				get_mtype = TRUE;
				اكسر;
			}
			ipcf = ipcf->next;
		} ܟܡܐ (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		ܐܢ (get_mtype) {
			r = data_copy(src_e,
			    m_src_v + offsetof(message, m_type), KERNEL,
			    (vir_bytes)&m_buff.m_type, حجم(m_buff.m_type));
			ܐܢ (r != OK) {
				/* allow for now, this will fail later anyway */
#ܐܢ DEBUG_DUMPIPCF
				printf("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#endif
				ܦܢܐ TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_source = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	ܥܒܕ {
		ܐܢ (allow != (ipcf->type == IPCF_WHITELIST)) {
			num_elements = ipcf->num_elements;
			لكل (i = 0; i < num_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				ܐܢ (IPCF_EL_MATCH(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					اكسر;
				}
			}
		}
		ipcf = ipcf->next;
	} ܟܡܐ (ipcf);

#ܐܢ DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#endif

	ܦܢܐ allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
صحيح allow_ipc_filtered_memreq(بنية proc *src_rp, بنية proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	بنية proc *vmp;
	message m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	ܐܢ (priv(vmp)->s_ipcf == عدم)
		ܦܢܐ TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_MESSAGE;
	ܐܢ (!allow_ipc_filtered_msg(vmp, SYSTEM, 0, &m_buf))
		ܦܢܐ FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	ܦܢܐ TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
صحيح priv_add_irq(بنية proc *rp, صحيح irq)
{
        بنية priv *priv = priv(rp);
        صحيح i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	لكل (i = 0; i < priv->s_nr_irq; i++) {
		ܐܢ (priv->s_irq_tab[i] == irq)
			ܦܢܐ OK;
	}

	i= priv->s_nr_irq;
	ܐܢ (i >= NR_IRQ) {
		printf("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		ܦܢܐ ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	ܦܢܐ OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
صحيح priv_add_io(بنية proc *rp, بنية io_range *ior)
{
        بنية priv *priv = priv(rp);
        صحيح i;

	priv->s_flags |= CHECK_IO_PORT;	/* Check I/O accesses */

	لكل (i = 0; i < priv->s_nr_io_range; i++) {
		ܐܢ (priv->s_io_tab[i].ior_base == ior->ior_base &&
			priv->s_io_tab[i].ior_limit == ior->ior_limit)
			ܦܢܐ OK;
	}

	i= priv->s_nr_io_range;
	ܐܢ (i >= NR_IO_RANGE) {
		printf("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		ܦܢܐ ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	ܦܢܐ OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
صحيح priv_add_mem(بنية proc *rp, بنية minix_mem_range *memr)
{
        بنية priv *priv = priv(rp);
        صحيح i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	لكل (i = 0; i < priv->s_nr_mem_range; i++) {
		ܐܢ (priv->s_mem_tab[i].mr_base == memr->mr_base &&
			priv->s_mem_tab[i].mr_limit == memr->mr_limit)
			ܦܢܐ OK;
	}

	i= priv->s_nr_mem_range;
	ܐܢ (i >= NR_MEM_RANGE) {
		printf("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		ܦܢܐ ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	ܦܢܐ OK;
}

