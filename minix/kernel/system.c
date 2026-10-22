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

#شامل "kernel/system.h"
#شامل "kernel/vm.h"
#شامل "kernel/clock.h"
#شامل <stdlib.h>
#شامل <stddef.h>
#شامل <assert.h>
#شامل <signal.h>
#شامل <unistd.h>
#شامل <minix/endpoint.h>
#شامل <minix/safecopies.h>

/* Declaration of the call vector that defines the mapping of system calls
 * to handler functions. The vector is initialized in sys_init() with map(),
 * which makes sure the system call numbers are ok. No space is allocated,
 * because the dummy is declared extern. If an illegal call is given, the
 * array size will be negative and this won't compile.
 */
ساکن صحیح (*call_vec[NR_SYS_CALLS])(ساخت proc * caller, پیغام *m_ptr);

#تعریف_کرو map(call_nr, handler) 					\
    {	صحیح call_اشاریہ = call_nr-KERNEL_CALL; 				\
    	أكد(call_اشاریہ >= 0 && call_اشاریہ < NR_SYS_CALLS);			\
    call_vec[call_اشاریہ] = (handler)  ; }

ساکن خالی kernel_call_اختتام(ساخت proc * caller, پیغام *پیغام_2, صحیح نتیجہ)
{
  اگر(نتیجہ == VMSUSPEND) {
	  /* Special case: message has to be saved for handling
	   * until VM tells us it's allowed. VM has been notified
	   * and we must wait for its reply to restart the call.
	   */
	  أكد(RTS_ISSET(caller, RTS_VMREQUEST));
	  أكد(caller->p_vmrequest.type == VMSTYPE_KERNELCALL);
	  caller->p_vmrequest.saved.reqmsg = *پیغام_2;
	  caller->p_misc_flags |= MF_KCALL_RESUME;
  } ورنہ {
	  /*
	   * call is finished, we could have been suspended because of VM,
	   * remove the request message
	   */
	  caller->p_vmrequest.saved.reqmsg.m_ماخذ = NONE;
	  اگر (نتیجہ != EDONTREPLY) {
		  /* copy the result as a message to the original user buffer */
		  پیغام_2->m_ماخذ = SYSTEM;
		  پیغام_2->m_type = نتیجہ;		/* report status of call */
#اگر DEBUG_IPC_HOOK
	hook_ipc_msgkresult(پیغام_2, caller);
#اگر_ختم
		  اگر (copy_پیغام_مقصد_user(پیغام_2, (پیغام *)caller->p_delivermsg_vir)) {
			  لکھو("WARNING wrong user pointer 0x%08x from "
					  "process %s / %d\n",
					  caller->p_delivermsg_vir,
					  caller->p_name,
					  caller->p_endpoint);
			  cause_sig(proc_nr(caller), SIGSEGV);
		  }
	  }
  }
}

ساکن صحیح kernel_call_dispatch(ساخت proc * caller, پیغام *پیغام_2)
{
  صحیح نتیجہ = OK;
  صحیح call_nr;

#اگر DEBUG_IPC_HOOK
	hook_ipc_msgkcall(پیغام_2, caller);
#اگر_ختم
  call_nr = پیغام_2->m_type - KERNEL_CALL;

  /* See if the caller made a valid request and try to handle it. */
  اگر (call_nr < 0 || call_nr >= NR_SYS_CALLS) {	/* check call number */
	  لکھو("SYSTEM: illegal request %d from %d.\n",
			  call_nr,پیغام_2->m_ماخذ);
	  نتیجہ = EBADREQUEST;			/* illegal message type */
  }
  ورنہ اگر (!GET_ثنائی_ہندسہ(priv(caller)->s_k_call_نقاب, call_nr)) {
	  لکھو("SYSTEM: denied request %d from %d.\n",
			  call_nr,پیغام_2->m_ماخذ);
	  نتیجہ = ECALLDENIED;			/* illegal message type */
  } ورنہ {
	  /* handle the system call */
	  اگر (call_vec[call_nr])
		  نتیجہ = (*call_vec[call_nr])(caller, پیغام_2);
	  ورنہ {
		  لکھو("Unused kernel call %d from %d\n",
				  call_nr, caller->p_endpoint);
		  نتیجہ = EBADREQUEST;
	  }
  }

  واپس نتیجہ;
}

