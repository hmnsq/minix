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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव "kernel/clock.h"
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stddef.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <minix/endpoint.h>
#अन्तर्भाव <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
स्थिर पूर्णाङ्क (*call_vec[NR_SYS_CALLS])(संरचना proc * caller, सन्देशः *m_ptr);

#परिभाषय map(call_nr, handler) 					\
    {	पूर्णाङ्क call_सूचकांक = call_nr-KERNEL_CALL; 				\
    	प्रतिज्ञा(call_सूचकांक >= 0 && call_सूचकांक < NR_SYS_CALLS);			\
    call_vec[call_सूचकांक] = (handler)  ; }

स्थिर शून्य kernel_call_finish(संरचना proc * caller, सन्देशः *सन्देशः_2, पूर्णाङ्क परिणाम)
{
  यदि(परिणाम == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  प्रतिज्ञा(RTS_ISSET(caller, RTS_VMREQUEST));
	  प्रतिज्ञा(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *सन्देशः_2;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } अन्यथा {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_source = NONE;
	  यदि (परिणाम != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  सन्देशः_2->m_source = SYSTEM;
		  सन्देशः_2->m_type = परिणाम;		/* report status of call */
#यदि DEBUG_IPC_HOOK
	hook_ipc_msgkresult(सन्देशः_2, caller);
#यदि_अन्तः
		  यदि (copy_सन्देशः_गन्तव्यम्_user(सन्देशः_2, (सन्देशः *)caller->p_delivermsg_vir)) {
			  प्रारूप_लिख("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

स्थिर पूर्णाङ्क kernel_call_dispatch(संरचना proc * caller, सन्देशः *सन्देशः_2)
{
  पूर्णाङ्क परिणाम = OK;
  पूर्णाङ्क call_nr;

#यदि DEBUG_IPC_HOOK
	hook_ipc_msgkcall(सन्देशः_2, caller);
#यदि_अन्तः
  call_nr = सन्देशः_2->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  यदि (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  प्रारूप_लिख("SYSTEM: illegal request %d from %d.\n",
			  call_nr,सन्देशः_2->m_source);
	  परिणाम = EBADREQUEST;			/* illegal message type */
  }
  अन्यथा यदि (!GET_BIT(priv(caller)->s_k_call_आवरणम्, call_nr)) {
	  प्रारूप_लिख("SYSTEM: denied request %d from %d.\n",
			  call_nr,सन्देशः_2->m_source);
	  परिणाम = ECALLDENIED;			/* illegal message type */
  } अन्यथा {
	  /* handle the system call */
	  यदि (call_vec[call_nr])
		  परिणाम = (*call_vec[call_nr])(caller, सन्देशः_2);
	  अन्यथा {
		  प्रारूप_लिख("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  परिणाम = EBADREQUEST;
	  }
  }

  प्रत्यागम परिणाम;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
शून्य kernel_call(सन्देशः *m_user, संरचना proc * caller)
{
  पूर्णाङ्क परिणाम = OK;
  सन्देशः सन्देशः_2;

  caller->p_delivermsg_vir = (vir_अष्टकानि) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  यदि (copy_सन्देशः_from_user(m_user, &सन्देशः_2) == 0) {
	  सन्देशः_2.m_source = caller->p_endpoint;
	  परिणाम = kernel_call_dispatch(caller, &सन्देशः_2);
  }
  अन्यथा {
	  प्रारूप_लिख("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  प्रत्यागम;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_finish(caller, &सन्देशः_2, परिणाम);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
शून्य system_init(शून्य)
{
  पञ्जिका संरचना priv *sp;
  पूर्णाङ्क i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  प्रति (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  प्रति (sp=BEG_PRIV_ADDR; sp < अन्तः_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  प्रति (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = रिक्त;
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
#यदि defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#यदि_अन्तः

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
  map(SYS_SAFECOPYTO, do_safecopy_गन्तव्यम्);	/* copy with pre-granted permission */
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
#यदि defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#यदि_अन्तः

  /* i386-specific. */
#यदि defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#यदि_अन्तः

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_कालसारणी, do_कालसारणी);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
पूर्णाङ्क get_priv(
  पञ्जिका संरचना proc *rc,		/* new (child) process pointer */
  पूर्णाङ्क priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  पञ्जिका संरचना priv *sp;                 /* privilege structure */

  यदि(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      प्रति (sp = BEG_DYN_PRIV_ADDR; sp < अन्तः_DYN_PRIV_ADDR; ++sp)
          यदि (sp->s_proc_nr == NONE) भङ्ग;
      यदि (sp >= अन्तः_DYN_PRIV_ADDR) प्रत्यागम(ENOSPC);
  }
  अन्यथा {                                    /* allocate slot from id */
      यदि(!is_static_priv_id(priv_id)) {
          प्रत्यागम EINVAL;                    /* invalid static priv id */
      }
      यदि(priv[priv_id].s_proc_nr != NONE) {
          प्रत्यागम EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  प्रत्यागम(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
शून्य समुच्चयः_sendto_bit(ध्रुव संरचना proc *rp, पूर्णाङ्क id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  यदि (id_गन्तव्यम्_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_bit(priv(rp)->s_ipc_गन्तव्यम्, id);
	प्रत्यागम;
  }

  समुच्चयः_sys_bit(priv(rp)->s_ipc_गन्तव्यम्, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  यदि (priv_addr(id)->s_trap_आवरणम् & ~((1 << RECEIVE)))
      समुच्चयः_sys_bit(priv_addr(id)->s_ipc_गन्तव्यम्, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
शून्य unset_sendto_bit(ध्रुव संरचना proc *rp, पूर्णाङ्क id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_bit(priv(rp)->s_ipc_गन्तव्यम्, id);

  unset_sys_bit(priv_addr(id)->s_ipc_गन्तव्यम्, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
शून्य fill_sendto_आवरणम्(ध्रुव संरचना proc *rp, sys_map_t *map)
{
  पूर्णाङ्क i;

  प्रति (i=0; i < NR_SYS_PROCS; i++) {
  	यदि (get_sys_bit(*map, i))
  		समुच्चयः_sendto_bit(rp, i);
  	अन्यथा
  		unset_sendto_bit(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
पूर्णाङ्क send_sig(endpoint_t ep, पूर्णाङ्क sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  पञ्जिका संरचना proc *rp;
  संरचना priv *priv;
  पूर्णाङ्क proc_nr;

  यदि(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	प्रत्यागम EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  यदि(!priv) प्रत्यागम ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  प्रत्यागम OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
शून्य cause_sig(proc_nr_t proc_nr, पूर्णाङ्क sig_nr)
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
  पञ्जिका संरचना proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  पूर्णाङ्क sig_mgr_proc_nr;
  पूर्णाङ्क s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  यदि(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  यदि(rp->p_endpoint == sig_mgr) {
       यदि(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           यदि(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               प्रत्यागम;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       यदि(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       प्रत्यागम;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  यदि (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      यदि (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_समुच्चयः(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          यदि(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
शून्य sig_delay_समाप्तम्(संरचना proc *rp)
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
शून्य send_diag_sig(शून्य)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  संरचना priv *privp;
  endpoint_t ep;

  प्रति (privp = BEG_PRIV_ADDR; privp < अन्तः_PRIV_ADDR; privp++) {
	यदि (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
स्थिर शून्य clear_memreq(संरचना proc *rp)
{
  संरचना proc **rpp;

  यदि (!RTS_ISSET(rp, RTS_VMREQUEST))
	प्रत्यागम; /* nothing to do */

  प्रति (rpp = &vmrequest; *rpp != रिक्त;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	यदि (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		भङ्ग;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
स्थिर शून्य clear_ipc(
  पञ्जिका संरचना proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  संरचना proc **xpp;			/* iterate over caller queue */

  यदि (RTS_ISSET(rc, RTS_SENDING)) {
      पूर्णाङ्क target_proc;

      okendpt(rc->p_sendto_e, &target_proc);
      xpp = &proc_addr(target_proc)->p_caller_q; /* destination's queue */
      यावत् (*xpp) {		/* check entire queue */
          यदि (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#यदि DEBUG_ENABLE_IPC_WARNINGS
	      प्रारूप_लिख("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#यदि_अन्तः
              भङ्ग;				/* can only be queued once */
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
शून्य clear_endpoint(संरचना proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  यदि(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#यदि DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#यदि_अन्तः

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_समुच्चयः(rc, RTS_NO_ENDPOINT);
  यदि (priv(rc)->s_flags & SYS_PROC)
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
शून्य clear_ipc_refs(
  पञ्जिका संरचना proc *rc,		/* slot of process to clean up */
  पूर्णाङ्क caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  संरचना proc *rp;			/* iterate over process table */
  पूर्णाङ्क src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  यावत् ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_गन्तव्यम्_nr(src_id)), rc);

  प्रति (rp = BEG_PROC_ADDR; rp < अन्तः_PROC_ADDR; rp++) {
      यदि(isemptyp(rp))
	अनुवर्त;

      /* Unset pending notification bits. */
      unset_sys_bit(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_bit(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      यदि (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
शून्य kernel_call_resume(संरचना proc *caller)
{
	पूर्णाङ्क परिणाम;

	प्रतिज्ञा(!RTS_ISSET(caller, RTS_SLOT_FREE));
	प्रतिज्ञा(!RTS_ISSET(caller, RTS_VMREQUEST));

	प्रतिज्ञा(caller->p_vmrequest.saved.reqmsg.m_source == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	परिणाम = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_finish(caller, &caller->p_vmrequest.saved.reqmsg, परिणाम);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
पूर्णाङ्क sched_proc(संरचना proc *p, पूर्णाङ्क प्राथमिकता, पूर्णाङ्क कालांशः, पूर्णाङ्क cpu, पूर्णाङ्क niced)
{
	/* Make sure the values given are within the allowed range.*/
	यदि ((प्राथमिकता < TASK_Q && प्राथमिकता != -1) || प्राथमिकता > NR_SCHED_QUEUES)
		प्रत्यागम(EINVAL);

	यदि (कालांशः < 1 && कालांशः != -1)
		प्रत्यागम(EINVAL);

#यदि_परिभाषितम् CONFIG_SMP
	यदि ((cpu < 0 && cpu != -1) || (cpu > 0 && (अचिह्नित) cpu >= ncpus))
		प्रत्यागम(EINVAL);
	यदि (cpu != -1 && !(cpu_is_ready(cpu)))
		प्रत्यागम EBADCPU;
#यदि_अन्तः

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	यदि (proc_is_runnable(p)) {
#यदि_परिभाषितम् CONFIG_SMP
		यदि (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_कालसारणी_migrate_proc(p, cpu);
		}
#यदि_अन्तः

		RTS_समुच्चयः(p, RTS_NO_कालांशः);
	}

	यदि (proc_is_runnable(p))
		RTS_समुच्चयः(p, RTS_NO_कालांशः);

	यदि (प्राथमिकता != -1)
		p->p_प्राथमिकता = प्राथमिकता;
	यदि (कालांशः != -1) {
		p->p_कालांशः_आकार_ms = कालांशः;
		p->p_cpu_time_वामः = ms_2_cpu_time(कालांशः);
	}
#यदि_परिभाषितम् CONFIG_SMP
	यदि (cpu != -1)
		p->p_cpu = cpu;
#यदि_अन्तः

	यदि (niced)
		p->p_misc_flags |= MF_NICED;
	अन्यथा
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_कालांशः);

	प्रत्यागम OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
पूर्णाङ्क सङ्कलय_ipc_filter(संरचना proc *rp, पूर्णाङ्क type, vir_अष्टकानि स्थानाङ्कः,
	size_t दीर्घता)
{
	पूर्णाङ्क अङ्कः_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	यदि (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		प्रत्यागम EINVAL;

	यदि (दीर्घता % परिमाण(ipc_filter_el_t) != 0)
		प्रत्यागम EINVAL;

	अङ्कः_elements = दीर्घता / परिमाण(ipc_filter_el_t);
	यदि (अङ्कः_elements <= 0 || अङ्कः_elements > IPCF_MAX_ELEMENTS)
		प्रत्यागम E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_SLOT(type, &ipcf);
	यदि (ipcf == रिक्त)
		प्रत्यागम ENOMEM;

	/* Fill details. */
	ipcf->अङ्कः_elements = अङ्कः_elements;
	ipcf->अग्र = रिक्त;
	r = दत्त_copy(rp->p_endpoint, स्थानाङ्कः,
		KERNEL, (vir_अष्टकानि)ipcf->elements, दीर्घता);
	यदि (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	यदि (r != OK) {
		IPCF_POOL_FREE_SLOT(ipcf);
		प्रत्यागम r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	प्रति (ipcfp = &priv(rp)->s_ipcf; *ipcfp != रिक्त;
	    ipcfp = &(*ipcfp)->अग्र)
		;
	*ipcfp = ipcf;

	प्रत्यागम OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
शून्य clear_ipc_filters(संरचना proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	यावत् (ipcf != रिक्त) {
		curr_ipcf = ipcf;
		ipcf = ipcf->अग्र;
		IPCF_POOL_FREE_SLOT(curr_ipcf);
	}

	priv(rp)->s_ipcf = रिक्त;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	यदि (rp->p_endpoint == VM_PROC_NR && vmrequest != रिक्त)
		यदि (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
पूर्णाङ्क check_ipc_filter(ipc_filter_t *ipcf, पूर्णाङ्क fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	पूर्णाङ्क i, अङ्कः_elements, flags;

	यदि (ipcf == रिक्त)
		प्रत्यागम OK;

	अङ्कः_elements = ipcf->अङ्कः_elements;
	flags = 0;
	प्रति (i = 0; i < अङ्कः_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		यदि (!IPCF_EL_CHECK(ipcf_el))
			प्रत्यागम EINVAL;
		flags |= ipcf_el->flags;
	}

	यदि (fill_flags)
		ipcf->flags = flags;
	अन्यथा यदि (ipcf->flags != flags)
		प्रत्यागम EINVAL;
	प्रत्यागम OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
पूर्णाङ्क allow_ipc_filtered_सन्देशः(संरचना proc *rp, endpoint_t src_e,
	vir_अष्टकानि m_src_v, सन्देशः *m_src_p)
{
	पूर्णाङ्क i, r, अङ्कः_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	सन्देशः m_buff;

	ipcf = priv(rp)->s_ipcf;
	यदि (ipcf == रिक्त)
		प्रत्यागम TRUE; /* no IPC filters, always allow */

	यदि (m_src_p == रिक्त) {
		प्रतिज्ञा(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		कुरु {
#यदि DEBUG_DUMPIPCF
			यदि (TRUE) {
#अन्यथा
			यदि (ipcf->flags & IPCF_मेलनम्_M_TYPE) {
#यदि_अन्तः
				get_mtype = TRUE;
				भङ्ग;
			}
			ipcf = ipcf->अग्र;
		} यावत् (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		यदि (get_mtype) {
			r = दत्त_copy(src_e,
			    m_src_v + offsetof(सन्देशः, m_type), KERNEL,
			    (vir_अष्टकानि)&m_buff.m_type, परिमाण(m_buff.m_type));
			यदि (r != OK) {
				/* allow for now, this will fail later anyway */
#यदि DEBUG_DUMPIPCF
				प्रारूप_लिख("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#यदि_अन्तः
				प्रत्यागम TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_source = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	कुरु {
		यदि (allow != (ipcf->type == IPCF_WHITELIST)) {
			अङ्कः_elements = ipcf->अङ्कः_elements;
			प्रति (i = 0; i < अङ्कः_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				यदि (IPCF_EL_मेलनम्(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					भङ्ग;
				}
			}
		}
		ipcf = ipcf->अग्र;
	} यावत् (ipcf);

#यदि DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#यदि_अन्तः

	प्रत्यागम allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
पूर्णाङ्क allow_ipc_filtered_memreq(संरचना proc *src_rp, संरचना proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	संरचना proc *vmp;
	सन्देशः m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	यदि (priv(vmp)->s_ipcf == रिक्त)
		प्रत्यागम TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_सन्देशः;
	यदि (!allow_ipc_filtered_सन्देशः(vmp, SYSTEM, 0, &m_buf))
		प्रत्यागम FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	प्रत्यागम TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
पूर्णाङ्क priv_सङ्कलय_irq(संरचना proc *rp, पूर्णाङ्क irq)
{
        संरचना priv *priv = priv(rp);
        पूर्णाङ्क i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	प्रति (i = 0; i < priv->s_nr_irq; i++) {
		यदि (priv->s_irq_tab[i] == irq)
			प्रत्यागम OK;
	}

	i= priv->s_nr_irq;
	यदि (i >= NR_IRQ) {
		प्रारूप_लिख("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		प्रत्यागम ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	प्रत्यागम OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
पूर्णाङ्क priv_सङ्कलय_io(संरचना proc *rp, संरचना io_range *ior)
{
        संरचना priv *priv = priv(rp);
        पूर्णाङ्क i;

	priv->s_flags |= CHECK_IO_पत्तनम्;	/* Check I/O accesses */

	प्रति (i = 0; i < priv->s_nr_io_range; i++) {
		यदि (priv->s_io_tab[i].ior_आधारः == ior->ior_आधारः &&
			priv->s_io_tab[i].ior_सीमा == ior->ior_सीमा)
			प्रत्यागम OK;
	}

	i= priv->s_nr_io_range;
	यदि (i >= NR_IO_RANGE) {
		प्रारूप_लिख("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		प्रत्यागम ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	प्रत्यागम OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
पूर्णाङ्क priv_सङ्कलय_mem(संरचना proc *rp, संरचना minix_mem_range *memr)
{
        संरचना priv *priv = priv(rp);
        पूर्णाङ्क i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	प्रति (i = 0; i < priv->s_nr_mem_range; i++) {
		यदि (priv->s_mem_tab[i].mr_आधारः == memr->mr_आधारः &&
			priv->s_mem_tab[i].mr_सीमा == memr->mr_सीमा)
			प्रत्यागम OK;
	}

	i= priv->s_nr_mem_range;
	यदि (i >= NR_MEM_RANGE) {
		प्रारूप_लिख("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		प्रत्यागम ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	प्रत्यागम OK;
}

