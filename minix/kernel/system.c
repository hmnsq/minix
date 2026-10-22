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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ "kernel/clock.h"
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stddef.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
స్థిర పూర్ణాంకం (*call_vec[NR_SYS_CALLS])(నిర్మాణం proc * caller, సందేశం *m_ptr);

#నిర్వచించు map(call_nr, handler) 					\
    {	పూర్ణాంకం call_సూచీ = call_nr-KERNEL_CALL; 				\
    	ప్రతిజ్ఞా(call_సూచీ >= 0 && call_సూచీ < NR_SYS_CALLS);			\
    call_vec[call_సూచీ] = (handler)  ; }

స్థిర శూన్య kernel_call_ముగింపు(నిర్మాణం proc * caller, సందేశం *సందేశం_2, పూర్ణాంకం ఫలితం)
{
  ఒకవేళ(ఫలితం == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  ప్రతిజ్ఞా(RTS_ISSET(caller, RTS_VMREQUEST));
	  ప్రతిజ్ఞా(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *సందేశం_2;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } లేకపోతే {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_ఆధారం = NONE;
	  ఒకవేళ (ఫలితం != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  సందేశం_2->m_ఆధారం = SYSTEM;
		  సందేశం_2->m_type = ఫలితం;		/* report status of call */
#ఒకవేళ DEBUG_IPC_HOOK
	hook_ipc_msgkresult(సందేశం_2, caller);
#అయితే_ముగింపు
		  ఒకవేళ (copy_సందేశం_గమ్యం_user(సందేశం_2, (సందేశం *)caller->p_delivermsg_vir)) {
			  రాయి("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

స్థిర పూర్ణాంకం kernel_call_dispatch(నిర్మాణం proc * caller, సందేశం *సందేశం_2)
{
  పూర్ణాంకం ఫలితం = OK;
  పూర్ణాంకం call_nr;

#ఒకవేళ DEBUG_IPC_HOOK
	hook_ipc_msgkcall(సందేశం_2, caller);
#అయితే_ముగింపు
  call_nr = సందేశం_2->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  ఒకవేళ (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  రాయి("SYSTEM: illegal request %d from %d.\n",
			  call_nr,సందేశం_2->m_ఆధారం);
	  ఫలితం = EBADREQUEST;			/* illegal message type */
  }
  లేకపోతే ఒకవేళ (!GET_బిట్టు(priv(caller)->s_k_call_ముసుగు, call_nr)) {
	  రాయి("SYSTEM: denied request %d from %d.\n",
			  call_nr,సందేశం_2->m_ఆధారం);
	  ఫలితం = ECALLDENIED;			/* illegal message type */
  } లేకపోతే {
	  /* handle the system call */
	  ఒకవేళ (call_vec[call_nr])
		  ఫలితం = (*call_vec[call_nr])(caller, సందేశం_2);
	  లేకపోతే {
		  రాయి("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  ఫలితం = EBADREQUEST;
	  }
  }

  తిరిగి ఫలితం;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
శూన్య kernel_call(సందేశం *m_user, నిర్మాణం proc * caller)
{
  పూర్ణాంకం ఫలితం = OK;
  సందేశం సందేశం_2;

  caller->p_delivermsg_vir = (vir_బైట్లు) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  ఒకవేళ (copy_సందేశం_from_user(m_user, &సందేశం_2) == 0) {
	  సందేశం_2.m_ఆధారం = caller->p_endpoint;
	  ఫలితం = kernel_call_dispatch(caller, &సందేశం_2);
  }
  లేకపోతే {
	  రాయి("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  తిరిగి;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_ముగింపు(caller, &సందేశం_2, ఫలితం);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
శూన్య system_init(శూన్య)
{
  నమోదు నిర్మాణం priv *sp;
  పూర్ణాంకం i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  కొరకు (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  కొరకు (sp=BEG_PRIV_ADDR; sp < చివర_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  కొరకు (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = ఖాళీ;
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
#ఒకవేళ defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#అయితే_ముగింపు

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
  map(SYS_SAFECOPYTO, do_safecopy_గమ్యం);	/* copy with pre-granted permission */
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
#ఒకవేళ defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#అయితే_ముగింపు

  /* i386-specific. */
#ఒకవేళ defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#అయితే_ముగింపు

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_కాలపట్టిక, do_కాలపట్టిక);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
పూర్ణాంకం get_priv(
  నమోదు నిర్మాణం proc *rc,		/* new (child) process pointer */
  పూర్ణాంకం priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  నమోదు నిర్మాణం priv *sp;                 /* privilege structure */

  ఒకవేళ(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      కొరకు (sp = BEG_DYN_PRIV_ADDR; sp < చివర_DYN_PRIV_ADDR; ++sp)
          ఒకవేళ (sp->s_proc_nr == NONE) ఆపు;
      ఒకవేళ (sp >= చివర_DYN_PRIV_ADDR) తిరిగి(ENOSPC);
  }
  లేకపోతే {                                    /* allocate slot from id */
      ఒకవేళ(!is_static_priv_id(priv_id)) {
          తిరిగి EINVAL;                    /* invalid static priv id */
      }
      ఒకవేళ(priv[priv_id].s_proc_nr != NONE) {
          తిరిగి EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  తిరిగి(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
శూన్య సమితి_sendto_బిట్టు(స్థిరరాశి నిర్మాణం proc *rp, పూర్ణాంకం id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  ఒకవేళ (id_గమ్యం_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_బిట్టు(priv(rp)->s_ipc_గమ్యం, id);
	తిరిగి;
  }

  సమితి_sys_బిట్టు(priv(rp)->s_ipc_గమ్యం, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  ఒకవేళ (priv_addr(id)->s_trap_ముసుగు & ~((1 << RECEIVE)))
      సమితి_sys_బిట్టు(priv_addr(id)->s_ipc_గమ్యం, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
శూన్య unset_sendto_బిట్టు(స్థిరరాశి నిర్మాణం proc *rp, పూర్ణాంకం id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_బిట్టు(priv(rp)->s_ipc_గమ్యం, id);

  unset_sys_బిట్టు(priv_addr(id)->s_ipc_గమ్యం, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
శూన్య fill_sendto_ముసుగు(స్థిరరాశి నిర్మాణం proc *rp, sys_map_t *map)
{
  పూర్ణాంకం i;

  కొరకు (i=0; i < NR_SYS_PROCS; i++) {
  	ఒకవేళ (get_sys_బిట్టు(*map, i))
  		సమితి_sendto_బిట్టు(rp, i);
  	లేకపోతే
  		unset_sendto_బిట్టు(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
పూర్ణాంకం send_sig(endpoint_t ep, పూర్ణాంకం sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  నమోదు నిర్మాణం proc *rp;
  నిర్మాణం priv *priv;
  పూర్ణాంకం proc_nr;

  ఒకవేళ(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	తిరిగి EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  ఒకవేళ(!priv) తిరిగి ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  తిరిగి OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
శూన్య cause_sig(proc_nr_t proc_nr, పూర్ణాంకం sig_nr)
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
  నమోదు నిర్మాణం proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  పూర్ణాంకం sig_mgr_proc_nr;
  పూర్ణాంకం s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  ఒకవేళ(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  ఒకవేళ(rp->p_endpoint == sig_mgr) {
       ఒకవేళ(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           ఒకవేళ(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               తిరిగి;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       ఒకవేళ(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       తిరిగి;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  ఒకవేళ (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      ఒకవేళ (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_సమితి(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          ఒకవేళ(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
శూన్య sig_delay_పూర్తి(నిర్మాణం proc *rp)
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
శూన్య send_diag_sig(శూన్య)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  నిర్మాణం priv *privp;
  endpoint_t ep;

  కొరకు (privp = BEG_PRIV_ADDR; privp < చివర_PRIV_ADDR; privp++) {
	ఒకవేళ (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
స్థిర శూన్య clear_memreq(నిర్మాణం proc *rp)
{
  నిర్మాణం proc **rpp;

  ఒకవేళ (!RTS_ISSET(rp, RTS_VMREQUEST))
	తిరిగి; /* nothing to do */

  కొరకు (rpp = &vmrequest; *rpp != ఖాళీ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ఒకవేళ (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		ఆపు;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
స్థిర శూన్య clear_ipc(
  నమోదు నిర్మాణం proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  నిర్మాణం proc **xpp;			/* iterate over caller queue */

  ఒకవేళ (RTS_ISSET(rc, RTS_SENDING)) {
      పూర్ణాంకం లక్ష్యం_proc;

      okendpt(rc->p_sendto_e, &లక్ష్యం_proc);
      xpp = &proc_addr(లక్ష్యం_proc)->p_caller_q; /* destination's queue */
      అయితే (*xpp) {		/* check entire queue */
          ఒకవేళ (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
	      రాయి("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#అయితే_ముగింపు
              ఆపు;				/* can only be queued once */
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
శూన్య clear_endpoint(నిర్మాణం proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  ఒకవేళ(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#ఒకవేళ DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#అయితే_ముగింపు

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_సమితి(rc, RTS_NO_ENDPOINT);
  ఒకవేళ (priv(rc)->s_flags & SYS_PROC)
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
శూన్య clear_ipc_refs(
  నమోదు నిర్మాణం proc *rc,		/* slot of process to clean up */
  పూర్ణాంకం caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  నిర్మాణం proc *rp;			/* iterate over process table */
  పూర్ణాంకం src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  అయితే ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_గమ్యం_nr(src_id)), rc);

  కొరకు (rp = BEG_PROC_ADDR; rp < చివర_PROC_ADDR; rp++) {
      ఒకవేళ(isemptyp(rp))
	కొనసాగు;

      /* Unset pending notification bits. */
      unset_sys_బిట్టు(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_బిట్టు(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      ఒకవేళ (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
శూన్య kernel_call_resume(నిర్మాణం proc *caller)
{
	పూర్ణాంకం ఫలితం;

	ప్రతిజ్ఞా(!RTS_ISSET(caller, RTS_స్థానం_FREE));
	ప్రతిజ్ఞా(!RTS_ISSET(caller, RTS_VMREQUEST));

	ప్రతిజ్ఞా(caller->p_vmrequest.saved.reqmsg.m_ఆధారం == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	ఫలితం = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_ముగింపు(caller, &caller->p_vmrequest.saved.reqmsg, ఫలితం);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
పూర్ణాంకం sched_proc(నిర్మాణం proc *p, పూర్ణాంకం ప్రాధాన్యత, పూర్ణాంకం కాలాంశం, పూర్ణాంకం cpu, పూర్ణాంకం niced)
{
	/* Make sure the values given are within the allowed range.*/
	ఒకవేళ ((ప్రాధాన్యత < TASK_Q && ప్రాధాన్యత != -1) || ప్రాధాన్యత > NR_SCHED_QUEUES)
		తిరిగి(EINVAL);

	ఒకవేళ (కాలాంశం < 1 && కాలాంశం != -1)
		తిరిగి(EINVAL);

#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ ((cpu < 0 && cpu != -1) || (cpu > 0 && (గుర్తులేని) cpu >= ncpus))
		తిరిగి(EINVAL);
	ఒకవేళ (cpu != -1 && !(cpu_is_ready(cpu)))
		తిరిగి EBADCPU;
#అయితే_ముగింపు

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	ఒకవేళ (proc_is_runnable(p)) {
#నిర్వచితమైతే CONFIG_SMP
		ఒకవేళ (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_కాలపట్టిక_migrate_proc(p, cpu);
		}
#అయితే_ముగింపు

		RTS_సమితి(p, RTS_NO_కాలాంశం);
	}

	ఒకవేళ (proc_is_runnable(p))
		RTS_సమితి(p, RTS_NO_కాలాంశం);

	ఒకవేళ (ప్రాధాన్యత != -1)
		p->p_ప్రాధాన్యత = ప్రాధాన్యత;
	ఒకవేళ (కాలాంశం != -1) {
		p->p_కాలాంశం_కొలత_ms = కాలాంశం;
		p->p_cpu_సమయం_ఎడమ = ms_2_cpu_సమయం(కాలాంశం);
	}
#నిర్వచితమైతే CONFIG_SMP
	ఒకవేళ (cpu != -1)
		p->p_cpu = cpu;
#అయితే_ముగింపు

	ఒకవేళ (niced)
		p->p_misc_flags |= MF_NICED;
	లేకపోతే
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_కాలాంశం);

	తిరిగి OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
పూర్ణాంకం కూడు_ipc_filter(నిర్మాణం proc *rp, పూర్ణాంకం type, vir_బైట్లు చిరునామా,
	size_t నిడివి)
{
	పూర్ణాంకం అంకె_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	ఒకవేళ (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		తిరిగి EINVAL;

	ఒకవేళ (నిడివి % పరిమాణం(ipc_filter_el_t) != 0)
		తిరిగి EINVAL;

	అంకె_elements = నిడివి / పరిమాణం(ipc_filter_el_t);
	ఒకవేళ (అంకె_elements <= 0 || అంకె_elements > IPCF_MAX_ELEMENTS)
		తిరిగి E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_స్థానం(type, &ipcf);
	ఒకవేళ (ipcf == ఖాళీ)
		తిరిగి ENOMEM;

	/* Fill details. */
	ipcf->అంకె_elements = అంకె_elements;
	ipcf->తదుపరి = ఖాళీ;
	r = దత్తాంశం_copy(rp->p_endpoint, చిరునామా,
		KERNEL, (vir_బైట్లు)ipcf->elements, నిడివి);
	ఒకవేళ (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	ఒకవేళ (r != OK) {
		IPCF_POOL_FREE_స్థానం(ipcf);
		తిరిగి r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	కొరకు (ipcfp = &priv(rp)->s_ipcf; *ipcfp != ఖాళీ;
	    ipcfp = &(*ipcfp)->తదుపరి)
		;
	*ipcfp = ipcf;

	తిరిగి OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
శూన్య clear_ipc_filters(నిర్మాణం proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	అయితే (ipcf != ఖాళీ) {
		curr_ipcf = ipcf;
		ipcf = ipcf->తదుపరి;
		IPCF_POOL_FREE_స్థానం(curr_ipcf);
	}

	priv(rp)->s_ipcf = ఖాళీ;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	ఒకవేళ (rp->p_endpoint == VM_PROC_NR && vmrequest != ఖాళీ)
		ఒకవేళ (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
పూర్ణాంకం check_ipc_filter(ipc_filter_t *ipcf, పూర్ణాంకం fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	పూర్ణాంకం i, అంకె_elements, flags;

	ఒకవేళ (ipcf == ఖాళీ)
		తిరిగి OK;

	అంకె_elements = ipcf->అంకె_elements;
	flags = 0;
	కొరకు (i = 0; i < అంకె_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		ఒకవేళ (!IPCF_EL_CHECK(ipcf_el))
			తిరిగి EINVAL;
		flags |= ipcf_el->flags;
	}

	ఒకవేళ (fill_flags)
		ipcf->flags = flags;
	లేకపోతే ఒకవేళ (ipcf->flags != flags)
		తిరిగి EINVAL;
	తిరిగి OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
పూర్ణాంకం allow_ipc_filtered_సందేశం(నిర్మాణం proc *rp, endpoint_t src_e,
	vir_బైట్లు m_src_v, సందేశం *m_src_p)
{
	పూర్ణాంకం i, r, అంకె_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	సందేశం m_buff;

	ipcf = priv(rp)->s_ipcf;
	ఒకవేళ (ipcf == ఖాళీ)
		తిరిగి TRUE; /* no IPC filters, always allow */

	ఒకవేళ (m_src_p == ఖాళీ) {
		ప్రతిజ్ఞా(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		చేయి {
#ఒకవేళ DEBUG_DUMPIPCF
			ఒకవేళ (TRUE) {
#లేకపోతే
			ఒకవేళ (ipcf->flags & IPCF_సరిపోలిక_M_TYPE) {
#అయితే_ముగింపు
				get_mtype = TRUE;
				ఆపు;
			}
			ipcf = ipcf->తదుపరి;
		} అయితే (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		ఒకవేళ (get_mtype) {
			r = దత్తాంశం_copy(src_e,
			    m_src_v + offsetof(సందేశం, m_type), KERNEL,
			    (vir_బైట్లు)&m_buff.m_type, పరిమాణం(m_buff.m_type));
			ఒకవేళ (r != OK) {
				/* allow for now, this will fail later anyway */
#ఒకవేళ DEBUG_DUMPIPCF
				రాయి("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#అయితే_ముగింపు
				తిరిగి TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_ఆధారం = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	చేయి {
		ఒకవేళ (allow != (ipcf->type == IPCF_WHITELIST)) {
			అంకె_elements = ipcf->అంకె_elements;
			కొరకు (i = 0; i < అంకె_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				ఒకవేళ (IPCF_EL_సరిపోలిక(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					ఆపు;
				}
			}
		}
		ipcf = ipcf->తదుపరి;
	} అయితే (ipcf);

#ఒకవేళ DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#అయితే_ముగింపు

	తిరిగి allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
పూర్ణాంకం allow_ipc_filtered_memreq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	నిర్మాణం proc *vmp;
	సందేశం m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	ఒకవేళ (priv(vmp)->s_ipcf == ఖాళీ)
		తిరిగి TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_సందేశం;
	ఒకవేళ (!allow_ipc_filtered_సందేశం(vmp, SYSTEM, 0, &m_buf))
		తిరిగి FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	తిరిగి TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
పూర్ణాంకం priv_కూడు_irq(నిర్మాణం proc *rp, పూర్ణాంకం irq)
{
        నిర్మాణం priv *priv = priv(rp);
        పూర్ణాంకం i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	కొరకు (i = 0; i < priv->s_nr_irq; i++) {
		ఒకవేళ (priv->s_irq_tab[i] == irq)
			తిరిగి OK;
	}

	i= priv->s_nr_irq;
	ఒకవేళ (i >= NR_IRQ) {
		రాయి("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		తిరిగి ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	తిరిగి OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
పూర్ణాంకం priv_కూడు_io(నిర్మాణం proc *rp, నిర్మాణం io_range *ior)
{
        నిర్మాణం priv *priv = priv(rp);
        పూర్ణాంకం i;

	priv->s_flags |= CHECK_IO_పోర్టు;	/* Check I/O accesses */

	కొరకు (i = 0; i < priv->s_nr_io_range; i++) {
		ఒకవేళ (priv->s_io_tab[i].ior_పునాది == ior->ior_పునాది &&
			priv->s_io_tab[i].ior_హద్దు == ior->ior_హద్దు)
			తిరిగి OK;
	}

	i= priv->s_nr_io_range;
	ఒకవేళ (i >= NR_IO_RANGE) {
		రాయి("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		తిరిగి ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	తిరిగి OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
పూర్ణాంకం priv_కూడు_mem(నిర్మాణం proc *rp, నిర్మాణం minix_mem_range *memr)
{
        నిర్మాణం priv *priv = priv(rp);
        పూర్ణాంకం i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	కొరకు (i = 0; i < priv->s_nr_mem_range; i++) {
		ఒకవేళ (priv->s_mem_tab[i].mr_పునాది == memr->mr_పునాది &&
			priv->s_mem_tab[i].mr_హద్దు == memr->mr_హద్దు)
			తిరిగి OK;
	}

	i= priv->s_nr_mem_range;
	ఒకవేళ (i >= NR_MEM_RANGE) {
		రాయి("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		తిరిగి ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	తిరిగి OK;
}