/*===========================================================================*
 *				kernel_call				     *
 *===========================================================================*/
/*
 * this function checks the basic syscall parameters and if accepted it
 * dispatches its handling to the right handler
 */
خالی kernel_call(پیغام *m_user, ساخت proc * caller)
{
  صحیح نتیجہ = OK;
  پیغام پیغام_2;

  caller->p_delivermsg_vir = (vir_بائٹیں) m_user;
  /*
   * the ldt and cr3 of the caller process is loaded because it just've trapped
   * into the kernel or was already set in switch_to_user() before we resume
   * execution of an interrupted kernel call
   */
  اگر (copy_پیغام_from_user(m_user, &پیغام_2) == 0) {
	  پیغام_2.m_ماخذ = caller->p_endpoint;
	  نتیجہ = kernel_call_dispatch(caller, &پیغام_2);
  }
  ورنہ {
	  لکھو("WARNING wrong user pointer 0x%08x from process %s / %d\n",
			  m_user, caller->p_name, caller->p_endpoint);
	  cause_sig(proc_nr(caller), SIGSEGV);
	  واپس;
  }


  /* remember who invoked the kcall so we can bill it its time */
  kbill_kcall = caller;

  kernel_call_اختتام(caller, &پیغام_2, نتیجہ);
}

/*===========================================================================*
 *				initialize				     *
 *===========================================================================*/
