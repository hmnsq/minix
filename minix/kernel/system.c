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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
ਸਥਿਰ ਪੂਰਨ (*call_vec[NR_SYS_CALLS])(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ map(call_nr, handler) 					\
    {	ਪੂਰਨ call_ਸੂਚਕਾਂਕ = call_nr-KERNEL_CALL; 				\
    	ਪ੍ਰਤਿਜ੍ਞਾ(call_ਸੂਚਕਾਂਕ >= 0 && call_ਸੂਚਕਾਂਕ < NR_SYS_CALLS);			\
    call_vec[call_ਸੂਚਕਾਂਕ] = (handler)  ; }

ਸਥਿਰ ਖਾਲੀ kernel_call_ਅੰਤ(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਪੂਰਨ ਨਤੀਜਾ)
{
  ਜੇ(ਨਤੀਜਾ == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(caller, RTS_VMREQUEST));
	  ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *ਸੁਨੇਹਾ_2;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } ਨਹੀਂਤਾਂ {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_ਸਰੋਤ = NONE;
	  ਜੇ (ਨਤੀਜਾ != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  ਸੁਨੇਹਾ_2->m_ਸਰੋਤ = SYSTEM;
		  ਸੁਨੇਹਾ_2->m_type = ਨਤੀਜਾ;		/* report status of call */
#ਜੇ DEBUG_IPC_HOOK
	hook_ipc_msgkresult(ਸੁਨੇਹਾ_2, caller);
#ਜੇ_ਅੰਤ
		  ਜੇ (copy_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_user(ਸੁਨੇਹਾ_2, (ਸੁਨੇਹਾ *)caller->p_delivermsg_vir)) {
			  ਲਿਖੋ("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

ਸਥਿਰ ਪੂਰਨ kernel_call_dispatch(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2)
{
  ਪੂਰਨ ਨਤੀਜਾ = OK;
  ਪੂਰਨ call_nr;

#ਜੇ DEBUG_IPC_HOOK
	hook_ipc_msgkcall(ਸੁਨੇਹਾ_2, caller);
#ਜੇ_ਅੰਤ
  call_nr = ਸੁਨੇਹਾ_2->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  ਜੇ (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  ਲਿਖੋ("SYSTEM: illegal request %d from %d.\n",
			  call_nr,ਸੁਨੇਹਾ_2->m_ਸਰੋਤ);
	  ਨਤੀਜਾ = EBADREQUEST;			/* illegal message type */
  }
  ਨਹੀਂਤਾਂ ਜੇ (!GET_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(caller)->s_k_call_ਨਕਾਬ, call_nr)) {
	  ਲਿਖੋ("SYSTEM: denied request %d from %d.\n",
			  call_nr,ਸੁਨੇਹਾ_2->m_ਸਰੋਤ);
	  ਨਤੀਜਾ = ECALLDENIED;			/* illegal message type */
  } ਨਹੀਂਤਾਂ {
	  /* handle the system call */
	  ਜੇ (call_vec[call_nr])
		  ਨਤੀਜਾ = (*call_vec[call_nr])(caller, ਸੁਨੇਹਾ_2);
	  ਨਹੀਂਤਾਂ {
		  ਲਿਖੋ("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  ਨਤੀਜਾ = EBADREQUEST;
	  }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
ਖਾਲੀ kernel_call(ਸੁਨੇਹਾ *m_user, ਬਣਤਰ proc * caller)
{
  ਪੂਰਨ ਨਤੀਜਾ = OK;
  ਸੁਨੇਹਾ ਸੁਨੇਹਾ_2;

  caller->p_delivermsg_vir = (vir_ਬਾਈਟਾਂ) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  ਜੇ (copy_ਸੁਨੇਹਾ_from_user(m_user, &ਸੁਨੇਹਾ_2) == 0) {
	  ਸੁਨੇਹਾ_2.m_ਸਰੋਤ = caller->p_endpoint;
	  ਨਤੀਜਾ = kernel_call_dispatch(caller, &ਸੁਨੇਹਾ_2);
  }
  ਨਹੀਂਤਾਂ {
	  ਲਿਖੋ("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  ਵਾਪਸ;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_ਅੰਤ(caller, &ਸੁਨੇਹਾ_2, ਨਤੀਜਾ);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
ਖਾਲੀ system_init(ਖਾਲੀ)
{
  ਰਜਿਸਟਰ ਬਣਤਰ priv *sp;
  ਪੂਰਨ i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  ਲਈ (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  ਲਈ (sp=BEG_PRIV_ADDR; sp < ਸਿਰਾ_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  ਲਈ (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = ਨਿਰਾ;
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
#ਜੇ defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#ਜੇ_ਅੰਤ

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
  map(SYS_SAFECOPYTO, do_safecopy_ਮੰਜ਼ਿਲ);	/* copy with pre-granted permission */
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
#ਜੇ defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#ਜੇ_ਅੰਤ

  /* i386-specific. */
#ਜੇ defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#ਜੇ_ਅੰਤ

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_ਸਮਾਂ_ਸਾਰਣੀ, do_ਸਮਾਂ_ਸਾਰਣੀ);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
ਪੂਰਨ get_priv(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rc,		/* new (child) process pointer */
  ਪੂਰਨ priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  ਰਜਿਸਟਰ ਬਣਤਰ priv *sp;                 /* privilege structure */

  ਜੇ(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      ਲਈ (sp = BEG_DYN_PRIV_ADDR; sp < ਸਿਰਾ_DYN_PRIV_ADDR; ++sp)
          ਜੇ (sp->s_proc_nr == NONE) ਰੋਕੋ;
      ਜੇ (sp >= ਸਿਰਾ_DYN_PRIV_ADDR) ਵਾਪਸ(ENOSPC);
  }
  ਨਹੀਂਤਾਂ {                                    /* allocate slot from id */
      ਜੇ(!is_static_priv_id(priv_id)) {
          ਵਾਪਸ EINVAL;                    /* invalid static priv id */
      }
      ਜੇ(priv[priv_id].s_proc_nr != NONE) {
          ਵਾਪਸ EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  ਵਾਪਸ(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
ਖਾਲੀ ਸੈੱਟ_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਣਤਰ proc *rp, ਪੂਰਨ id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  ਜੇ (id_ਮੰਜ਼ਿਲ_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_ipc_ਮੰਜ਼ਿਲ, id);
	ਵਾਪਸ;
  }

  ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_ipc_ਮੰਜ਼ਿਲ, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  ਜੇ (priv_addr(id)->s_trap_ਨਕਾਬ & ~((1 << RECEIVE)))
      ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv_addr(id)->s_ipc_ਮੰਜ਼ਿਲ, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
ਖਾਲੀ unset_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਣਤਰ proc *rp, ਪੂਰਨ id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_ipc_ਮੰਜ਼ਿਲ, id);

  unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv_addr(id)->s_ipc_ਮੰਜ਼ਿਲ, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
ਖਾਲੀ fill_sendto_ਨਕਾਬ(ਅਚਲ ਬਣਤਰ proc *rp, sys_map_t *map)
{
  ਪੂਰਨ i;

  ਲਈ (i=0; i < NR_SYS_PROCS; i++) {
  	ਜੇ (get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, i))
  		ਸੈੱਟ_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(rp, i);
  	ਨਹੀਂਤਾਂ
  		unset_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
ਪੂਰਨ send_sig(endpoint_t ep, ਪੂਰਨ sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;
  ਬਣਤਰ priv *priv;
  ਪੂਰਨ proc_nr;

  ਜੇ(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	ਵਾਪਸ EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  ਜੇ(!priv) ਵਾਪਸ ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  ਵਾਪਸ OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
ਖਾਲੀ cause_sig(proc_nr_t proc_nr, ਪੂਰਨ sig_nr)
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
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  ਪੂਰਨ sig_mgr_proc_nr;
  ਪੂਰਨ s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  ਜੇ(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  ਜੇ(rp->p_endpoint == sig_mgr) {
       ਜੇ(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           ਜੇ(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               ਵਾਪਸ;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       ਜੇ(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       ਵਾਪਸ;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  ਜੇ (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      ਜੇ (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_ਸੈੱਟ(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          ਜੇ(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
ਖਾਲੀ sig_delay_ਪੂਰਾ(ਬਣਤਰ proc *rp)
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
ਖਾਲੀ send_diag_sig(ਖਾਲੀ)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  ਬਣਤਰ priv *privp;
  endpoint_t ep;

  ਲਈ (privp = BEG_PRIV_ADDR; privp < ਸਿਰਾ_PRIV_ADDR; privp++) {
	ਜੇ (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ clear_memreq(ਬਣਤਰ proc *rp)
{
  ਬਣਤਰ proc **rpp;

  ਜੇ (!RTS_ISSET(rp, RTS_VMREQUEST))
	ਵਾਪਸ; /* nothing to do */

  ਲਈ (rpp = &vmrequest; *rpp != ਨਿਰਾ;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	ਜੇ (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		ਰੋਕੋ;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ clear_ipc(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  ਬਣਤਰ proc **xpp;			/* iterate over caller queue */

  ਜੇ (RTS_ISSET(rc, RTS_SENDING)) {
      ਪੂਰਨ ਨਿਸ਼ਾਨਾ_proc;

      okendpt(rc->p_sendto_e, &ਨਿਸ਼ਾਨਾ_proc);
      xpp = &proc_addr(ਨਿਸ਼ਾਨਾ_proc)->p_caller_q; /* destination's queue */
      ਜਦੋਂ (*xpp) {		/* check entire queue */
          ਜੇ (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
	      ਲਿਖੋ("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#ਜੇ_ਅੰਤ
              ਰੋਕੋ;				/* can only be queued once */
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
ਖਾਲੀ clear_endpoint(ਬਣਤਰ proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  ਜੇ(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#ਜੇ DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#ਜੇ_ਅੰਤ

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_ਸੈੱਟ(rc, RTS_NO_ENDPOINT);
  ਜੇ (priv(rc)->s_flags & SYS_PROC)
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
ਖਾਲੀ clear_ipc_refs(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rc,		/* slot of process to clean up */
  ਪੂਰਨ caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  ਬਣਤਰ proc *rp;			/* iterate over process table */
  ਪੂਰਨ src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  ਜਦੋਂ ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_ਮੰਜ਼ਿਲ_nr(src_id)), rc);

  ਲਈ (rp = BEG_PROC_ADDR; rp < ਸਿਰਾ_PROC_ADDR; rp++) {
      ਜੇ(isemptyp(rp))
	ਜਾਰੀ;

      /* Unset pending notification bits. */
      unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      ਜੇ (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
ਖਾਲੀ kernel_call_resume(ਬਣਤਰ proc *caller)
{
	ਪੂਰਨ ਨਤੀਜਾ;

	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(caller, RTS_ਥਾਂ_FREE));
	ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(caller, RTS_VMREQUEST));

	ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_vmrequest.saved.reqmsg.m_ਸਰੋਤ == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	ਨਤੀਜਾ = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_ਅੰਤ(caller, &caller->p_vmrequest.saved.reqmsg, ਨਤੀਜਾ);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
ਪੂਰਨ sched_proc(ਬਣਤਰ proc *p, ਪੂਰਨ ਤਰਜੀਹ, ਪੂਰਨ ਸਮਾਂ_ਹਿੱਸਾ, ਪੂਰਨ cpu, ਪੂਰਨ niced)
{
	/* Make sure the values given are within the allowed range.*/
	ਜੇ ((ਤਰਜੀਹ < TASK_Q && ਤਰਜੀਹ != -1) || ਤਰਜੀਹ > NR_SCHED_QUEUES)
		ਵਾਪਸ(EINVAL);

	ਜੇ (ਸਮਾਂ_ਹਿੱਸਾ < 1 && ਸਮਾਂ_ਹਿੱਸਾ != -1)
		ਵਾਪਸ(EINVAL);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ ((cpu < 0 && cpu != -1) || (cpu > 0 && (ਬਿਨਾਨਿਸ਼ਾਨੀ) cpu >= ncpus))
		ਵਾਪਸ(EINVAL);
	ਜੇ (cpu != -1 && !(cpu_is_ready(cpu)))
		ਵਾਪਸ EBADCPU;
#ਜੇ_ਅੰਤ

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	ਜੇ (proc_is_runnable(p)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
		ਜੇ (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_ਸਮਾਂ_ਸਾਰਣੀ_migrate_proc(p, cpu);
		}
#ਜੇ_ਅੰਤ

		RTS_ਸੈੱਟ(p, RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);
	}

	ਜੇ (proc_is_runnable(p))
		RTS_ਸੈੱਟ(p, RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);

	ਜੇ (ਤਰਜੀਹ != -1)
		p->p_ਤਰਜੀਹ = ਤਰਜੀਹ;
	ਜੇ (ਸਮਾਂ_ਹਿੱਸਾ != -1) {
		p->p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms = ਸਮਾਂ_ਹਿੱਸਾ;
		p->p_cpu_ਸਮਾਂ_ਖੱਬਾ = ms_2_cpu_ਸਮਾਂ(ਸਮਾਂ_ਹਿੱਸਾ);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ (cpu != -1)
		p->p_cpu = cpu;
#ਜੇ_ਅੰਤ

	ਜੇ (niced)
		p->p_misc_flags |= MF_NICED;
	ਨਹੀਂਤਾਂ
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
ਪੂਰਨ ਜੋੜੋ_ipc_filter(ਬਣਤਰ proc *rp, ਪੂਰਨ type, vir_ਬਾਈਟਾਂ ਪਤਾ,
	size_t ਲੰਮਾਈ)
{
	ਪੂਰਨ ਅੰਕ_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	ਜੇ (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		ਵਾਪਸ EINVAL;

	ਜੇ (ਲੰਮਾਈ % ਆਕਾਰ(ipc_filter_el_t) != 0)
		ਵਾਪਸ EINVAL;

	ਅੰਕ_elements = ਲੰਮਾਈ / ਆਕਾਰ(ipc_filter_el_t);
	ਜੇ (ਅੰਕ_elements <= 0 || ਅੰਕ_elements > IPCF_MAX_ELEMENTS)
		ਵਾਪਸ E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_ਥਾਂ(type, &ipcf);
	ਜੇ (ipcf == ਨਿਰਾ)
		ਵਾਪਸ ENOMEM;

	/* Fill details. */
	ipcf->ਅੰਕ_elements = ਅੰਕ_elements;
	ipcf->ਅਗਲਾ = ਨਿਰਾ;
	r = ਅੰਕੜੇ_copy(rp->p_endpoint, ਪਤਾ,
		KERNEL, (vir_ਬਾਈਟਾਂ)ipcf->elements, ਲੰਮਾਈ);
	ਜੇ (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	ਜੇ (r != OK) {
		IPCF_POOL_FREE_ਥਾਂ(ipcf);
		ਵਾਪਸ r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	ਲਈ (ipcfp = &priv(rp)->s_ipcf; *ipcfp != ਨਿਰਾ;
	    ipcfp = &(*ipcfp)->ਅਗਲਾ)
		;
	*ipcfp = ipcf;

	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
ਖਾਲੀ clear_ipc_filters(ਬਣਤਰ proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	ਜਦੋਂ (ipcf != ਨਿਰਾ) {
		curr_ipcf = ipcf;
		ipcf = ipcf->ਅਗਲਾ;
		IPCF_POOL_FREE_ਥਾਂ(curr_ipcf);
	}

	priv(rp)->s_ipcf = ਨਿਰਾ;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	ਜੇ (rp->p_endpoint == VM_PROC_NR && vmrequest != ਨਿਰਾ)
		ਜੇ (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
ਪੂਰਨ check_ipc_filter(ipc_filter_t *ipcf, ਪੂਰਨ fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	ਪੂਰਨ i, ਅੰਕ_elements, flags;

	ਜੇ (ipcf == ਨਿਰਾ)
		ਵਾਪਸ OK;

	ਅੰਕ_elements = ipcf->ਅੰਕ_elements;
	flags = 0;
	ਲਈ (i = 0; i < ਅੰਕ_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		ਜੇ (!IPCF_EL_CHECK(ipcf_el))
			ਵਾਪਸ EINVAL;
		flags |= ipcf_el->flags;
	}

	ਜੇ (fill_flags)
		ipcf->flags = flags;
	ਨਹੀਂਤਾਂ ਜੇ (ipcf->flags != flags)
		ਵਾਪਸ EINVAL;
	ਵਾਪਸ OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
ਪੂਰਨ allow_ipc_filtered_ਸੁਨੇਹਾ(ਬਣਤਰ proc *rp, endpoint_t src_e,
	vir_ਬਾਈਟਾਂ m_src_v, ਸੁਨੇਹਾ *m_src_p)
{
	ਪੂਰਨ i, r, ਅੰਕ_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	ਸੁਨੇਹਾ m_buff;

	ipcf = priv(rp)->s_ipcf;
	ਜੇ (ipcf == ਨਿਰਾ)
		ਵਾਪਸ TRUE; /* no IPC filters, always allow */

	ਜੇ (m_src_p == ਨਿਰਾ) {
		ਪ੍ਰਤਿਜ੍ਞਾ(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		ਕਰੋ {
#ਜੇ DEBUG_DUMPIPCF
			ਜੇ (TRUE) {
#ਨਹੀਂਤਾਂ
			ਜੇ (ipcf->flags & IPCF_ਮੇਲ_M_TYPE) {
#ਜੇ_ਅੰਤ
				get_mtype = TRUE;
				ਰੋਕੋ;
			}
			ipcf = ipcf->ਅਗਲਾ;
		} ਜਦੋਂ (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		ਜੇ (get_mtype) {
			r = ਅੰਕੜੇ_copy(src_e,
			    m_src_v + offsetof(ਸੁਨੇਹਾ, m_type), KERNEL,
			    (vir_ਬਾਈਟਾਂ)&m_buff.m_type, ਆਕਾਰ(m_buff.m_type));
			ਜੇ (r != OK) {
				/* allow for now, this will fail later anyway */
#ਜੇ DEBUG_DUMPIPCF
				ਲਿਖੋ("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#ਜੇ_ਅੰਤ
				ਵਾਪਸ TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_ਸਰੋਤ = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	ਕਰੋ {
		ਜੇ (allow != (ipcf->type == IPCF_WHITELIST)) {
			ਅੰਕ_elements = ipcf->ਅੰਕ_elements;
			ਲਈ (i = 0; i < ਅੰਕ_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				ਜੇ (IPCF_EL_ਮੇਲ(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					ਰੋਕੋ;
				}
			}
		}
		ipcf = ipcf->ਅਗਲਾ;
	} ਜਦੋਂ (ipcf);

#ਜੇ DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
ਪੂਰਨ allow_ipc_filtered_memreq(ਬਣਤਰ proc *src_rp, ਬਣਤਰ proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	ਬਣਤਰ proc *vmp;
	ਸੁਨੇਹਾ m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	ਜੇ (priv(vmp)->s_ipcf == ਨਿਰਾ)
		ਵਾਪਸ TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_ਸੁਨੇਹਾ;
	ਜੇ (!allow_ipc_filtered_ਸੁਨੇਹਾ(vmp, SYSTEM, 0, &m_buf))
		ਵਾਪਸ FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	ਵਾਪਸ TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
ਪੂਰਨ priv_ਜੋੜੋ_irq(ਬਣਤਰ proc *rp, ਪੂਰਨ irq)
{
        ਬਣਤਰ priv *priv = priv(rp);
        ਪੂਰਨ i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	ਲਈ (i = 0; i < priv->s_nr_irq; i++) {
		ਜੇ (priv->s_irq_tab[i] == irq)
			ਵਾਪਸ OK;
	}

	i= priv->s_nr_irq;
	ਜੇ (i >= NR_IRQ) {
		ਲਿਖੋ("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		ਵਾਪਸ ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	ਵਾਪਸ OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
ਪੂਰਨ priv_ਜੋੜੋ_io(ਬਣਤਰ proc *rp, ਬਣਤਰ io_range *ior)
{
        ਬਣਤਰ priv *priv = priv(rp);
        ਪੂਰਨ i;

	priv->s_flags |= CHECK_IO_ਪੋਰਟ;	/* Check I/O accesses */

	ਲਈ (i = 0; i < priv->s_nr_io_range; i++) {
		ਜੇ (priv->s_io_tab[i].ior_ਅਧਾਰ == ior->ior_ਅਧਾਰ &&
			priv->s_io_tab[i].ior_ਹੱਦਬੰਦੀ == ior->ior_ਹੱਦਬੰਦੀ)
			ਵਾਪਸ OK;
	}

	i= priv->s_nr_io_range;
	ਜੇ (i >= NR_IO_RANGE) {
		ਲਿਖੋ("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		ਵਾਪਸ ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	ਵਾਪਸ OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
ਪੂਰਨ priv_ਜੋੜੋ_mem(ਬਣਤਰ proc *rp, ਬਣਤਰ minix_mem_range *memr)
{
        ਬਣਤਰ priv *priv = priv(rp);
        ਪੂਰਨ i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	ਲਈ (i = 0; i < priv->s_nr_mem_range; i++) {
		ਜੇ (priv->s_mem_tab[i].mr_ਅਧਾਰ == memr->mr_ਅਧਾਰ &&
			priv->s_mem_tab[i].mr_ਹੱਦਬੰਦੀ == memr->mr_ਹੱਦਬੰਦੀ)
			ਵਾਪਸ OK;
	}

	i= priv->s_nr_mem_range;
	ਜੇ (i >= NR_MEM_RANGE) {
		ਲਿਖੋ("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		ਵਾਪਸ ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	ਵਾਪਸ OK;
}

