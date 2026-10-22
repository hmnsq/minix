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

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব "kernel/vm.h"
#অন্তর্ভাব "kernel/clock.h"
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stddef.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <minix/endpoint.h>
#অন্তর্ভাব <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
স্থির পূর্ণসংখ্যা (*call_vec[NR_SYS_CALLS])(কাঠামো proc * caller, message *m_ptr);

#define map(call_nr, handler) 					\
    {	পূর্ণসংখ্যা call_সূচক = call_nr-KERNEL_CALL; 				\
    	প্রতিজ্ঞা(call_সূচক >= 0 && call_সূচক < NR_SYS_CALLS);			\
    call_vec[call_সূচক] = (handler)  ; }

স্থির শূন্য kernel_call_finish(কাঠামো proc * caller, message *বার্তা, পূর্ণসংখ্যা ফলাফল)
{
  যদি(ফলাফল == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  প্রতিজ্ঞা(RTS_ISSET(caller, RTS_VMREQUEST));
	  প্রতিজ্ঞা(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *বার্তা;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } অন্যথা {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_source = NONE;
	  যদি (ফলাফল != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  বার্তা->m_source = SYSTEM;
		  বার্তা->m_type = ফলাফল;		/* report status of call */
#যদি DEBUG_IPC_HOOK
	hook_ipc_msgkresult(বার্তা, caller);
#endif
		  যদি (copy_বার্তা_to_user(বার্তা, (message *)caller->p_delivermsg_vir)) {
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

স্থির পূর্ণসংখ্যা kernel_call_dispatch(কাঠামো proc * caller, message *বার্তা)
{
  পূর্ণসংখ্যা ফলাফল = OK;
  পূর্ণসংখ্যা call_nr;

#যদি DEBUG_IPC_HOOK
	hook_ipc_msgkcall(বার্তা, caller);
#endif
  call_nr = বার্তা->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  যদি (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  printf("SYSTEM: illegal request %d from %d.\n",
			  call_nr,বার্তা->m_source);
	  ফলাফল = EBADREQUEST;			/* illegal message type */
  }
  অন্যথা যদি (!GET_BIT(priv(caller)->s_k_call_mask, call_nr)) {
	  printf("SYSTEM: denied request %d from %d.\n",
			  call_nr,বার্তা->m_source);
	  ফলাফল = ECALLDENIED;			/* illegal message type */
  } অন্যথা {
	  /* handle the system call */
	  যদি (call_vec[call_nr])
		  ফলাফল = (*call_vec[call_nr])(caller, বার্তা);
	  অন্যথা {
		  printf("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  ফলাফল = EBADREQUEST;
	  }
  }

  ফেরত ফলাফল;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
শূন্য kernel_call(message *m_user, কাঠামো proc * caller)
{
  পূর্ণসংখ্যা ফলাফল = OK;
  message বার্তা;

  caller->p_delivermsg_vir = (vir_bytes) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  যদি (copy_বার্তা_from_user(m_user, &বার্তা) == 0) {
	  বার্তা.m_source = caller->p_endpoint;
	  ফলাফল = kernel_call_dispatch(caller, &বার্তা);
  }
  অন্যথা {
	  printf("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  ফেরত;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_finish(caller, &বার্তা, ফলাফল);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
শূন্য system_init(শূন্য)
{
  নিবন্ধ কাঠামো priv *sp;
  পূর্ণসংখ্যা i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  জন্য (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  জন্য (sp=BEG_PRIV_ADDR; sp < END_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  জন্য (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = ফাঁকা;
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
#যদি defined(__i386__)
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
#যদি defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#endif

  /* i386-specific. */
#যদি defined(__i386__)
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
পূর্ণসংখ্যা get_priv(
  নিবন্ধ কাঠামো proc *rc,		/* new (child) process pointer */
  পূর্ণসংখ্যা priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  নিবন্ধ কাঠামো priv *sp;                 /* privilege structure */

  যদি(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      জন্য (sp = BEG_DYN_PRIV_ADDR; sp < END_DYN_PRIV_ADDR; ++sp)
          যদি (sp->s_proc_nr == NONE) বিরতি;
      যদি (sp >= END_DYN_PRIV_ADDR) ফেরত(ENOSPC);
  }
  অন্যথা {                                    /* allocate slot from id */
      যদি(!is_static_priv_id(priv_id)) {
          ফেরত EINVAL;                    /* invalid static priv id */
      }
      যদি(priv[priv_id].s_proc_nr != NONE) {
          ফেরত EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  ফেরত(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
শূন্য set_sendto_bit(ধ্রুবক কাঠামো proc *rp, পূর্ণসংখ্যা id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  যদি (id_to_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_bit(priv(rp)->s_ipc_to, id);
	ফেরত;
  }

  set_sys_bit(priv(rp)->s_ipc_to, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  যদি (priv_addr(id)->s_trap_mask & ~((1 << RECEIVE)))
      set_sys_bit(priv_addr(id)->s_ipc_to, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
শূন্য unset_sendto_bit(ধ্রুবক কাঠামো proc *rp, পূর্ণসংখ্যা id)
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
শূন্য fill_sendto_mask(ধ্রুবক কাঠামো proc *rp, sys_map_t *map)
{
  পূর্ণসংখ্যা i;

  জন্য (i=0; i < NR_SYS_PROCS; i++) {
  	যদি (get_sys_bit(*map, i))
  		set_sendto_bit(rp, i);
  	অন্যথা
  		unset_sendto_bit(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
পূর্ণসংখ্যা send_sig(endpoint_t ep, পূর্ণসংখ্যা sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  নিবন্ধ কাঠামো proc *rp;
  কাঠামো priv *priv;
  পূর্ণসংখ্যা proc_nr;

  যদি(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	ফেরত EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  যদি(!priv) ফেরত ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  ফেরত OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
শূন্য cause_sig(proc_nr_t proc_nr, পূর্ণসংখ্যা sig_nr)
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
  নিবন্ধ কাঠামো proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  পূর্ণসংখ্যা sig_mgr_proc_nr;
  পূর্ণসংখ্যা s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  যদি(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  যদি(rp->p_endpoint == sig_mgr) {
       যদি(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           যদি(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               ফেরত;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       যদি(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       ফেরত;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  যদি (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      যদি (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_SET(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          যদি(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
শূন্য sig_delay_done(কাঠামো proc *rp)
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
শূন্য send_diag_sig(শূন্য)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  কাঠামো priv *privp;
  endpoint_t ep;

  জন্য (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; privp++) {
	যদি (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
স্থির শূন্য clear_memreq(কাঠামো proc *rp)
{
  কাঠামো proc **rpp;

  যদি (!RTS_ISSET(rp, RTS_VMREQUEST))
	ফেরত; /* nothing to do */

  জন্য (rpp = &vmrequest; *rpp != ফাঁকা;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	যদি (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		বিরতি;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
স্থির শূন্য clear_ipc(
  নিবন্ধ কাঠামো proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  কাঠামো proc **xpp;			/* iterate over caller queue */

  যদি (RTS_ISSET(rc, RTS_SENDING)) {
      পূর্ণসংখ্যা target_proc;

      okendpt(rc->p_sendto_e, &target_proc);
      xpp = &proc_addr(target_proc)->p_caller_q; /* destination's queue */
      যখন (*xpp) {		/* check entire queue */
          যদি (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#যদি DEBUG_ENABLE_IPC_WARNINGS
	      printf("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#endif
              বিরতি;				/* can only be queued once */
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
শূন্য clear_endpoint(কাঠামো proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  যদি(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#যদি DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#endif

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_SET(rc, RTS_NO_ENDPOINT);
  যদি (priv(rc)->s_flags & SYS_PROC)
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
শূন্য clear_ipc_refs(
  নিবন্ধ কাঠামো proc *rc,		/* slot of process to clean up */
  পূর্ণসংখ্যা caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  কাঠামো proc *rp;			/* iterate over process table */
  পূর্ণসংখ্যা src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  যখন ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_to_nr(src_id)), rc);

  জন্য (rp = BEG_PROC_ADDR; rp < END_PROC_ADDR; rp++) {
      যদি(isemptyp(rp))
	চালু;

      /* Unset pending notification bits. */
      unset_sys_bit(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_bit(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      যদি (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
শূন্য kernel_call_resume(কাঠামো proc *caller)
{
	পূর্ণসংখ্যা ফলাফল;

	প্রতিজ্ঞা(!RTS_ISSET(caller, RTS_SLOT_FREE));
	প্রতিজ্ঞা(!RTS_ISSET(caller, RTS_VMREQUEST));

	প্রতিজ্ঞা(caller->p_vmrequest.saved.reqmsg.m_source == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	ফলাফল = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_finish(caller, &caller->p_vmrequest.saved.reqmsg, ফলাফল);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
পূর্ণসংখ্যা sched_proc(কাঠামো proc *p, পূর্ণসংখ্যা priority, পূর্ণসংখ্যা quantum, পূর্ণসংখ্যা cpu, পূর্ণসংখ্যা niced)
{
	/* Make sure the values given are within the allowed range.*/
	যদি ((priority < TASK_Q && priority != -1) || priority > NR_SCHED_QUEUES)
		ফেরত(EINVAL);

	যদি (quantum < 1 && quantum != -1)
		ফেরত(EINVAL);

#ifdef CONFIG_SMP
	যদি ((cpu < 0 && cpu != -1) || (cpu > 0 && (unsigned) cpu >= ncpus))
		ফেরত(EINVAL);
	যদি (cpu != -1 && !(cpu_is_ready(cpu)))
		ফেরত EBADCPU;
#endif

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	যদি (proc_is_runnable(p)) {
#ifdef CONFIG_SMP
		যদি (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_schedule_migrate_proc(p, cpu);
		}
#endif

		RTS_SET(p, RTS_NO_QUANTUM);
	}

	যদি (proc_is_runnable(p))
		RTS_SET(p, RTS_NO_QUANTUM);

	যদি (priority != -1)
		p->p_priority = priority;
	যদি (quantum != -1) {
		p->p_quantum_আকার_ms = quantum;
		p->p_cpu_time_বাম = ms_2_cpu_time(quantum);
	}
#ifdef CONFIG_SMP
	যদি (cpu != -1)
		p->p_cpu = cpu;
#endif

	যদি (niced)
		p->p_misc_flags |= MF_NICED;
	অন্যথা
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_QUANTUM);

	ফেরত OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
পূর্ণসংখ্যা add_ipc_filter(কাঠামো proc *rp, পূর্ণসংখ্যা type, vir_bytes address,
	size_t দৈর্ঘ্য)
{
	পূর্ণসংখ্যা অঙ্ক_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	যদি (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		ফেরত EINVAL;

	যদি (দৈর্ঘ্য % আকার(ipc_filter_el_t) != 0)
		ফেরত EINVAL;

	অঙ্ক_elements = দৈর্ঘ্য / আকার(ipc_filter_el_t);
	যদি (অঙ্ক_elements <= 0 || অঙ্ক_elements > IPCF_MAX_ELEMENTS)
		ফেরত E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_SLOT(type, &ipcf);
	যদি (ipcf == ফাঁকা)
		ফেরত ENOMEM;

	/* Fill details. */
	ipcf->অঙ্ক_elements = অঙ্ক_elements;
	ipcf->পরবর্তী = ফাঁকা;
	r = তথ্য_copy(rp->p_endpoint, address,
		KERNEL, (vir_bytes)ipcf->elements, দৈর্ঘ্য);
	যদি (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	যদি (r != OK) {
		IPCF_POOL_FREE_SLOT(ipcf);
		ফেরত r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	জন্য (ipcfp = &priv(rp)->s_ipcf; *ipcfp != ফাঁকা;
	    ipcfp = &(*ipcfp)->পরবর্তী)
		;
	*ipcfp = ipcf;

	ফেরত OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
শূন্য clear_ipc_filters(কাঠামো proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	যখন (ipcf != ফাঁকা) {
		curr_ipcf = ipcf;
		ipcf = ipcf->পরবর্তী;
		IPCF_POOL_FREE_SLOT(curr_ipcf);
	}

	priv(rp)->s_ipcf = ফাঁকা;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	যদি (rp->p_endpoint == VM_PROC_NR && vmrequest != ফাঁকা)
		যদি (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
পূর্ণসংখ্যা check_ipc_filter(ipc_filter_t *ipcf, পূর্ণসংখ্যা fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	পূর্ণসংখ্যা i, অঙ্ক_elements, flags;

	যদি (ipcf == ফাঁকা)
		ফেরত OK;

	অঙ্ক_elements = ipcf->অঙ্ক_elements;
	flags = 0;
	জন্য (i = 0; i < অঙ্ক_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		যদি (!IPCF_EL_CHECK(ipcf_el))
			ফেরত EINVAL;
		flags |= ipcf_el->flags;
	}

	যদি (fill_flags)
		ipcf->flags = flags;
	অন্যথা যদি (ipcf->flags != flags)
		ফেরত EINVAL;
	ফেরত OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
পূর্ণসংখ্যা allow_ipc_filtered_বার্তা(কাঠামো proc *rp, endpoint_t src_e,
	vir_bytes m_src_v, message *m_src_p)
{
	পূর্ণসংখ্যা i, r, অঙ্ক_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	message m_buff;

	ipcf = priv(rp)->s_ipcf;
	যদি (ipcf == ফাঁকা)
		ফেরত TRUE; /* no IPC filters, always allow */

	যদি (m_src_p == ফাঁকা) {
		প্রতিজ্ঞা(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		করো {
#যদি DEBUG_DUMPIPCF
			যদি (TRUE) {
#অন্যথা
			যদি (ipcf->flags & IPCF_MATCH_M_TYPE) {
#endif
				get_mtype = TRUE;
				বিরতি;
			}
			ipcf = ipcf->পরবর্তী;
		} যখন (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		যদি (get_mtype) {
			r = তথ্য_copy(src_e,
			    m_src_v + offsetof(message, m_type), KERNEL,
			    (vir_bytes)&m_buff.m_type, আকার(m_buff.m_type));
			যদি (r != OK) {
				/* allow for now, this will fail later anyway */
#যদি DEBUG_DUMPIPCF
				printf("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#endif
				ফেরত TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_source = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	করো {
		যদি (allow != (ipcf->type == IPCF_WHITELIST)) {
			অঙ্ক_elements = ipcf->অঙ্ক_elements;
			জন্য (i = 0; i < অঙ্ক_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				যদি (IPCF_EL_MATCH(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					বিরতি;
				}
			}
		}
		ipcf = ipcf->পরবর্তী;
	} যখন (ipcf);

#যদি DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#endif

	ফেরত allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
পূর্ণসংখ্যা allow_ipc_filtered_memreq(কাঠামো proc *src_rp, কাঠামো proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	কাঠামো proc *vmp;
	message m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	যদি (priv(vmp)->s_ipcf == ফাঁকা)
		ফেরত TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_MESSAGE;
	যদি (!allow_ipc_filtered_বার্তা(vmp, SYSTEM, 0, &m_buf))
		ফেরত FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	ফেরত TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
পূর্ণসংখ্যা priv_add_irq(কাঠামো proc *rp, পূর্ণসংখ্যা irq)
{
        কাঠামো priv *priv = priv(rp);
        পূর্ণসংখ্যা i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	জন্য (i = 0; i < priv->s_nr_irq; i++) {
		যদি (priv->s_irq_tab[i] == irq)
			ফেরত OK;
	}

	i= priv->s_nr_irq;
	যদি (i >= NR_IRQ) {
		printf("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		ফেরত ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	ফেরত OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
পূর্ণসংখ্যা priv_add_io(কাঠামো proc *rp, কাঠামো io_range *ior)
{
        কাঠামো priv *priv = priv(rp);
        পূর্ণসংখ্যা i;

	priv->s_flags |= CHECK_IO_PORT;	/* Check I/O accesses */

	জন্য (i = 0; i < priv->s_nr_io_range; i++) {
		যদি (priv->s_io_tab[i].ior_base == ior->ior_base &&
			priv->s_io_tab[i].ior_limit == ior->ior_limit)
			ফেরত OK;
	}

	i= priv->s_nr_io_range;
	যদি (i >= NR_IO_RANGE) {
		printf("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		ফেরত ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	ফেরত OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
পূর্ণসংখ্যা priv_add_mem(কাঠামো proc *rp, কাঠামো minix_mem_range *memr)
{
        কাঠামো priv *priv = priv(rp);
        পূর্ণসংখ্যা i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	জন্য (i = 0; i < priv->s_nr_mem_range; i++) {
		যদি (priv->s_mem_tab[i].mr_base == memr->mr_base &&
			priv->s_mem_tab[i].mr_limit == memr->mr_limit)
			ফেরত OK;
	}

	i= priv->s_nr_mem_range;
	যদি (i >= NR_MEM_RANGE) {
		printf("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		ফেরত ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	ফেরত OK;
}