خالی system_init(خالی)
{
  اندراج ساخت priv *sp;
  صحیح i;

  /* Initialize IRQ handler hooks. Mark all hooks available. */
  برائے (i=0; i<NR_IRQ_HOOKS; i++) {
      irq_hooks[i].proc_nr_e = NONE;
  }

  /* Initialize all alarm timers for all processes. */
  برائے (sp=BEG_PRIV_ADDR; sp < انتہا_PRIV_ADDR; sp++) {
    tmr_inittimer(&(sp->s_alarm_timer));
  }

  /* Initialize the call vector to a safe default handler. Some system calls
   * may be disabled or nonexistant. Then explicitly map known calls to their
   * handler functions. This is done with a macro that gives a compile error
   * if an illegal call number is used. The ordering is not important here.
   */
  برائے (i=0; i<NR_SYS_CALLS; i++) {
      call_vec[i] = تہی;
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
#اگر defined(__i386__)
  map(SYS_DEVIO, do_devio);   		/* inb, inw, inl, outb, outw, outl */
  map(SYS_VDEVIO, do_vdevio);  		/* vector with devio requests */
#اگر_ختم

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
  map(SYS_SAFECOPYTO, do_safecopy_مقصد);	/* copy with pre-granted permission */
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
#اگر defined(__arm__)
  map(SYS_PADCONF, do_padconf);		/* configure pinmux */
#اگر_ختم

  /* i386-specific. */
#اگر defined(__i386__)
  map(SYS_READBIOS, do_readbios);	/* read from BIOS locations */
  map(SYS_IOPENABLE, do_iopenable); 	/* Enable I/O */
  map(SYS_SDEVIO, do_sdevio);		/* phys_insb, _insw, _outsb, _outsw */
#اگر_ختم

  /* Machine state switching. */
  map(SYS_SETMCONTEXT, do_setmcontext); /* set machine context */
  map(SYS_GETMCONTEXT, do_getmcontext); /* get machine context */

  /* Scheduling */
  map(SYS_جدول_وقت, do_جدول_وقت);	/* reschedule a process */
  map(SYS_SCHEDCTL, do_schedctl);	/* change process scheduler */

}
/*===========================================================================*
 *				get_priv				     *
 *===========================================================================*/
صحیح get_priv(
  اندراج ساخت proc *rc,		/* new (child) process pointer */
  صحیح priv_id				/* privilege id */
)
{
/* Allocate a new privilege structure for a system process. Privilege ids
 * can be assigned either statically or dynamically.
 */
  اندراج ساخت priv *sp;                 /* privilege structure */

  اگر(priv_id == NULL_PRIV_ID) {             /* allocate slot dynamically */
      برائے (sp = BEG_DYN_PRIV_ADDR; sp < انتہا_DYN_PRIV_ADDR; ++sp)
          اگر (sp->s_proc_nr == NONE) توڑو;
      اگر (sp >= انتہا_DYN_PRIV_ADDR) واپس(ENOSPC);
  }
  ورنہ {                                    /* allocate slot from id */
      اگر(!is_static_priv_id(priv_id)) {
          واپس EINVAL;                    /* invalid static priv id */
      }
      اگر(priv[priv_id].s_proc_nr != NONE) {
          واپس EBUSY;                     /* slot already in use */
      }
      sp = &priv[priv_id];
  }
  rc->p_priv = sp;			    /* assign new slot */
  rc->p_priv->s_proc_nr = proc_nr(rc);	    /* set association */

  واپس(OK);
}

/*===========================================================================*
 *				set_sendto_bit				     *
 *===========================================================================*/
خالی سیٹ_sendto_ثنائی_ہندسہ(ثابت ساخت proc *rp, صحیح id)
{
/* Allow a process to send messages to the process(es) associated with the
 * system privilege structure with the given ID.
 */

  /* Disallow the process from sending to a process privilege structure with no
   * associated process, and disallow the process from sending to itself.
   */
  اگر (id_مقصد_nr(id) == NONE || priv_id(rp) == id) {
	unset_sys_ثنائی_ہندسہ(priv(rp)->s_ipc_مقصد, id);
	واپس;
  }

  سیٹ_sys_ثنائی_ہندسہ(priv(rp)->s_ipc_مقصد, id);

  /* The process that this process can now send to, must be able to reply (or
   * vice versa). Therefore, its send mask should be updated as well. Ignore
   * receivers that don't support traps other than RECEIVE, they can't reply
   * or send messages anyway.
   */
  اگر (priv_addr(id)->s_trap_نقاب & ~((1 << RECEIVE)))
      سیٹ_sys_ثنائی_ہندسہ(priv_addr(id)->s_ipc_مقصد, priv_id(rp));
}

/*===========================================================================*
 *				unset_sendto_bit			     *
 *===========================================================================*/
خالی unset_sendto_ثنائی_ہندسہ(ثابت ساخت proc *rp, صحیح id)
{
/* Prevent a process from sending to another process. Retain the send mask
 * symmetry by also unsetting the bit for the other direction.
 */

  unset_sys_ثنائی_ہندسہ(priv(rp)->s_ipc_مقصد, id);

  unset_sys_ثنائی_ہندسہ(priv_addr(id)->s_ipc_مقصد, priv_id(rp));
}

/*===========================================================================*
 *			      fill_sendto_mask				     *
 *===========================================================================*/
خالی fill_sendto_نقاب(ثابت ساخت proc *rp, sys_map_t *map)
{
  صحیح i;

  برائے (i=0; i < NR_SYS_PROCS; i++) {
  	اگر (get_sys_ثنائی_ہندسہ(*map, i))
  		سیٹ_sendto_ثنائی_ہندسہ(rp, i);
  	ورنہ
  		unset_sendto_ثنائی_ہندسہ(rp, i);
  }
}

/*===========================================================================*
 *				send_sig				     *
 *===========================================================================*/
صحیح send_sig(endpoint_t ep, صحیح sig_nr)
{
/* Notify a system process about a signal. This is straightforward. Simply
 * set the signal that is to be delivered in the pending signals map and
 * send a notification with source SYSTEM.
 */
  اندراج ساخت proc *rp;
  ساخت priv *priv;
  صحیح proc_nr;

  اگر(!isokendpt(ep, &proc_nr) || isemptyn(proc_nr))
	واپس EINVAL;

  rp = proc_addr(proc_nr);
  priv = priv(rp);
  اگر(!priv) واپس ENOENT;
  sigaddset(&priv->s_sig_pending, sig_nr);
  mini_notify(proc_addr(SYSTEM), rp->p_endpoint);

  واپس OK;
}

/*===========================================================================*
 *				cause_sig				     *
 *===========================================================================*/
خالی cause_sig(proc_nr_t proc_nr, صحیح sig_nr)
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
  اندراج ساخت proc *rp, *sig_mgr_rp;
  endpoint_t sig_mgr;
  صحیح sig_mgr_proc_nr;
  صحیح s;

  /* Lookup signal manager. */
  rp = proc_addr(proc_nr);
  sig_mgr = priv(rp)->s_sig_mgr;
  اگر(sig_mgr == SELF) sig_mgr = rp->p_endpoint;

  /* If the target is the signal manager of itself, send the signal directly. */
  اگر(rp->p_endpoint == sig_mgr) {
       اگر(SIGS_IS_LETHAL(sig_nr)) {
           /* If the signal is lethal, see if a backup signal manager exists. */
           sig_mgr = priv(rp)->s_bak_sig_mgr;
           اگر(sig_mgr != NONE && isokendpt(sig_mgr, &sig_mgr_proc_nr)) {
               priv(rp)->s_sig_mgr = sig_mgr;
               priv(rp)->s_bak_sig_mgr = NONE;
               sig_mgr_rp = proc_addr(sig_mgr_proc_nr);
               RTS_UNSET(sig_mgr_rp, RTS_NO_PRIV);
               cause_sig(proc_nr, sig_nr); /* try again with the new sig mgr. */
               واپس;
           }
           /* We are out of luck. Time to panic. */
           proc_stacktrace(rp);
           panic("cause_sig: sig manager %d gets lethal signal %d for itself",
	   	rp->p_endpoint, sig_nr);
       }
       sigaddset(&priv(rp)->s_sig_pending, sig_nr);
       اگر(OK != send_sig(rp->p_endpoint, SIGKSIGSM))
       	panic("send_sig failed");
       واپس;
  }

  s = sigismember(&rp->p_pending, sig_nr);
  /* Check if the signal is already pending. Process it otherwise. */
  اگر (!s) {
      sigaddset(&rp->p_pending, sig_nr);
      اگر (! (RTS_ISSET(rp, RTS_SIGNALED))) {		/* other pending */
	  RTS_سیٹ(rp, RTS_SIGNALED | RTS_SIG_PENDING);
          اگر(OK != send_sig(sig_mgr, SIGKSIG))
	  	panic("send_sig failed");
      }
  }
}

