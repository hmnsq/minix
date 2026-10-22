/* This file contains essentially all of the process and message handling.
 * Together with "mpx.s" it forms the lowest layer of the MINIX kernel.
 * There is one entry point from the outside:
 *
 *   sys_call: 	      a system call, i.e., the kernel is trapped with an INT
 *
 * Changes:
 *   Aug 19, 2005     rewrote scheduling code  (Jorrit N. Herder)
 *   Jul 25, 2005     rewrote system call handling  (Jorrit N. Herder)
 *   May 26, 2005     rewrote message passing functions  (Jorrit N. Herder)
 *   May 24, 2005     new notification system call  (Jorrit N. Herder)
 *   Oct 28, 2004     nonblocking send and receive calls  (Jorrit N. Herder)
 *
 * The code here is critical to make everything work and is important for the
 * overall performance of the system. A large fraction of the code deals with
 * list manipulation. To make this both easy to understand and fast to execute 
 * pointer pointers are used throughout the code. Pointer pointers prevent
 * exceptions for the head or tail of a linked list. 
 *
 *  node_t *queue, *new_node;	// assume these as global variables
 *  node_t **xpp = &queue; 	// get pointer pointer to head of queue 
 *  while (*xpp != NULL) 	// find last pointer of the linked list
 *      xpp = &(*xpp)->next;	// get pointer to next pointer 
 *  *xpp = new_node;		// now replace the end (the NULL pointer) 
 *  new_node->next = NULL;	// and mark the new end of the list
 * 
 * For example, when adding a new node to the end of the list, one normally 
 * makes an exception for an empty list and looks up the end of the list for 
 * nonempty lists. As shown above, this is not required with pointer pointers.
 */

#अन्तर्भाव <stddef.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "vm.h"
#अन्तर्भाव "clock.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <minix/syslib.h>

/* Scheduling and message passing functions */
स्थिर शून्य idle(शून्य);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
स्थिर पूर्णाङ्क mini_receive(संरचना proc *caller_ptr, endpoint_t src,
	message *m_buff_usr, पूर्णाङ्क flags);
स्थिर पूर्णाङ्क mini_senda(संरचना proc *caller_ptr, asynmsg_t *table, माप_प्रकार
	size);
स्थिर पूर्णाङ्क deadlock(पूर्णाङ्क function, पञ्जिका संरचना proc *caller,
	endpoint_t src_dst_e);
स्थिर पूर्णाङ्क try_async(संरचना proc *caller_ptr);
स्थिर पूर्णाङ्क try_one(endpoint_t receive_e, संरचना proc *src_ptr,
	संरचना proc *dst_ptr);
स्थिर संरचना proc * pick_proc(शून्य);
स्थिर शून्य enqueue_head(संरचना proc *rp);

/* all idles share the same idle_priv structure */
स्थिर संरचना priv idle_priv;