/*===========================================================================*
 *				sig_delay_done				     *
 *===========================================================================*/
خالی sig_delay_مکمل(ساخت proc *rp)
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
خالی send_diag_sig(خالی)
{
/* Send a SIGKMESS signal to all processes in receiving updates about new
 * diagnostics messages.
 */
  ساخت priv *privp;
  endpoint_t ep;

  برائے (privp = BEG_PRIV_ADDR; privp < انتہا_PRIV_ADDR; privp++) {
	اگر (privp->s_proc_nr != NONE && privp->s_diag_sig == TRUE) {
		ep = proc_addr(privp->s_proc_nr)->p_endpoint;
		send_sig(ep, SIGKMESS);
	}
  }
}

/*===========================================================================*
 *			         clear_memreq				     *
 *===========================================================================*/
ساکن خالی clear_memreq(ساخت proc *rp)
{
  ساخت proc **rpp;

  اگر (!RTS_ISSET(rp, RTS_VMREQUEST))
	واپس; /* nothing to do */

  برائے (rpp = &vmrequest; *rpp != تہی;
     rpp = &(*rpp)->p_vmrequest.nextrequestor) {
	اگر (*rpp == rp) {
		*rpp = rp->p_vmrequest.nextrequestor;
		توڑو;
	}
  }

  RTS_UNSET(rp, RTS_VMREQUEST);
}

/*===========================================================================*
 *			         clear_ipc				     *
 *===========================================================================*/
ساکن خالی clear_ipc(
  اندراج ساخت proc *rc	/* slot of process to clean up */
)
{
/* Clear IPC data for a given process slot. */
  ساخت proc **xpp;			/* iterate over caller queue */

  اگر (RTS_ISSET(rc, RTS_SENDING)) {
      صحیح ہدف_proc;

      okendpt(rc->p_sendto_e, &ہدف_proc);
      xpp = &proc_addr(ہدف_proc)->p_caller_q; /* destination's queue */
      جب (*xpp) {		/* check entire queue */
          اگر (*xpp == rc) {			/* process is on the queue */
              *xpp = (*xpp)->p_q_link;		/* replace by next process */
#اگر DEBUG_ENABLE_IPC_WARNINGS
	      لکھو("endpoint %d / %s removed from queue at %d\n",
	          rc->p_endpoint, rc->p_name, rc->p_sendto_e);
#اگر_ختم
              توڑو;				/* can only be queued once */
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
خالی clear_endpoint(ساخت proc * rc)
{
/* Clean up the slot of the process given as 'rc'. */
  اگر(isemptyp(rc)) panic("clear_proc: empty process: %d",  rc->p_endpoint);


#اگر DEBUG_IPC_HOOK
  hook_ipc_clear(rc);
#اگر_ختم

  /* Make sure that the exiting process is no longer scheduled. */
  RTS_سیٹ(rc, RTS_NO_ENDPOINT);
  اگر (priv(rc)->s_flags & SYS_PROC)
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
خالی clear_ipc_refs(
  اندراج ساخت proc *rc,		/* slot of process to clean up */
  صحیح caller_ret			/* code to return on callers */
)
{
/* Clear IPC references for a given process slot. */
  ساخت proc *rp;			/* iterate over process table */
  صحیح src_id;

  /* Tell processes that sent asynchronous messages to 'rc' they are not
   * going to be delivered */
  جب ((src_id = has_pending_asend(rc, ANY)) != NULL_PRIV_ID)
      cancel_async(proc_addr(id_مقصد_nr(src_id)), rc);

  برائے (rp = BEG_PROC_ADDR; rp < انتہا_PROC_ADDR; rp++) {
      اگر(isemptyp(rp))
	جاری;

      /* Unset pending notification bits. */
      unset_sys_ثنائی_ہندسہ(priv(rp)->s_notify_pending, priv(rc)->s_id);

      /* Unset pending asynchronous messages */
      unset_sys_ثنائی_ہندسہ(priv(rp)->s_asyn_pending, priv(rc)->s_id);

      /* Check if process depends on given process. */
      اگر (P_BLOCKEDON(rp) == rc->p_endpoint) {
          rp->p_reg.retreg = caller_ret;	/* return requested code */
	  clear_ipc(rp);
      }
  }
}

/*===========================================================================*
 *                              kernel_call_resume                           *
 *===========================================================================*/
خالی kernel_call_resume(ساخت proc *caller)
{
	صحیح نتیجہ;

	أكد(!RTS_ISSET(caller, RTS_خانہ_FREE));
	أكد(!RTS_ISSET(caller, RTS_VMREQUEST));

	أكد(caller->p_vmrequest.saved.reqmsg.m_ماخذ == caller->p_endpoint);

	/*
	printf("KERNEL_CALL restart from %s / %d rts 0x%08x misc 0x%08x\n",
			caller->p_name, caller->p_endpoint,
			caller->p_rts_flags, caller->p_misc_flags);
	 */

	/* re-execute the kernel call, with MF_KCALL_RESUME still set so
	 * the call knows this is a retry.
	 */
	نتیجہ = kernel_call_dispatch(caller, &caller->p_vmrequest.saved.reqmsg);
	/*
	 * we are resuming the kernel call so we have to remove this flag so it
	 * can be set again
	 */
	caller->p_misc_flags &= ~MF_KCALL_RESUME;
	kernel_call_اختتام(caller, &caller->p_vmrequest.saved.reqmsg, نتیجہ);
}

/*===========================================================================*
 *                               sched_proc                                  *
 *===========================================================================*/
صحیح sched_proc(ساخت proc *p, صحیح ترجیح, صحیح وقتی_حصہ, صحیح cpu, صحیح niced)
{
	/* Make sure the values given are within the allowed range.*/
	اگر ((ترجیح < TASK_Q && ترجیح != -1) || ترجیح > NR_SCHED_QUEUES)
		واپس(EINVAL);

	اگر (وقتی_حصہ < 1 && وقتی_حصہ != -1)
		واپس(EINVAL);

#اگر_تعریف_شدہ CONFIG_SMP
	اگر ((cpu < 0 && cpu != -1) || (cpu > 0 && (غیرعلامتی) cpu >= ncpus))
		واپس(EINVAL);
	اگر (cpu != -1 && !(cpu_is_ready(cpu)))
		واپس EBADCPU;
#اگر_ختم

	/* In some cases, we might be rescheduling a runnable process. In such
	 * a case (i.e. if we are updating the priority) we set the NO_QUANTUM
	 * flag before the generic unset to dequeue/enqueue the process
	 */

	/* FIXME this preempts the process, do we really want to do that ?*/

	/* FIXME this is a problem for SMP if the processes currently runs on a
	 * different CPU */
	اگر (proc_is_runnable(p)) {
#اگر_تعریف_شدہ CONFIG_SMP
		اگر (p->p_cpu != cpuid && cpu != -1 && cpu != p->p_cpu) {
			smp_جدول_وقت_migrate_proc(p, cpu);
		}
#اگر_ختم

		RTS_سیٹ(p, RTS_NO_وقتی_حصہ);
	}

	اگر (proc_is_runnable(p))
		RTS_سیٹ(p, RTS_NO_وقتی_حصہ);

	اگر (ترجیح != -1)
		p->p_ترجیح = ترجیح;
	اگر (وقتی_حصہ != -1) {
		p->p_وقتی_حصہ_سائز_ms = وقتی_حصہ;
		p->p_cpu_وقت_بایاں = ms_2_cpu_وقت(وقتی_حصہ);
	}
#اگر_تعریف_شدہ CONFIG_SMP
	اگر (cpu != -1)
		p->p_cpu = cpu;
#اگر_ختم

	اگر (niced)
		p->p_misc_flags |= MF_NICED;
	ورنہ
		p->p_misc_flags &= ~MF_NICED;

	/* Clear the scheduling bit and enqueue the process */
	RTS_UNSET(p, RTS_NO_وقتی_حصہ);

	واپس OK;
}

/*===========================================================================*
 *				add_ipc_filter				     *
 *===========================================================================*/
صحیح جوڑو_ipc_filter(ساخت proc *rp, صحیح type, vir_بائٹیں پتہ,
	size_t لمبائی)
{
	صحیح عدد_elements, r;
	ipc_filter_t *ipcf, **ipcfp;

	/* Validate arguments. */
	اگر (type != IPCF_BLACKLIST && type != IPCF_WHITELIST)
		واپس EINVAL;

	اگر (لمبائی % حجم(ipc_filter_el_t) != 0)
		واپس EINVAL;

	عدد_elements = لمبائی / حجم(ipc_filter_el_t);
	اگر (عدد_elements <= 0 || عدد_elements > IPCF_MAX_ELEMENTS)
		واپس E2BIG;

	/* Allocate a new IPC filter slot. */
	IPCF_POOL_ALLOCATE_خانہ(type, &ipcf);
	اگر (ipcf == تہی)
		واپس ENOMEM;

	/* Fill details. */
	ipcf->عدد_elements = عدد_elements;
	ipcf->اگلا = تہی;
	r = ڈیٹا_copy(rp->p_endpoint, پتہ,
		KERNEL, (vir_بائٹیں)ipcf->elements, لمبائی);
	اگر (r == OK)
		r = check_ipc_filter(ipcf, TRUE /*fill_flags*/);
	اگر (r != OK) {
		IPCF_POOL_FREE_خانہ(ipcf);
		واپس r;
	}

	/* Add the new filter at the end of the IPC filter chain. */
	برائے (ipcfp = &priv(rp)->s_ipcf; *ipcfp != تہی;
	    ipcfp = &(*ipcfp)->اگلا)
		;
	*ipcfp = ipcf;

	واپس OK;
}

/*===========================================================================*
 *				clear_ipc_filters			     *
 *===========================================================================*/
خالی clear_ipc_filters(ساخت proc *rp)
{
	ipc_filter_t *curr_ipcf, *ipcf;

	ipcf = priv(rp)->s_ipcf;
	جب (ipcf != تہی) {
		curr_ipcf = ipcf;
		ipcf = ipcf->اگلا;
		IPCF_POOL_FREE_خانہ(curr_ipcf);
	}

	priv(rp)->s_ipcf = تہی;

	/* VM is a special case here: since the cleared IPC filter may have
	 * blocked memory handling requests, we may now have to tell VM that
	 * there are "new" requests pending.
	 */
	اگر (rp->p_endpoint == VM_PROC_NR && vmrequest != تہی)
		اگر (send_sig(VM_PROC_NR, SIGKMEM) != OK)
			panic("send_sig failed");
}

/*===========================================================================*
 *				check_ipc_filter			     *
 *===========================================================================*/
صحیح check_ipc_filter(ipc_filter_t *ipcf, صحیح fill_flags)
{
	ipc_filter_el_t *ipcf_el;
	صحیح i, عدد_elements, flags;

	اگر (ipcf == تہی)
		واپس OK;

	عدد_elements = ipcf->عدد_elements;
	flags = 0;
	برائے (i = 0; i < عدد_elements; i++) {
		ipcf_el = &ipcf->elements[i];
		اگر (!IPCF_EL_CHECK(ipcf_el))
			واپس EINVAL;
		flags |= ipcf_el->flags;
	}

	اگر (fill_flags)
		ipcf->flags = flags;
	ورنہ اگر (ipcf->flags != flags)
		واپس EINVAL;
	واپس OK;
}

/*===========================================================================*
 *				allow_ipc_filtered_msg			     *
 *===========================================================================*/
صحیح allow_ipc_filtered_پیغام(ساخت proc *rp, endpoint_t src_e,
	vir_بائٹیں m_src_v, پیغام *m_src_p)
{
	صحیح i, r, عدد_elements, get_mtype, allow;
	ipc_filter_t *ipcf;
	ipc_filter_el_t *ipcf_el;
	پیغام m_buff;

	ipcf = priv(rp)->s_ipcf;
	اگر (ipcf == تہی)
		واپس TRUE; /* no IPC filters, always allow */

	اگر (m_src_p == تہی) {
		أكد(m_src_v != 0);

		/* Should we copy in the message type? */
		get_mtype = FALSE;
		کرو {
#اگر DEBUG_DUMPIPCF
			اگر (TRUE) {
#ورنہ
			اگر (ipcf->flags & IPCF_مطابقت_M_TYPE) {
#اگر_ختم
				get_mtype = TRUE;
				توڑو;
			}
			ipcf = ipcf->اگلا;
		} جب (ipcf);
		ipcf = priv(rp)->s_ipcf; /* reset to start */

		/* If so, copy it in from the process. */
		اگر (get_mtype) {
			r = ڈیٹا_copy(src_e,
			    m_src_v + offsetof(پیغام, m_type), KERNEL,
			    (vir_بائٹیں)&m_buff.m_type, حجم(m_buff.m_type));
			اگر (r != OK) {
				/* allow for now, this will fail later anyway */
#اگر DEBUG_DUMPIPCF
				لکھو("KERNEL: allow_ipc_filtered_msg: data "
				    "copy error %d, allowing message...\n", r);
#اگر_ختم
				واپس TRUE;
			}
		}
		m_src_p = &m_buff;
	}

	m_src_p->m_ماخذ = src_e;

	/* See if the message is allowed. */
	allow = (ipcf->type == IPCF_BLACKLIST);
	کرو {
		اگر (allow != (ipcf->type == IPCF_WHITELIST)) {
			عدد_elements = ipcf->عدد_elements;
			برائے (i = 0; i < عدد_elements; i++) {
				ipcf_el = &ipcf->elements[i];
				اگر (IPCF_EL_مطابقت(ipcf_el, m_src_p)) {
					allow = (ipcf->type == IPCF_WHITELIST);
					توڑو;
				}
			}
		}
		ipcf = ipcf->اگلا;
	} جب (ipcf);

#اگر DEBUG_DUMPIPCF
	printmsg(m_src_p, proc_addr(_ENDPOINT_P(src_e)), rp, allow ? '+' : '-',
	    TRUE /*printparams*/);
#اگر_ختم

	واپس allow;
}

/*===========================================================================*
 *			  allow_ipc_filtered_memreq			     *
 *===========================================================================*/
صحیح allow_ipc_filtered_memreq(ساخت proc *src_rp, ساخت proc *dst_rp)
{
	/* Determine whether VM should receive a request to handle memory
	 * that is the result of process 'src_rp' trying to access currently
	 * unavailable memory in process 'dst_rp'. Return TRUE if VM should
	 * be given the request, FALSE otherwise.
	 */

	ساخت proc *vmp;
	پیغام m_buf;

	vmp = proc_addr(VM_PROC_NR);

	/* If VM has no filter in place, all requests should go through. */
	اگر (priv(vmp)->s_ipcf == تہی)
		واپس TRUE;

	/* VM obtains memory requests in response to a SIGKMEM signal, which
	 * is a notification sent from SYSTEM. Thus, if VM blocks such
	 * notifications, it also should not get any memory requests. Of
	 * course, VM should not be asking for requests in that case either,
	 * but the extra check doesn't hurt.
	 */
	m_buf.m_type = NOTIFY_پیغام;
	اگر (!allow_ipc_filtered_پیغام(vmp, SYSTEM, 0, &m_buf))
		واپس FALSE;

	/* A more refined policy may be implemented here, for example to
	 * ensure that both the source and the destination (if different)
	 * are in the group of processes that VM wants to talk to. Since VM
	 * is basically not able to handle any memory requests during an
	 * update, we will not get here, and none of that is needed.
	 */
	واپس TRUE;
}

/*===========================================================================*
 *                             priv_add_irq                                  *
 *===========================================================================*/
صحیح priv_جوڑو_irq(ساخت proc *rp, صحیح irq)
{
        ساخت priv *priv = priv(rp);
        صحیح i;

	priv->s_flags |= CHECK_IRQ;	/* Check IRQ */

	/* When restarting a driver, check if it already has the permission */
	برائے (i = 0; i < priv->s_nr_irq; i++) {
		اگر (priv->s_irq_tab[i] == irq)
			واپس OK;
	}

	i= priv->s_nr_irq;
	اگر (i >= NR_IRQ) {
		لکھو("do_privctl: %d already has %d irq's.\n",
			rp->p_endpoint, i);
		واپس ENOMEM;
	}
	priv->s_irq_tab[i]= irq;
	priv->s_nr_irq++;
	واپس OK;
}

/*===========================================================================*
 *                             priv_add_io                                   *
 *===========================================================================*/
صحیح priv_جوڑو_io(ساخت proc *rp, ساخت io_range *ior)
{
        ساخت priv *priv = priv(rp);
        صحیح i;

	priv->s_flags |= CHECK_IO_پورٹ;	/* Check I/O accesses */

	برائے (i = 0; i < priv->s_nr_io_range; i++) {
		اگر (priv->s_io_tab[i].ior_بنیاد == ior->ior_بنیاد &&
			priv->s_io_tab[i].ior_حد_بندی == ior->ior_حد_بندی)
			واپس OK;
	}

	i= priv->s_nr_io_range;
	اگر (i >= NR_IO_RANGE) {
		لکھو("do_privctl: %d already has %d i/o ranges.\n",
			rp->p_endpoint, i);
		واپس ENOMEM;
	}

	priv->s_io_tab[i] = *ior;
	priv->s_nr_io_range++;
	واپس OK;
}

/*===========================================================================*
 *                             priv_add_mem                                  *
 *===========================================================================*/
صحیح priv_جوڑو_mem(ساخت proc *rp, ساخت minix_mem_range *memr)
{
        ساخت priv *priv = priv(rp);
        صحیح i;

	priv->s_flags |= CHECK_MEM;	/* Check memory mappings */

	/* When restarting a driver, check if it already has the permission */
	برائے (i = 0; i < priv->s_nr_mem_range; i++) {
		اگر (priv->s_mem_tab[i].mr_بنیاد == memr->mr_بنیاد &&
			priv->s_mem_tab[i].mr_حد_بندی == memr->mr_حد_بندی)
			واپس OK;
	}

	i= priv->s_nr_mem_range;
	اگر (i >= NR_MEM_RANGE) {
		لکھو("do_privctl: %d already has %d mem ranges.\n",
			rp->p_endpoint, i);
		واپس ENOMEM;
	}
	priv->s_mem_tab[i]= *memr;
	priv->s_nr_mem_range++;
	واپس OK;
}