स्थिर शून्य set_idle_name(वर्ण * name, पूर्णाङ्क n)
{
        पूर्णाङ्क i, c;
        पूर्णाङ्क p_z = 0;

        यदि (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        लागि (i = 4, c = 100; c > 0; c /= 10) {
                पूर्णाङ्क digit;

                digit = n / c;  
                n -= digit * c;  

                यदि (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#define PICK_ANY	1
#define PICK_HIGHERONLY	2

#define BuildNotifyMessage(m_ptr, src, dst_ptr) \
	स_रखो((m_ptr), 0, परिमाण(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_MESSAGE;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	विकल्प (src) {							\
	पक्ष HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		रोक;							\
	पक्ष SYSTEM:							\
		स_नकल(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			परिमाण(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		रोक;							\
	}

स्थिर message m_notify_buff = { 0, NOTIFY_MESSAGE };

शून्य proc_init(शून्य)
{
	संरचना proc * rp;
	संरचना priv *sp;
	पूर्णाङ्क i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	लागि (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < END_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_SLOT_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = रिक्त;		/* no user space scheduler */
		rp->p_priority = 0;		/* no priority */
		rp->p_quantum_size_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	लागि (sp = BEG_PRIV_ADDR, i = 0; sp < END_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	लागि (i = 0; i < CONFIG_MAX_CPUS; i++) {
		संरचना proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		set_idle_name(ip->p_name, i);
	}
}

स्थिर शून्य switch_address_space_idle(शून्य)
{
#ifdef CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_address_space(proc_addr(VM_PROC_NR));
#endif
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
स्थिर शून्य idle(शून्य)
{
	संरचना proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	यदि (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_address_space_idle();

#ifdef CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	यदि (cpuid != bsp_cpu_id)
		stop_local_timer();
	अन्यथा
#endif
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#यदि !SPROFILE
	halt_cpu();
#अन्यथा
	यदि (!sprofiling)
		halt_cpu();
	अन्यथा {
		चञ्चल पूर्णाङ्क * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		जबसम्म (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#endif
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
शून्य vm_suspend(संरचना proc *caller, अचर संरचना proc *target,
        अचर vir_bytes linaddr, अचर vir_bytes len, अचर पूर्णाङ्क type,
        अचर पूर्णाङ्क writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        प्रतिज्ञा(!RTS_ISSET(caller, RTS_VMREQUEST));
        प्रतिज्ञा(!RTS_ISSET(target, RTS_VMREQUEST));

        RTS_SET(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.target = target->p_endpoint;
        caller->p_vmrequest.params.check.start = linaddr;
        caller->p_vmrequest.params.check.length = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        यदि(!(caller->p_vmrequest.nextrequestor = vmrequest))
                यदि(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
स्थिर शून्य delivermsg(संरचना proc *rp)
{
        प्रतिज्ञा(!RTS_ISSET(rp, RTS_VMREQUEST));
        प्रतिज्ञा(rp->p_misc_flags & MF_DELIVERMSG);
        प्रतिज्ञा(rp->p_delivermsg.m_source != NONE);

        यदि (copy_सन्देश_to_user(&rp->p_delivermsg,
                                (message *) rp->p_delivermsg_vir)) {
                यदि(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        म_लिखो("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, संक_अंश);
                } अन्यथा {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                परिमाण(message), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } अन्यथा {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_source = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                यदि(!(rp->p_misc_flags & MF_CONTEXT_SET)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
शून्य switch_to_user(शून्य)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	संरचना proc * p;
#ifdef CONFIG_SMP
	पूर्णाङ्क tlb_must_refresh = 0;
#endif

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	यदि (proc_is_runnable(p))
		गच्छ check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	यदि (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		यदि (proc_is_runnable(p)) {
			यदि (p->p_cpu_time_बायाँ)
				enqueue_head(p);
			अन्यथा
				enqueue(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	जबसम्म (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ifdef CONFIG_SMP
	यदि (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#endif
	switch_address_space(p);

check_misc_flags:

	प्रतिज्ञा(p);
	प्रतिज्ञा(proc_is_runnable(p));
	जबसम्म (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ACTIVE)) {

		प्रतिज्ञा(proc_is_runnable(p));
		यदि (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		अन्यथा यदि (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, म_लिखो("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		अन्यथा यदि (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			प्रतिज्ञा (!(p->p_misc_flags & MF_SC_ACTIVE));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			यदि ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_done(p);
		}
		अन्यथा यदि (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			यदि (!(p->p_misc_flags & MF_SC_ACTIVE))
				रोक;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ACTIVE);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		अन्यथा यदि (p->p_misc_flags & MF_SC_ACTIVE) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ACTIVE;

			रोक;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		यदि (!proc_is_runnable(p))
			गच्छ not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	यदि (!p->p_cpu_time_बायाँ)
		proc_no_time(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	यदि (!proc_is_runnable(p))
		गच्छ not_runnable_pick_new;

	TRACE(VF_SCHEDULING, म_लिखो("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#यदि DEBUG_TRACE
	p->p_schedules++;
#endif

	p = arch_finish_switch_to_user();
	प्रतिज्ञा(p->p_cpu_time_बायाँ);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	यदि (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	अन्यथा
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_SET;

#यदि defined(__i386__)
  	प्रतिज्ञा(p->p_seg.p_cr3 != 0);
#अथवायदि defined(__arm__)
	प्रतिज्ञा(p->p_seg.p_ttbr != 0);
#endif
#ifdef CONFIG_SMP
	यदि (p->p_misc_flags & MF_FLUSH_TLB) {
		यदि (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#endif
	
	restart_local_timer();
	
	/*
	 * restore_user_context() carries out the actual mode switch from kernel
	 * to userspace. This function does not return
	 */
	restore_user_context(p);
	NOT_REACHABLE;
}

/*
 * handler for all synchronous IPC calls
 */
स्थिर पूर्णाङ्क do_sync_ipc(संरचना proc * caller_ptr, /* who made the call */
			पूर्णाङ्क call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			message *m_ptr)	/* users pointer to a message */
{
  पूर्णाङ्क result;					/* the system call's result */
  पूर्णाङ्क src_dst_p;				/* Process slot number */
  वर्ण *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  प्रतिज्ञा(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  यदि (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_names[call_nr])) {
#यदि DEBUG_ENABLE_IPC_WARNINGS
      म_लिखो("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#endif
	फर्काऊ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  यदि (src_dst_e == ANY)
  {
	यदि (call_nr != RECEIVE)
	{
#यदि 0
		म_लिखो("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		फर्काऊ EINVAL;
	}
	src_dst_p = (पूर्णाङ्क) src_dst_e;
  }
  अन्यथा
  {
	/* Require a valid source and/or destination process. */
	यदि(!isokendpt(src_dst_e, &src_dst_p)) {
#यदि 0
		म_लिखो("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#endif
		फर्काऊ EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	यदि (call_nr != RECEIVE)
	{
		यदि (!may_send_to(caller_ptr, src_dst_p)) {
#यदि DEBUG_ENABLE_IPC_WARNINGS
			म_लिखो(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#endif
			फर्काऊ(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  यदि (!(priv(caller_ptr)->s_trap_mask & (1 << call_nr))) {
#यदि DEBUG_ENABLE_IPC_WARNINGS
      म_लिखो("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#endif
	फर्काऊ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  यदि (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#यदि DEBUG_ENABLE_IPC_WARNINGS
      म_लिखो("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#endif
	फर्काऊ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  विकल्प(call_nr) {
  पक्ष SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_REPLY_PEND;
	/* fall through */
  पक्ष SEND:			
	result = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	यदि (call_nr == SEND || result != OK)
		रोक;				/* done, or SEND failed */
	/* fall through for SENDREC */
  पक्ष RECEIVE:			
	यदि (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
		IPC_STATUS_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	result = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	रोक;
  पक्ष NOTIFY:
	result = mini_notify(caller_ptr, src_dst_e);
	रोक;
  पक्ष SENDNB:
        result = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        रोक;
  स्वभाव:
	result = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  फर्काऊ(result);
}

पूर्णाङ्क do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  संरचना proc *अचर caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  पूर्णाङ्क call_nr = (पूर्णाङ्क) r1;

  प्रतिज्ञा(!RTS_ISSET(caller_ptr, RTS_SLOT_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  यदि (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	यदि ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		प्रतिज्ञा(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		फर्काऊ caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	प्रतिज्ञा (!(caller_ptr->p_misc_flags & MF_SC_ACTIVE));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ACTIVE;
  }

  यदि(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
	panic("sys_call: MF_DELIVERMSG on for %s / %d\n",
		caller_ptr->p_name, caller_ptr->p_endpoint);
  }

  /* Now check if the call is known and try to perform the request. The only
   * system calls that exist in MINIX are sending and receiving messages.
   *   - SENDREC: combines SEND and RECEIVE in a single system call
   *   - SEND:    sender blocks until its message has been delivered
   *   - RECEIVE: receiver blocks until an acceptable message has arrived
   *   - NOTIFY:  asynchronous call; deliver notification or mark pending
   *   - SENDA:   list of asynchronous send requests
   */
  विकल्प(call_nr) {
  	पक्ष SENDREC:
  	पक्ष SEND:			
  	पक्ष RECEIVE:			
  	पक्ष NOTIFY:
  	पक्ष SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    फर्काऊ do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (message *) r3);
  	}
  	पक्ष SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    माप_प्रकार सन्देश_size = (माप_प्रकार) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    यदि (सन्देश_size > 16*(NR_TASKS + NR_PROCS))
	        फर्काऊ गलत_तर्क;
  	    फर्काऊ mini_senda(caller_ptr, (asynmsg_t *) r3, सन्देश_size);
  	}
  	पक्ष MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	यदि(!minix_kerninfo_user) {
			फर्काऊ EBADCALL;
		}

  		arch_set_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		फर्काऊ OK;
	}
  	स्वभाव:
	फर्काऊ EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
स्थिर पूर्णाङ्क deadlock(
  पूर्णाङ्क function,				/* trap number */
  पञ्जिका संरचना proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  पञ्जिका संरचना proc *xp;			/* process pointer */
  पूर्णाङ्क group_size = 1;				/* start with only caller */
#यदि DEBUG_ENABLE_IPC_WARNINGS
  स्थिर संरचना proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#endif

  जबसम्म (src_dst_e != ANY) { 			/* check while process nr */
      पूर्णाङ्क src_dst_slot;
      okendpt(src_dst_e, &src_dst_slot);
      xp = proc_addr(src_dst_slot);		/* follow chain of processes */
      प्रतिज्ञा(proc_ptr_ok(xp));
      प्रतिज्ञा(!RTS_ISSET(xp, RTS_SLOT_FREE));
#यदि DEBUG_ENABLE_IPC_WARNINGS
      processes[group_size] = xp;
#endif
      group_size ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      यदि((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	फर्काऊ 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      यदि (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  यदि (group_size == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      यदि ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          फर्काऊ(0);			/* not a deadlock */
	      }
	  }
#यदि DEBUG_ENABLE_IPC_WARNINGS
	  {
		पूर्णाङ्क i;
		म_लिखो("deadlock between these processes:\n");
		लागि(i = 0; i < group_size; i++) {
			म_लिखो(" %10s ", processes[i]->p_name);
		}
		म_लिखो("\n\n");
		लागि(i = 0; i < group_size; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#endif
          फर्काऊ(group_size);			/* deadlock found */
      }
  }
  फर्काऊ(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
स्थिर पूर्णाङ्क has_pending(sys_map_t *map, पूर्णाङ्क src_p, पूर्णाङ्क asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  पूर्णाङ्क src_id;
  sys_id_t id = NULL_PRIV_ID;
#ifdef CONFIG_SMP
  संरचना proc * p;
#endif

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  यदि (src_p != ANY) {
	src_id = nr_to_id(src_p);
	यदि (get_sys_bit(*map, src_id)) {
#ifdef CONFIG_SMP
		p = proc_addr(id_to_nr(src_id));
		यदि (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_MISS;
		अन्यथा
#endif
			id = src_id;
	}
  } अन्यथा {
	/* Find a source with a pending message */
	लागि (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_BITS) {
		यदि (get_sys_bits(*map, src_id) != 0) {
#ifdef CONFIG_SMP
			जबसम्म (src_id < NR_SYS_PROCS) {
				जबसम्म (!get_sys_bit(*map, src_id)) {
					यदि (src_id == NR_SYS_PROCS)
						गच्छ quit_खोज;
					src_id++;
				}
				p = proc_addr(id_to_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				यदि (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_MISS;
					src_id++;
				} अन्यथा
					गच्छ quit_खोज;
			}
#अन्यथा
			जबसम्म (!get_sys_bit(*map, src_id)) src_id++;
			गच्छ quit_खोज;
#endif
		}
	}

quit_खोज:
	यदि (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  फर्काऊ(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
पूर्णाङ्क has_pending_notify(संरचना proc * caller, पूर्णाङ्क src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	फर्काऊ has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
पूर्णाङ्क has_pending_asend(संरचना proc * caller, पूर्णाङ्क src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	फर्काऊ has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
शून्य unset_notify_pending(संरचना proc * caller, पूर्णाङ्क src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_bit(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
पूर्णाङ्क mini_send(
  पञ्जिका संरचना proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  message *m_ptr,			/* pointer to message buffer */
  अचर पूर्णाङ्क flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  पञ्जिका संरचना proc *dst_ptr;
  पञ्जिका संरचना proc **xpp;
  पूर्णाङ्क dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  यदि (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	फर्काऊ EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  यदि (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_bytes)m_ptr, रिक्त)) {
	पूर्णाङ्क call;
	/* Destination is indeed waiting for this message. */
	प्रतिज्ञा(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	यदि (!(flags & FROM_KERNEL)) {
		यदि(copy_सन्देश_from_user(m_ptr, &dst_ptr->p_delivermsg))
			फर्काऊ EFAULT;
	} अन्यथा {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_STATUS_ADD_FLAGS(dst_ptr, IPC_FLG_सन्देश_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_REPLY_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_STATUS_ADD_CALL(dst_ptr, call);

	यदि (dst_ptr->p_misc_flags & MF_REPLY_PEND)
		dst_ptr->p_misc_flags &= ~MF_REPLY_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#यदि DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
  } अन्यथा {
	यदि(flags & NON_BLOCKING) {
		फर्काऊ(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	यदि (deadlock(SEND, caller_ptr, dst_e)) {
		फर्काऊ(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	यदि (!(flags & FROM_KERNEL)) {
		यदि(copy_सन्देश_from_user(m_ptr, &caller_ptr->p_sendmsg))
			फर्काऊ EFAULT;
	} अन्यथा {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_SET(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	प्रतिज्ञा(caller_ptr->p_q_link == रिक्त);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	जबसम्म (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#यदि DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#endif
  }
  फर्काऊ(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
स्थिर पूर्णाङ्क mini_receive(संरचना proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			message * m_buff_usr, /* pointer to message buffer */
			अचर पूर्णाङ्क flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  पञ्जिका संरचना proc **xpp;
  पूर्णाङ्क r, src_id, found, src_proc_nr, src_p;
  endpoint_t sender_e;

  प्रतिज्ञा(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_bytes) m_buff_usr;

  यदि(src_e == ANY) src_p = ANY;
  अन्यथा
  {
	okendpt(src_e, &src_p);
	यदि (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		फर्काऊ EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  यदि (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    यदि (! (caller_ptr->p_misc_flags & MF_REPLY_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        found = src_id != NULL_PRIV_ID;
        यदि(found) {
            src_proc_nr = id_to_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        यदि (found && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#यदि DEBUG_ENABLE_IPC_WARNINGS
	    यदि(src_proc_nr == NONE) {
		म_लिखो("mini_receive: sending notify from NONE\n");
	    }
#endif
	    प्रतिज्ञा(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    प्रतिज्ञा(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    प्रतिज्ञा(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    BuildNotifyMessage(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_source = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_STATUS_ADD_CALL(caller_ptr, NOTIFY);

	    गच्छ receive_done;
        }
    }

    /* Check for pending asynchronous messages */
    यदि (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        यदि (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        अन्यथा
        	r = try_async(caller_ptr);

	यदि (r == OK) {
            IPC_STATUS_ADD_CALL(caller_ptr, SENDA);
            गच्छ receive_done;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    जबसम्म (*xpp) {
	संरचना proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        यदि (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            पूर्णाङ्क call;
	    प्रतिज्ञा(!RTS_ISSET(sender, RTS_SLOT_FREE));
	    प्रतिज्ञा(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    प्रतिज्ञा(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_source = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_REPLY_PEND ? SENDREC : SEND);
	    IPC_STATUS_ADD_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    यदि (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_STATUS_ADD_FLAGS(caller_ptr, IPC_FLG_सन्देश_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    यदि (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_done(sender);

#यदि DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#endif
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = रिक्त;
	    गच्छ receive_done;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  यदि ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      यदि (deadlock(RECEIVE, caller_ptr, src_e)) {
          फर्काऊ(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_SET(caller_ptr, RTS_RECEIVING);
      फर्काऊ(OK);
  } अन्यथा {
	फर्काऊ(ENOTREADY);
  }

receive_done:
  यदि (caller_ptr->p_misc_flags & MF_REPLY_PEND)
	  caller_ptr->p_misc_flags &= ~MF_REPLY_PEND;
  फर्काऊ OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
पूर्णाङ्क mini_notify(
  अचर संरचना proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  पञ्जिका संरचना proc *dst_ptr;
  पूर्णाङ्क src_id;				/* source id for late delivery */
  पूर्णाङ्क dst_p;

  यदि (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	म_लिखो("mini_notify: bogus endpoint %d\n", dst_e);
	फर्काऊ EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  यदि (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_REPLY_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      प्रतिज्ञा(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      BuildNotifyMessage(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_STATUS_ADD_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      फर्काऊ(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  set_sys_bit(priv(dst_ptr)->s_notify_pending, src_id); 
  फर्काऊ(OK);
}

#define ASCOMPLAIN(caller, entry, field)	\
	म_लिखो("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
field, caller->p_name, entry, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#define A_RETR(entry) गर {			\
  यदि (data_copy(				\
  		caller_ptr->p_endpoint, table_v + (entry)*परिमाण(asynmsg_t),\
  		KERNEL, (vir_bytes) &tabent,	\
  		परिमाण(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
  			r = EFAULT;		\
	                गच्छ asyn_error; \
  }						\
  अन्यथा यदि(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } जबसम्म(0)

#define A_INSRT(entry) गर {			\
  यदि (data_copy(KERNEL, (vir_bytes) &tabent,	\
  		caller_ptr->p_endpoint, table_v + (entry)*परिमाण(asynmsg_t),\
  		परिमाण(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, entry, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } जबसम्म(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
पूर्णाङ्क try_deliver_senda(संरचना proc *caller_ptr,
				asynmsg_t *table,
				माप_प्रकार size)
{
  पूर्णाङ्क r, dst_p, done, do_notify;
  अचिन्हित पूर्णाङ्क i;
  अचिन्हित flags;
  endpoint_t dst;
  संरचना proc *dst_ptr;
  संरचना priv *privp;
  asynmsg_t tabent;
  अचर vir_bytes table_v = (vir_bytes) table;
  message *m_ptr = रिक्त;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  यदि (size == 0) फर्काऊ(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  यदि (size > 16*(NR_TASKS + NR_PROCS)) {
    r = गलत_तर्क;
    फर्काऊ r;
  }

  लागि (i = 0; i < size; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	यदि (flags == 0) जारी; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	यदि(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		गच्छ asyn_error;
	}
	यदि (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		गच्छ asyn_error;
	}
	यदि (flags & AMF_DONE) जारी;	/* Already done processing */

	r = OK;
	यदि (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	अन्यथा यदि (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	अन्यथा यदि (!may_asynsend_to(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	अन्यथा 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	यदि (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	यदि (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_bytes)&table[i].सन्देश, रिक्त) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_REPLY_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.सन्देश;
		dst_ptr->p_delivermsg.m_source = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_STATUS_ADD_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#यदि DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#endif
	} अन्यथा यदि (r == OK) {
		/* Inform receiver that something is pending */
		set_sys_bit(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		done = FALSE;
		जारी;
	} 

	/* Store results */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	यदि (flags & AMF_NOTIFY)
		do_notify = TRUE;
	अन्यथा यदि (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	जारी;

asyn_error:
	यदि (dst != NONE)
		म_लिखो("KERNEL senda error %d to %d\n", r, dst);
	अन्यथा
		म_लिखो("KERNEL senda error %d\n", r);
  }

  यदि (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  यदि (!done) {
	privp->s_asyntab = (vir_bytes) table;
	privp->s_asynsize = size;
  }

  फर्काऊ(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
स्थिर पूर्णाङ्क mini_senda(संरचना proc *caller_ptr, asynmsg_t *table, माप_प्रकार size)
{
  संरचना priv *privp;

  privp = priv(caller_ptr);
  यदि (!(privp->s_flags & SYS_PROC)) {
	म_लिखो( "mini_senda: warning caller has no privilege structure\n");
	फर्काऊ(EPERM);
  }

  फर्काऊ try_deliver_senda(caller_ptr, table, size);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
स्थिर पूर्णाङ्क try_async(संरचना proc * caller_ptr)
{
  पूर्णाङ्क r;
  संरचना priv *privp;
  संरचना proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  लागि (privp = BEG_PRIV_ADDR; privp < END_PRIV_ADDR; ++privp)  {
	यदि (privp->s_proc_nr == NONE)
		जारी;

	यदि (!get_sys_bit(*map, privp->s_id)) 
		जारी;

	src_ptr = proc_addr(privp->s_proc_nr);

#ifdef CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	यदि (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_MISS;
		जारी;
	}
#endif

	प्रतिज्ञा(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	यदि ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		फर्काऊ(r);
  }

  फर्काऊ(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
स्थिर पूर्णाङ्क try_one(endpoint_t receive_e, संरचना proc *src_ptr,
    संरचना proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  पूर्णाङ्क r = EAGAIN, done, do_notify;
  अचिन्हित पूर्णाङ्क flags, i;
  माप_प्रकार size;
  endpoint_t dst, src_e;
  संरचना proc *caller_ptr;
  संरचना priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  यदि (!(privp->s_flags & SYS_PROC)) फर्काऊ(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  यदि (size == 0) फर्काऊ(EAGAIN);
  यदि (privp->s_asynendpoint != src_ptr->p_endpoint) फर्काऊ EAGAIN;
  यदि (!may_asynsend_to(src_ptr, proc_nr(dst_ptr))) फर्काऊ (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;

  लागि (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	यदि (flags == 0) जारी;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	यदि(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	अन्यथा यदि (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	अन्यथा यदि (flags & AMF_DONE) जारी; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	done = FALSE;

	यदि (r == EINVAL)
		गच्छ store_result;

	/* Message must be directed at receiving end */
	यदि (dst != dst_ptr->p_endpoint) जारी;

	यदि (!CANRECEIVE(receive_e, src_e, dst_ptr,
		table_v + i*परिमाण(asynmsg_t) + दुरत्व(संरचना asynmsg,सन्देश),
		रिक्त)) {
		जारी;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	यदि ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_REPLY_PEND)) 
		जारी;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.सन्देश;
	dst_ptr->p_delivermsg.m_source = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#यदि DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#endif

store_result:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	यदि (flags & AMF_NOTIFY) do_notify = TRUE;
	अन्यथा यदि (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	रोक;
  }

  यदि (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  यदि (done) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } अन्यथा {
	set_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  फर्काऊ(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
पूर्णाङ्क cancel_async(संरचना proc *src_ptr, संरचना proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  पूर्णाङ्क done, do_notify;
  अचिन्हित पूर्णाङ्क flags, i;
  माप_प्रकार size;
  endpoint_t dst;
  संरचना proc *caller_ptr;
  संरचना priv *privp;
  asynmsg_t tabent;
  vir_bytes table_v;

  privp = priv(src_ptr);
  यदि (!(privp->s_flags & SYS_PROC)) फर्काऊ(EPERM);
  size = privp->s_asynsize;
  table_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_bit(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  यदि (size == 0) फर्काऊ(EAGAIN);
  यदि (!may_send_to(src_ptr, proc_nr(dst_ptr))) फर्काऊ(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  done = TRUE;


  लागि (i = 0; i < size; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	पूर्णाङ्क r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	यदि (flags == 0) जारी;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	यदि(flags & ~(AMF_VALID|AMF_DONE|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	अन्यथा यदि (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	अन्यथा यदि (flags & AMF_DONE) जारी; /* Already done processing */

	/* Message must be directed at receiving end */
	यदि (dst != dst_ptr->p_endpoint) {
		done = FALSE;
		जारी;
	}

	/* Store results for sender */
	tabent.result = r;
	tabent.flags = flags | AMF_DONE;
	यदि (flags & AMF_NOTIFY) do_notify = TRUE;
	अन्यथा यदि (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  यदि (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  यदि (!done) {
	privp->s_asyntab = table_v;
	privp->s_asynsize = size;
  }

asyn_error:
  फर्काऊ(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
शून्य enqueue(
  पञ्जिका संरचना proc *rp	/* this process is now runnable */
)
{
/* Add 'rp' to one of the queues of runnable processes.  This function is 
 * responsible for inserting a process into one of the scheduling queues. 
 * The mechanism is implemented here.   The actual scheduling policy is
 * defined in sched() and pick_proc().
 *
 * This function can be used x-cpu as it always uses the queues of the cpu the
 * process is assigned to.
 */
  पूर्णाङ्क q = rp->p_priority;	 		/* scheduling queue to use */
  संरचना proc **rdy_head, **rdy_tail;
  
  प्रतिज्ञा(proc_is_runnable(rp));

  प्रतिज्ञा(q >= 0);

  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  यदि (!rdy_head[q]) {		/* add to empty queue */
      rdy_head[q] = rdy_tail[q] = rp; 		/* create a new queue */
      rp->p_nextready = रिक्त;		/* mark new end */
  } 
  अन्यथा {					/* add to tail of queue */
      rdy_tail[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_tail[q] = rp;				/* set new queue tail */
      rp->p_nextready = रिक्त;		/* mark new end */
  }

  यदि (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  संरचना proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  प्रतिज्ञा(p);
	  यदि((p->p_priority > rp->p_priority) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_SET(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ifdef CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  अन्यथा यदि (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_schedule(rp->p_cpu);
  }
#endif

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_queue));


#यदि DEBUG_SANITYCHECKS
  प्रतिज्ञा(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				enqueue_head				     *
 *===========================================================================*/
/*
 * put a process at the front of its run queue. It comes handy when a process is
 * preempted and removed from run queue to not to have a currently not-runnable
 * process on a run queue. We have to put this process back at the fron to be
 * fair
 */
स्थिर शून्य enqueue_head(संरचना proc *rp)
{
  अचर पूर्णाङ्क q = rp->p_priority;	 		/* scheduling queue to use */

  संरचना proc **rdy_head, **rdy_tail;

  प्रतिज्ञा(proc_ptr_ok(rp));
  प्रतिज्ञा(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  प्रतिज्ञा(rp->p_cpu_time_बायाँ);

  प्रतिज्ञा(q >= 0);


  rdy_head = get_cpu_var(rp->p_cpu, run_q_head);
  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now add the process to the queue. */
  यदि (!rdy_head[q]) {		/* add to empty queue */
	rdy_head[q] = rdy_tail[q] = rp; 	/* create a new queue */
	rp->p_nextready = रिक्त;			/* mark new end */
  } अन्यथा {					/* add to head of queue */
	rp->p_nextready = rdy_head[q];		/* chain head of queue */
	rdy_head[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_queue)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#यदि DEBUG_SANITYCHECKS
  प्रतिज्ञा(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
शून्य dequeue(संरचना proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  पूर्णाङ्क q = rp->p_priority;		/* queue to use */
  संरचना proc **xpp;			/* iterate over queue */
  संरचना proc *prev_xp;
  u64_t tsc, tsc_delta;

  संरचना proc **rdy_tail;

  प्रतिज्ञा(proc_ptr_ok(rp));
  प्रतिज्ञा(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  प्रतिज्ञा (!iskernelp(rp) || *priv(rp)->s_थाक_guard == थाक_GUARD);

  rdy_tail = get_cpu_var(rp->p_cpu, run_q_tail);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = रिक्त;				
  लागि (xpp = get_cpu_var_ptr(rp->p_cpu, run_q_head[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      यदि (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          यदि (rp == rdy_tail[q]) {		/* queue tail removed */
              rdy_tail[q] = prev_xp;		/* set new tail */
	  }

          रोक;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  यदि (rp->p_accounting.enter_queue) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_queue;
	rp->p_accounting.time_in_queue = rp->p_accounting.time_in_queue +
		tsc_delta;
	rp->p_accounting.enter_queue = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#यदि DEBUG_SANITYCHECKS
  प्रतिज्ञा(runqueues_ok_local());
#endif
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
स्थिर संरचना proc * pick_proc(शून्य)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  पञ्जिका संरचना proc *rp;			/* process to run */
  संरचना proc **rdy_head;
  पूर्णाङ्क q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_head = get_cpulocal_var(run_q_head);
  लागि (q=0; q < NR_SCHED_QUEUES; q++) {	
	यदि(!(rp = rdy_head[q])) {
		TRACE(VF_PICKPROC, म_लिखो("cpu %d queue %d empty\n", cpuid, q););
		जारी;
	}
	प्रतिज्ञा(proc_is_runnable(rp));
	यदि (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	फर्काऊ rp;
  }
  फर्काऊ रिक्त;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
संरचना proc *endpoint_lookup(endpoint_t e)
{
	पूर्णाङ्क n;

	यदि(!isokendpt(e, &n)) फर्काऊ रिक्त;

	फर्काऊ proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#यदि DEBUG_ENABLE_IPC_WARNINGS
पूर्णाङ्क isokendpt_f(अचर वर्ण * file, पूर्णाङ्क पङ्क्ति, endpoint_t e, पूर्णाङ्क * p,
	अचर पूर्णाङ्क fatalflag)
#अन्यथा
पूर्णाङ्क isokendpt_f(endpoint_t e, पूर्णाङ्क * p, अचर पूर्णाङ्क fatalflag)
#endif
{
	पूर्णाङ्क ok = 0;
	/* Convert an endpoint number into a process number.
	 * Return nonzero if the process is alive with the corresponding
	 * generation number, zero otherwise.
	 *
	 * This function is called with file and line number by the
	 * isokendpt_d macro if DEBUG_ENABLE_IPC_WARNINGS is defined,
	 * otherwise without. This allows us to print the where the
	 * conversion was attempted, making the errors verbose without
	 * adding code for that at every call.
	 * 
	 * If fatalflag is nonzero, we must panic if the conversion doesn't
	 * succeed.
	 */
	*p = _ENDPOINT_P(e);
	ok = 0;
	यदि(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	यदि(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	फर्काऊ ok;
}

स्थिर शून्य notify_scheduler(संरचना proc *p)
{
	message m_no_quantum;
	पूर्णाङ्क err;

	प्रतिज्ञा(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_SET(p, RTS_NO_QUANTUM);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_quantum.m_source = p->p_endpoint;
	m_no_quantum.m_type   = SCHEDULING_NO_QUANTUM;
	m_no_quantum.m_krn_lsys_schedule.acnt_queue = cpu_time_2_ms(p->p_accounting.time_in_queue);
	m_no_quantum.m_krn_lsys_schedule.acnt_deqs      = p->p_accounting.dequeues;
	m_no_quantum.m_krn_lsys_schedule.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_quantum.m_krn_lsys_schedule.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_quantum.m_krn_lsys_schedule.acnt_preempt   = p->p_accounting.preempted;
	m_no_quantum.m_krn_lsys_schedule.acnt_cpu       = cpuid;
	m_no_quantum.m_krn_lsys_schedule.acnt_cpu_load  = cpu_load();

	/* Reset accounting */
	reset_proc_accounting(p);

	यदि ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_quantum, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

शून्य proc_no_time(संरचना proc * p)
{
	यदि (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	अन्यथा {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_time_बायाँ = ms_2_cpu_time(p->p_quantum_size_ms);
#यदि DEBUG_RACE
		RTS_SET(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#endif
	}
}

शून्य reset_proc_accounting(संरचना proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.time_in_queue = 0;
  p->p_accounting.enter_queue = 0;
}
	
शून्य copr_not_available_handler(शून्य)
{
	संरचना proc * p;
	संरचना proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	यदि (*local_fpu_owner != रिक्त) {
		प्रतिज्ञा(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	यदि (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = रिक्त;		/* release FPU */
		cause_sig(proc_nr(p), संक_भ_त्रुटि);
		फर्काऊ;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

शून्य release_fpu(संरचना proc * p) {
	संरचना proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	यदि (*fpu_owner_ptr == p)
		*fpu_owner_ptr = रिक्त;
}

शून्य ser_dump_proc(शून्य)
{
        संरचना proc *pp;

        लागि (pp= BEG_PROC_ADDR; pp < END_PROC_ADDR; pp++)
        {
                यदि (isemptyp(pp))
                        जारी;
                print_proc_recursive(pp);
        }
}
