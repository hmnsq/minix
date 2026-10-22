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

#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/syslib.h>

/* Scheduling and message passing functions */
ਸਥਿਰ ਖਾਲੀ idle(ਖਾਲੀ);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
ਸਥਿਰ ਪੂਰਨ mini_receive(ਬਣਤਰ proc *caller_ptr, endpoint_t src,
	ਸੁਨੇਹਾ *m_buff_usr, ਪੂਰਨ flags);
ਸਥਿਰ ਪੂਰਨ mini_senda(ਬਣਤਰ proc *caller_ptr, asynmsg_t *ਸਾਰਣੀ, size_t
	ਮਾਪ);
ਸਥਿਰ ਪੂਰਨ deadlock(ਪੂਰਨ function, ਰਜਿਸਟਰ ਬਣਤਰ proc *caller,
	endpoint_t src_dst_e);
ਸਥਿਰ ਪੂਰਨ try_async(ਬਣਤਰ proc *caller_ptr);
ਸਥਿਰ ਪੂਰਨ try_one(endpoint_t receive_e, ਬਣਤਰ proc *src_ptr,
	ਬਣਤਰ proc *dst_ptr);
ਸਥਿਰ ਬਣਤਰ proc * pick_proc(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_ਸਿਰ(ਬਣਤਰ proc *rp);

/* all idles share the same idle_priv structure */
ਸਥਿਰ ਬਣਤਰ priv idle_priv;

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_idle_name(ਅੱਖਰ * name, ਪੂਰਨ n)
{
        ਪੂਰਨ i, c;
        ਪੂਰਨ p_z = 0;

        ਜੇ (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        ਲਈ (i = 4, c = 100; c > 0; c /= 10) {
                ਪੂਰਨ digit;

                digit = n / c;  
                n -= digit * c;  

                ਜੇ (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PICK_ANY	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PICK_HIGHERONLY	2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Build_Notify_ਸੁਨੇਹਾ(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, ਆਕਾਰ(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_ਸੁਨੇਹਾ;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	ਚੋਣ (src) {							\
	ਹਾਲਤ HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		ਰੋਕੋ;							\
	ਹਾਲਤ SYSTEM:							\
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			ਆਕਾਰ(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		ਰੋਕੋ;							\
	}

ਸਥਿਰ ਸੁਨੇਹਾ m_notify_buff = { 0, NOTIFY_ਸੁਨੇਹਾ };

ਖਾਲੀ proc_init(ਖਾਲੀ)
{
	ਬਣਤਰ proc * rp;
	ਬਣਤਰ priv *sp;
	ਪੂਰਨ i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	ਲਈ (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < ਸਿਰਾ_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_ਥਾਂ_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = ਨਿਰਾ;		/* no user space scheduler */
		rp->p_ਤਰਜੀਹ = 0;		/* no priority */
		rp->p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	ਲਈ (sp = BEG_PRIV_ADDR, i = 0; sp < ਸਿਰਾ_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	ਲਈ (i = 0; i < CONFIG_MAX_CPUS; i++) {
		ਬਣਤਰ proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		ਸੈੱਟ_idle_name(ip->p_name, i);
	}
}

ਸਥਿਰ ਖਾਲੀ switch_ਪਤਾ_space_idle(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_ਪਤਾ_space(proc_addr(VM_PROC_NR));
#ਜੇ_ਅੰਤ
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ idle(ਖਾਲੀ)
{
	ਬਣਤਰ proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	ਜੇ (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_ਪਤਾ_space_idle();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	ਜੇ (cpuid != bsp_cpu_id)
		stop_local_timer();
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#ਜੇ !SPROFILE
	halt_cpu();
#ਨਹੀਂਤਾਂ
	ਜੇ (!sprofiling)
		halt_cpu();
	ਨਹੀਂਤਾਂ {
		ਅਸਥਿਰ ਪੂਰਨ * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		ਜਦੋਂ (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#ਜੇ_ਅੰਤ
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
ਖਾਲੀ vm_suspend(ਬਣਤਰ proc *caller, ਅਚਲ ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ,
        ਅਚਲ vir_ਬਾਈਟਾਂ linaddr, ਅਚਲ vir_ਬਾਈਟਾਂ len, ਅਚਲ ਪੂਰਨ type,
        ਅਚਲ ਪੂਰਨ writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(caller, RTS_VMREQUEST));
        ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(ਨਿਸ਼ਾਨਾ, RTS_VMREQUEST));

        RTS_ਸੈੱਟ(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ->p_endpoint;
        caller->p_vmrequest.params.check.ਸ਼ੁਰੂ = linaddr;
        caller->p_vmrequest.params.check.ਲੰਮਾਈ = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        ਜੇ(!(caller->p_vmrequest.nextrequestor = vmrequest))
                ਜੇ(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ delivermsg(ਬਣਤਰ proc *rp)
{
        ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(rp, RTS_VMREQUEST));
        ਪ੍ਰਤਿਜ੍ਞਾ(rp->p_misc_flags & MF_DELIVERMSG);
        ਪ੍ਰਤਿਜ੍ਞਾ(rp->p_delivermsg.m_ਸਰੋਤ != NONE);

        ਜੇ (copy_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_user(&rp->p_delivermsg,
                                (ਸੁਨੇਹਾ *) rp->p_delivermsg_vir)) {
                ਜੇ(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        ਲਿਖੋ("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } ਨਹੀਂਤਾਂ {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                ਆਕਾਰ(ਸੁਨੇਹਾ), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } ਨਹੀਂਤਾਂ {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_ਸਰੋਤ = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                ਜੇ(!(rp->p_misc_flags & MF_CONTEXT_ਸੈੱਟ)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
ਖਾਲੀ switch_ਮੰਜ਼ਿਲ_user(ਖਾਲੀ)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	ਬਣਤਰ proc * p;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਪੂਰਨ tlb_must_refresh = 0;
#ਜੇ_ਅੰਤ

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	ਜੇ (proc_is_runnable(p))
		ਜਾਓ check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	ਜੇ (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		ਜੇ (proc_is_runnable(p)) {
			ਜੇ (p->p_cpu_ਸਮਾਂ_ਖੱਬਾ)
				ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_ਸਿਰ(p);
			ਨਹੀਂਤਾਂ
				ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	ਜਦੋਂ (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#ਜੇ_ਅੰਤ
	switch_ਪਤਾ_space(p);

check_misc_flags:

	ਪ੍ਰਤਿਜ੍ਞਾ(p);
	ਪ੍ਰਤਿਜ੍ਞਾ(proc_is_runnable(p));
	ਜਦੋਂ (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ਸਰਗਰਮ)) {

		ਪ੍ਰਤਿਜ੍ਞਾ(proc_is_runnable(p));
		ਜੇ (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		ਨਹੀਂਤਾਂ ਜੇ (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, ਲਿਖੋ("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		ਨਹੀਂਤਾਂ ਜੇ (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			ਪ੍ਰਤਿਜ੍ਞਾ (!(p->p_misc_flags & MF_SC_ਸਰਗਰਮ));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			ਜੇ ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_ਪੂਰਾ(p);
		}
		ਨਹੀਂਤਾਂ ਜੇ (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			ਜੇ (!(p->p_misc_flags & MF_SC_ਸਰਗਰਮ))
				ਰੋਕੋ;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ਸਰਗਰਮ);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		ਨਹੀਂਤਾਂ ਜੇ (p->p_misc_flags & MF_SC_ਸਰਗਰਮ) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ਸਰਗਰਮ;

			ਰੋਕੋ;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		ਜੇ (!proc_is_runnable(p))
			ਜਾਓ not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	ਜੇ (!p->p_cpu_ਸਮਾਂ_ਖੱਬਾ)
		proc_no_ਸਮਾਂ(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	ਜੇ (!proc_is_runnable(p))
		ਜਾਓ not_runnable_pick_new;

	TRACE(VF_SCHEDULING, ਲਿਖੋ("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#ਜੇ DEBUG_TRACE
	p->p_schedules++;
#ਜੇ_ਅੰਤ

	p = arch_ਅੰਤ_switch_ਮੰਜ਼ਿਲ_user();
	ਪ੍ਰਤਿਜ੍ਞਾ(p->p_cpu_ਸਮਾਂ_ਖੱਬਾ);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	ਜੇ (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	ਨਹੀਂਤਾਂ
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_ਸੈੱਟ;

#ਜੇ defined(__i386__)
  	ਪ੍ਰਤਿਜ੍ਞਾ(p->p_seg.p_cr3 != 0);
#ਜਾਂਜੇ defined(__arm__)
	ਪ੍ਰਤਿਜ੍ਞਾ(p->p_seg.p_ttbr != 0);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	ਜੇ (p->p_misc_flags & MF_FLUSH_TLB) {
		ਜੇ (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#ਜੇ_ਅੰਤ
	
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
ਸਥਿਰ ਪੂਰਨ do_sync_ipc(ਬਣਤਰ proc * caller_ptr, /* who made the call */
			ਪੂਰਨ call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			ਸੁਨੇਹਾ *m_ptr)	/* users pointer to a message */
{
  ਪੂਰਨ ਨਤੀਜਾ;					/* the system call's result */
  ਪੂਰਨ src_dst_p;				/* Process slot number */
  ਅੱਖਰ *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  ਪ੍ਰਤਿਜ੍ਞਾ(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  ਜੇ (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_ਨਾਂ[call_nr])) {
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
      ਲਿਖੋ("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ਜੇ (src_dst_e == ANY)
  {
	ਜੇ (call_nr != RECEIVE)
	{
#ਜੇ 0
		ਲਿਖੋ("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#ਜੇ_ਅੰਤ
		ਵਾਪਸ EINVAL;
	}
	src_dst_p = (ਪੂਰਨ) src_dst_e;
  }
  ਨਹੀਂਤਾਂ
  {
	/* Require a valid source and/or destination process. */
	ਜੇ(!isokendpt(src_dst_e, &src_dst_p)) {
#ਜੇ 0
		ਲਿਖੋ("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#ਜੇ_ਅੰਤ
		ਵਾਪਸ EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	ਜੇ (call_nr != RECEIVE)
	{
		ਜੇ (!may_send_ਮੰਜ਼ਿਲ(caller_ptr, src_dst_p)) {
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
			ਲਿਖੋ(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#ਜੇ_ਅੰਤ
			ਵਾਪਸ(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  ਜੇ (!(priv(caller_ptr)->s_trap_ਨਕਾਬ & (1 << call_nr))) {
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
      ਲਿਖੋ("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ਜੇ (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
      ਲਿਖੋ("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ਚੋਣ(call_nr) {
  ਹਾਲਤ SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_ਜਵਾਬ_PEND;
	/* fall through */
  ਹਾਲਤ SEND:			
	ਨਤੀਜਾ = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	ਜੇ (call_nr == SEND || ਨਤੀਜਾ != OK)
		ਰੋਕੋ;				/* done, or SEND failed */
	/* fall through for SENDREC */
  ਹਾਲਤ RECEIVE:			
	ਜੇ (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_ਜਵਾਬ_PEND;
		IPC_ਸਥਿਤੀ_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	ਨਤੀਜਾ = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	ਰੋਕੋ;
  ਹਾਲਤ NOTIFY:
	ਨਤੀਜਾ = mini_notify(caller_ptr, src_dst_e);
	ਰੋਕੋ;
  ਹਾਲਤ SENDNB:
        ਨਤੀਜਾ = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        ਰੋਕੋ;
  ਮੂਲ:
	ਨਤੀਜਾ = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  ਵਾਪਸ(ਨਤੀਜਾ);
}

ਪੂਰਨ do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  ਬਣਤਰ proc *ਅਚਲ caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  ਪੂਰਨ call_nr = (ਪੂਰਨ) r1;

  ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(caller_ptr, RTS_ਥਾਂ_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  ਜੇ (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	ਜੇ ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		ਪ੍ਰਤਿਜ੍ਞਾ(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		ਵਾਪਸ caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	ਪ੍ਰਤਿਜ੍ਞਾ (!(caller_ptr->p_misc_flags & MF_SC_ਸਰਗਰਮ));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ਸਰਗਰਮ;
  }

  ਜੇ(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  ਚੋਣ(call_nr) {
  	ਹਾਲਤ SENDREC:
  	ਹਾਲਤ SEND:			
  	ਹਾਲਤ RECEIVE:			
  	ਹਾਲਤ NOTIFY:
  	ਹਾਲਤ SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    ਵਾਪਸ do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (ਸੁਨੇਹਾ *) r3);
  	}
  	ਹਾਲਤ SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t ਸੁਨੇਹਾ_ਮਾਪ = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    ਜੇ (ਸੁਨੇਹਾ_ਮਾਪ > 16*(NR_TASKS + NR_PROCS))
	        ਵਾਪਸ EDOM;
  	    ਵਾਪਸ mini_senda(caller_ptr, (asynmsg_t *) r3, ਸੁਨੇਹਾ_ਮਾਪ);
  	}
  	ਹਾਲਤ MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	ਜੇ(!minix_kerninfo_user) {
			ਵਾਪਸ EBADCALL;
		}

  		arch_ਸੈੱਟ_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		ਵਾਪਸ OK;
	}
  	ਮੂਲ:
	ਵਾਪਸ EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ deadlock(
  ਪੂਰਨ function,				/* trap number */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *xp;			/* process pointer */
  ਪੂਰਨ ਸਮੂਹ_ਮਾਪ = 1;				/* start with only caller */
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
  ਸਥਿਰ ਬਣਤਰ proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#ਜੇ_ਅੰਤ

  ਜਦੋਂ (src_dst_e != ANY) { 			/* check while process nr */
      ਪੂਰਨ src_dst_ਥਾਂ;
      okendpt(src_dst_e, &src_dst_ਥਾਂ);
      xp = proc_addr(src_dst_ਥਾਂ);		/* follow chain of processes */
      ਪ੍ਰਤਿਜ੍ਞਾ(proc_ptr_ok(xp));
      ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(xp, RTS_ਥਾਂ_FREE));
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
      processes[ਸਮੂਹ_ਮਾਪ] = xp;
#ਜੇ_ਅੰਤ
      ਸਮੂਹ_ਮਾਪ ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      ਜੇ((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	ਵਾਪਸ 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      ਜੇ (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  ਜੇ (ਸਮੂਹ_ਮਾਪ == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      ਜੇ ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          ਵਾਪਸ(0);			/* not a deadlock */
	      }
	  }
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
	  {
		ਪੂਰਨ i;
		ਲਿਖੋ("deadlock between these processes:\n");
		ਲਈ(i = 0; i < ਸਮੂਹ_ਮਾਪ; i++) {
			ਲਿਖੋ(" %10s ", processes[i]->p_name);
		}
		ਲਿਖੋ("\n\n");
		ਲਈ(i = 0; i < ਸਮੂਹ_ਮਾਪ; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#ਜੇ_ਅੰਤ
          ਵਾਪਸ(ਸਮੂਹ_ਮਾਪ);			/* deadlock found */
      }
  }
  ਵਾਪਸ(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ has_pending(sys_map_t *map, ਪੂਰਨ src_p, ਪੂਰਨ asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  ਪੂਰਨ src_id;
  sys_id_t id = NULL_PRIV_ID;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  ਬਣਤਰ proc * p;
#ਜੇ_ਅੰਤ

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  ਜੇ (src_p != ANY) {
	src_id = nr_ਮੰਜ਼ਿਲ_id(src_p);
	ਜੇ (get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, src_id)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
		p = proc_addr(id_ਮੰਜ਼ਿਲ_nr(src_id));
		ਜੇ (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_ਖੁੰਝ;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			id = src_id;
	}
  } ਨਹੀਂਤਾਂ {
	/* Find a source with a pending message */
	ਲਈ (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_ਬਿੱਟ) {
		ਜੇ (get_sys_ਬਿੱਟ(*map, src_id) != 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
			ਜਦੋਂ (src_id < NR_SYS_PROCS) {
				ਜਦੋਂ (!get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, src_id)) {
					ਜੇ (src_id == NR_SYS_PROCS)
						ਜਾਓ quit_ਖੋਜ;
					src_id++;
				}
				p = proc_addr(id_ਮੰਜ਼ਿਲ_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				ਜੇ (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_ਖੁੰਝ;
					src_id++;
				} ਨਹੀਂਤਾਂ
					ਜਾਓ quit_ਖੋਜ;
			}
#ਨਹੀਂਤਾਂ
			ਜਦੋਂ (!get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, src_id)) src_id++;
			ਜਾਓ quit_ਖੋਜ;
#ਜੇ_ਅੰਤ
		}
	}

quit_ਖੋਜ:
	ਜੇ (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  ਵਾਪਸ(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
ਪੂਰਨ has_pending_notify(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	ਵਾਪਸ has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
ਪੂਰਨ has_pending_asend(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	ਵਾਪਸ has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
ਖਾਲੀ unset_notify_pending(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
ਪੂਰਨ mini_send(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  ਸੁਨੇਹਾ *m_ptr,			/* pointer to message buffer */
  ਅਚਲ ਪੂਰਨ flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *dst_ptr;
  ਰਜਿਸਟਰ ਬਣਤਰ proc **xpp;
  ਪੂਰਨ dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  ਜੇ (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	ਵਾਪਸ EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  ਜੇ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_ਬਾਈਟਾਂ)m_ptr, ਨਿਰਾ)) {
	ਪੂਰਨ call;
	/* Destination is indeed waiting for this message. */
	ਪ੍ਰਤਿਜ੍ਞਾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	ਜੇ (!(flags & FROM_KERNEL)) {
		ਜੇ(copy_ਸੁਨੇਹਾ_from_user(m_ptr, &dst_ptr->p_delivermsg))
			ਵਾਪਸ EFAULT;
	} ਨਹੀਂਤਾਂ {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_ਸਥਿਤੀ_ਜੋੜੋ_FLAGS(dst_ptr, IPC_FLG_ਸੁਨੇਹਾ_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_ਸਰੋਤ = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(dst_ptr, call);

	ਜੇ (dst_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND)
		dst_ptr->p_misc_flags &= ~MF_ਜਵਾਬ_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#ਜੇ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#ਜੇ_ਅੰਤ
  } ਨਹੀਂਤਾਂ {
	ਜੇ(flags & NON_BLOCKING) {
		ਵਾਪਸ(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	ਜੇ (deadlock(SEND, caller_ptr, dst_e)) {
		ਵਾਪਸ(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	ਜੇ (!(flags & FROM_KERNEL)) {
		ਜੇ(copy_ਸੁਨੇਹਾ_from_user(m_ptr, &caller_ptr->p_sendmsg))
			ਵਾਪਸ EFAULT;
	} ਨਹੀਂਤਾਂ {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_ਸੈੱਟ(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	ਪ੍ਰਤਿਜ੍ਞਾ(caller_ptr->p_q_link == ਨਿਰਾ);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	ਜਦੋਂ (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#ਜੇ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ mini_receive(ਬਣਤਰ proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			ਸੁਨੇਹਾ * m_buff_usr, /* pointer to message buffer */
			ਅਚਲ ਪੂਰਨ flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc **xpp;
  ਪੂਰਨ r, src_id, ਮਿਲਿਆ, src_proc_nr, src_p;
  endpoint_t sender_e;

  ਪ੍ਰਤਿਜ੍ਞਾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_ਬਾਈਟਾਂ) m_buff_usr;

  ਜੇ(src_e == ANY) src_p = ANY;
  ਨਹੀਂਤਾਂ
  {
	okendpt(src_e, &src_p);
	ਜੇ (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		ਵਾਪਸ EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  ਜੇ (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    ਜੇ (! (caller_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        ਮਿਲਿਆ = src_id != NULL_PRIV_ID;
        ਜੇ(ਮਿਲਿਆ) {
            src_proc_nr = id_ਮੰਜ਼ਿਲ_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        ਜੇ (ਮਿਲਿਆ && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
	    ਜੇ(src_proc_nr == NONE) {
		ਲਿਖੋ("mini_receive: sending notify from NONE\n");
	    }
#ਜੇ_ਅੰਤ
	    ਪ੍ਰਤਿਜ੍ਞਾ(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    ਪ੍ਰਤਿਜ੍ਞਾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    ਪ੍ਰਤਿਜ੍ਞਾ(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    Build_Notify_ਸੁਨੇਹਾ(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_ਸਰੋਤ = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(caller_ptr, NOTIFY);

	    ਜਾਓ receive_ਪੂਰਾ;
        }
    }

    /* Check for pending asynchronous messages */
    ਜੇ (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        ਜੇ (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        ਨਹੀਂਤਾਂ
        	r = try_async(caller_ptr);

	ਜੇ (r == OK) {
            IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(caller_ptr, SENDA);
            ਜਾਓ receive_ਪੂਰਾ;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    ਜਦੋਂ (*xpp) {
	ਬਣਤਰ proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        ਜੇ (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            ਪੂਰਨ call;
	    ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(sender, RTS_ਥਾਂ_FREE));
	    ਪ੍ਰਤਿਜ੍ਞਾ(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    ਪ੍ਰਤਿਜ੍ਞਾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_ਸਰੋਤ = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_ਜਵਾਬ_PEND ? SENDREC : SEND);
	    IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    ਜੇ (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_ਸਥਿਤੀ_ਜੋੜੋ_FLAGS(caller_ptr, IPC_FLG_ਸੁਨੇਹਾ_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    ਜੇ (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_ਪੂਰਾ(sender);

#ਜੇ DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#ਜੇ_ਅੰਤ
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = ਨਿਰਾ;
	    ਜਾਓ receive_ਪੂਰਾ;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  ਜੇ ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      ਜੇ (deadlock(RECEIVE, caller_ptr, src_e)) {
          ਵਾਪਸ(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_ਸੈੱਟ(caller_ptr, RTS_RECEIVING);
      ਵਾਪਸ(OK);
  } ਨਹੀਂਤਾਂ {
	ਵਾਪਸ(ENOTREADY);
  }

receive_ਪੂਰਾ:
  ਜੇ (caller_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND)
	  caller_ptr->p_misc_flags &= ~MF_ਜਵਾਬ_PEND;
  ਵਾਪਸ OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
ਪੂਰਨ mini_notify(
  ਅਚਲ ਬਣਤਰ proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  ਰਜਿਸਟਰ ਬਣਤਰ proc *dst_ptr;
  ਪੂਰਨ src_id;				/* source id for late delivery */
  ਪੂਰਨ dst_p;

  ਜੇ (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	ਲਿਖੋ("mini_notify: bogus endpoint %d\n", dst_e);
	ਵਾਪਸ EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  ਜੇ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      ਪ੍ਰਤਿਜ੍ਞਾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      Build_Notify_ਸੁਨੇਹਾ(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_ਸਰੋਤ = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      ਵਾਪਸ(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(dst_ptr)->s_notify_pending, src_id); 
  ਵਾਪਸ(OK);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCOMPLAIN(caller, ਇੰਦਰਾਜ, ਖੇਤਰ)	\
	ਲਿਖੋ("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
ਖੇਤਰ, caller->p_name, ਇੰਦਰਾਜ, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_RETR(ਇੰਦਰਾਜ) ਕਰੋ {			\
  ਜੇ (ਅੰਕੜੇ_copy(				\
  		caller_ptr->p_endpoint, ਸਾਰਣੀ_v + (ਇੰਦਰਾਜ)*ਆਕਾਰ(asynmsg_t),\
  		KERNEL, (vir_ਬਾਈਟਾਂ) &tabent,	\
  		ਆਕਾਰ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, ਇੰਦਰਾਜ, "message entry");	\
  			r = EFAULT;		\
	                ਜਾਓ asyn_error; \
  }						\
  ਨਹੀਂਤਾਂ ਜੇ(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_INSRT(ਇੰਦਰਾਜ) ਕਰੋ {			\
  ਜੇ (ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ) &tabent,	\
  		caller_ptr->p_endpoint, ਸਾਰਣੀ_v + (ਇੰਦਰਾਜ)*ਆਕਾਰ(asynmsg_t),\
  		ਆਕਾਰ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, ਇੰਦਰਾਜ, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } ਜਦੋਂ(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
ਪੂਰਨ try_deliver_senda(ਬਣਤਰ proc *caller_ptr,
				asynmsg_t *ਸਾਰਣੀ,
				size_t ਮਾਪ)
{
  ਪੂਰਨ r, dst_p, ਪੂਰਾ, do_notify;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ flags;
  endpoint_t dst;
  ਬਣਤਰ proc *dst_ptr;
  ਬਣਤਰ priv *privp;
  asynmsg_t tabent;
  ਅਚਲ vir_ਬਾਈਟਾਂ ਸਾਰਣੀ_v = (vir_ਬਾਈਟਾਂ) ਸਾਰਣੀ;
  ਸੁਨੇਹਾ *m_ptr = ਨਿਰਾ;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  ਜੇ (ਮਾਪ == 0) ਵਾਪਸ(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  ਪੂਰਾ = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  ਜੇ (ਮਾਪ > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    ਵਾਪਸ r;
  }

  ਲਈ (i = 0; i < ਮਾਪ; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ਜੇ (flags == 0) ਜਾਰੀ; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ਜੇ(flags & ~(AMF_VALID|AMF_ਪੂਰਾ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		ਜਾਓ asyn_error;
	}
	ਜੇ (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		ਜਾਓ asyn_error;
	}
	ਜੇ (flags & AMF_ਪੂਰਾ) ਜਾਰੀ;	/* Already done processing */

	r = OK;
	ਜੇ (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	ਨਹੀਂਤਾਂ ਜੇ (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	ਨਹੀਂਤਾਂ ਜੇ (!may_asynsend_ਮੰਜ਼ਿਲ(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	ਨਹੀਂਤਾਂ 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	ਜੇ (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	ਜੇ (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_ਬਾਈਟਾਂ)&ਸਾਰਣੀ[i].ਸੁਨੇਹਾ_2, ਨਿਰਾ) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_ਜਵਾਬ_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.ਸੁਨੇਹਾ_2;
		dst_ptr->p_delivermsg.m_ਸਰੋਤ = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_ਸਥਿਤੀ_ਜੋੜੋ_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#ਜੇ DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ ਜੇ (r == OK) {
		/* Inform receiver that something is pending */
		ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		ਪੂਰਾ = FALSE;
		ਜਾਰੀ;
	} 

	/* Store results */
	tabent.ਨਤੀਜਾ = r;
	tabent.flags = flags | AMF_ਪੂਰਾ;
	ਜੇ (flags & AMF_NOTIFY)
		do_notify = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	ਜਾਰੀ;

asyn_error:
	ਜੇ (dst != NONE)
		ਲਿਖੋ("KERNEL senda error %d to %d\n", r, dst);
	ਨਹੀਂਤਾਂ
		ਲਿਖੋ("KERNEL senda error %d\n", r);
  }

  ਜੇ (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  ਜੇ (!ਪੂਰਾ) {
	privp->s_asyntab = (vir_ਬਾਈਟਾਂ) ਸਾਰਣੀ;
	privp->s_asynsize = ਮਾਪ;
  }

  ਵਾਪਸ(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ mini_senda(ਬਣਤਰ proc *caller_ptr, asynmsg_t *ਸਾਰਣੀ, size_t ਮਾਪ)
{
  ਬਣਤਰ priv *privp;

  privp = priv(caller_ptr);
  ਜੇ (!(privp->s_flags & SYS_PROC)) {
	ਲਿਖੋ( "mini_senda: warning caller has no privilege structure\n");
	ਵਾਪਸ(EPERM);
  }

  ਵਾਪਸ try_deliver_senda(caller_ptr, ਸਾਰਣੀ, ਮਾਪ);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ try_async(ਬਣਤਰ proc * caller_ptr)
{
  ਪੂਰਨ r;
  ਬਣਤਰ priv *privp;
  ਬਣਤਰ proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  ਲਈ (privp = BEG_PRIV_ADDR; privp < ਸਿਰਾ_PRIV_ADDR; ++privp)  {
	ਜੇ (privp->s_proc_nr == NONE)
		ਜਾਰੀ;

	ਜੇ (!get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(*map, privp->s_id)) 
		ਜਾਰੀ;

	src_ptr = proc_addr(privp->s_proc_nr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	ਜੇ (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_ਖੁੰਝ;
		ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ

	ਪ੍ਰਤਿਜ੍ਞਾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	ਜੇ ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		ਵਾਪਸ(r);
  }

  ਵਾਪਸ(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
ਸਥਿਰ ਪੂਰਨ try_one(endpoint_t receive_e, ਬਣਤਰ proc *src_ptr,
    ਬਣਤਰ proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  ਪੂਰਨ r = EAGAIN, ਪੂਰਾ, do_notify;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags, i;
  size_t ਮਾਪ;
  endpoint_t dst, src_e;
  ਬਣਤਰ proc *caller_ptr;
  ਬਣਤਰ priv *privp;
  asynmsg_t tabent;
  vir_ਬਾਈਟਾਂ ਸਾਰਣੀ_v;

  privp = priv(src_ptr);
  ਜੇ (!(privp->s_flags & SYS_PROC)) ਵਾਪਸ(EPERM);
  ਮਾਪ = privp->s_asynsize;
  ਸਾਰਣੀ_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ਜੇ (ਮਾਪ == 0) ਵਾਪਸ(EAGAIN);
  ਜੇ (privp->s_asynendpoint != src_ptr->p_endpoint) ਵਾਪਸ EAGAIN;
  ਜੇ (!may_asynsend_ਮੰਜ਼ਿਲ(src_ptr, proc_nr(dst_ptr))) ਵਾਪਸ (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  ਪੂਰਾ = TRUE;

  ਲਈ (i = 0; i < ਮਾਪ; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ਜੇ (flags == 0) ਜਾਰੀ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ਜੇ(flags & ~(AMF_VALID|AMF_ਪੂਰਾ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ਨਹੀਂਤਾਂ ਜੇ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ਨਹੀਂਤਾਂ ਜੇ (flags & AMF_ਪੂਰਾ) ਜਾਰੀ; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	ਪੂਰਾ = FALSE;

	ਜੇ (r == EINVAL)
		ਜਾਓ store_ਨਤੀਜਾ;

	/* Message must be directed at receiving end */
	ਜੇ (dst != dst_ptr->p_endpoint) ਜਾਰੀ;

	ਜੇ (!CANRECEIVE(receive_e, src_e, dst_ptr,
		ਸਾਰਣੀ_v + i*ਆਕਾਰ(asynmsg_t) + offsetof(ਬਣਤਰ asynmsg,ਸੁਨੇਹਾ_2),
		ਨਿਰਾ)) {
		ਜਾਰੀ;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	ਜੇ ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_ਜਵਾਬ_PEND)) 
		ਜਾਰੀ;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.ਸੁਨੇਹਾ_2;
	dst_ptr->p_delivermsg.m_ਸਰੋਤ = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#ਜੇ DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#ਜੇ_ਅੰਤ

store_ਨਤੀਜਾ:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.ਨਤੀਜਾ = r;
	tabent.flags = flags | AMF_ਪੂਰਾ;
	ਜੇ (flags & AMF_NOTIFY) do_notify = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	ਰੋਕੋ;
  }

  ਜੇ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ਜੇ (ਪੂਰਾ) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } ਨਹੀਂਤਾਂ {
	ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  ਵਾਪਸ(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
ਪੂਰਨ cancel_async(ਬਣਤਰ proc *src_ptr, ਬਣਤਰ proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  ਪੂਰਨ ਪੂਰਾ, do_notify;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags, i;
  size_t ਮਾਪ;
  endpoint_t dst;
  ਬਣਤਰ proc *caller_ptr;
  ਬਣਤਰ priv *privp;
  asynmsg_t tabent;
  vir_ਬਾਈਟਾਂ ਸਾਰਣੀ_v;

  privp = priv(src_ptr);
  ਜੇ (!(privp->s_flags & SYS_PROC)) ਵਾਪਸ(EPERM);
  ਮਾਪ = privp->s_asynsize;
  ਸਾਰਣੀ_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ਜੇ (ਮਾਪ == 0) ਵਾਪਸ(EAGAIN);
  ਜੇ (!may_send_ਮੰਜ਼ਿਲ(src_ptr, proc_nr(dst_ptr))) ਵਾਪਸ(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  ਪੂਰਾ = TRUE;


  ਲਈ (i = 0; i < ਮਾਪ; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	ਪੂਰਨ r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ਜੇ (flags == 0) ਜਾਰੀ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ਜੇ(flags & ~(AMF_VALID|AMF_ਪੂਰਾ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ਨਹੀਂਤਾਂ ਜੇ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ਨਹੀਂਤਾਂ ਜੇ (flags & AMF_ਪੂਰਾ) ਜਾਰੀ; /* Already done processing */

	/* Message must be directed at receiving end */
	ਜੇ (dst != dst_ptr->p_endpoint) {
		ਪੂਰਾ = FALSE;
		ਜਾਰੀ;
	}

	/* Store results for sender */
	tabent.ਨਤੀਜਾ = r;
	tabent.flags = flags | AMF_ਪੂਰਾ;
	ਜੇ (flags & AMF_NOTIFY) do_notify = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  ਜੇ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ਜੇ (!ਪੂਰਾ) {
	privp->s_asyntab = ਸਾਰਣੀ_v;
	privp->s_asynsize = ਮਾਪ;
  }

asyn_error:
  ਵਾਪਸ(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
ਖਾਲੀ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp	/* this process is now runnable */
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
  ਪੂਰਨ q = rp->p_ਤਰਜੀਹ;	 		/* scheduling queue to use */
  ਬਣਤਰ proc **rdy_ਸਿਰ, **rdy_ਪੂਛ;
  
  ਪ੍ਰਤਿਜ੍ਞਾ(proc_is_runnable(rp));

  ਪ੍ਰਤਿਜ੍ਞਾ(q >= 0);

  rdy_ਸਿਰ = get_cpu_var(rp->p_cpu, ਲੜੀਵਾਰ_q_ਸਿਰ);
  rdy_ਪੂਛ = get_cpu_var(rp->p_cpu, ਲੜੀਵਾਰ_q_ਪੂਛ);

  /* Now add the process to the queue. */
  ਜੇ (!rdy_ਸਿਰ[q]) {		/* add to empty queue */
      rdy_ਸਿਰ[q] = rdy_ਪੂਛ[q] = rp; 		/* create a new queue */
      rp->p_nextready = ਨਿਰਾ;		/* mark new end */
  } 
  ਨਹੀਂਤਾਂ {					/* add to tail of queue */
      rdy_ਪੂਛ[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_ਪੂਛ[q] = rp;				/* set new queue tail */
      rp->p_nextready = ਨਿਰਾ;		/* mark new end */
  }

  ਜੇ (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  ਬਣਤਰ proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  ਪ੍ਰਤਿਜ੍ਞਾ(p);
	  ਜੇ((p->p_ਤਰਜੀਹ > rp->p_ਤਰਜੀਹ) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_ਸੈੱਟ(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  ਨਹੀਂਤਾਂ ਜੇ (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_ਸਮਾਂ_ਸਾਰਣੀ(rp->p_cpu);
  }
#ਜੇ_ਅੰਤ

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_ਕਤਾਰ));


#ਜੇ DEBUG_SANITYCHECKS
  ਪ੍ਰਤਿਜ੍ਞਾ(runqueues_ok_local());
#ਜੇ_ਅੰਤ
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
ਸਥਿਰ ਖਾਲੀ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_ਸਿਰ(ਬਣਤਰ proc *rp)
{
  ਅਚਲ ਪੂਰਨ q = rp->p_ਤਰਜੀਹ;	 		/* scheduling queue to use */

  ਬਣਤਰ proc **rdy_ਸਿਰ, **rdy_ਪੂਛ;

  ਪ੍ਰਤਿਜ੍ਞਾ(proc_ptr_ok(rp));
  ਪ੍ਰਤਿਜ੍ਞਾ(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  ਪ੍ਰਤਿਜ੍ਞਾ(rp->p_cpu_ਸਮਾਂ_ਖੱਬਾ);

  ਪ੍ਰਤਿਜ੍ਞਾ(q >= 0);


  rdy_ਸਿਰ = get_cpu_var(rp->p_cpu, ਲੜੀਵਾਰ_q_ਸਿਰ);
  rdy_ਪੂਛ = get_cpu_var(rp->p_cpu, ਲੜੀਵਾਰ_q_ਪੂਛ);

  /* Now add the process to the queue. */
  ਜੇ (!rdy_ਸਿਰ[q]) {		/* add to empty queue */
	rdy_ਸਿਰ[q] = rdy_ਪੂਛ[q] = rp; 	/* create a new queue */
	rp->p_nextready = ਨਿਰਾ;			/* mark new end */
  } ਨਹੀਂਤਾਂ {					/* add to head of queue */
	rp->p_nextready = rdy_ਸਿਰ[q];		/* chain head of queue */
	rdy_ਸਿਰ[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_ਕਤਾਰ)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#ਜੇ DEBUG_SANITYCHECKS
  ਪ੍ਰਤਿਜ੍ਞਾ(runqueues_ok_local());
#ਜੇ_ਅੰਤ
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
ਖਾਲੀ ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਬਣਤਰ proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  ਪੂਰਨ q = rp->p_ਤਰਜੀਹ;		/* queue to use */
  ਬਣਤਰ proc **xpp;			/* iterate over queue */
  ਬਣਤਰ proc *prev_xp;
  u64_t tsc, tsc_delta;

  ਬਣਤਰ proc **rdy_ਪੂਛ;

  ਪ੍ਰਤਿਜ੍ਞਾ(proc_ptr_ok(rp));
  ਪ੍ਰਤਿਜ੍ਞਾ(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  ਪ੍ਰਤਿਜ੍ਞਾ (!iskernelp(rp) || *priv(rp)->s_ਢੇਰ_guard == ਢੇਰ_GUARD);

  rdy_ਪੂਛ = get_cpu_var(rp->p_cpu, ਲੜੀਵਾਰ_q_ਪੂਛ);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = ਨਿਰਾ;				
  ਲਈ (xpp = get_cpu_var_ptr(rp->p_cpu, ਲੜੀਵਾਰ_q_ਸਿਰ[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      ਜੇ (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          ਜੇ (rp == rdy_ਪੂਛ[q]) {		/* queue tail removed */
              rdy_ਪੂਛ[q] = prev_xp;		/* set new tail */
	  }

          ਰੋਕੋ;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  ਜੇ (rp->p_accounting.enter_ਕਤਾਰ) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_ਕਤਾਰ;
	rp->p_accounting.ਸਮਾਂ_ਵਿੱਚ_ਕਤਾਰ = rp->p_accounting.ਸਮਾਂ_ਵਿੱਚ_ਕਤਾਰ +
		tsc_delta;
	rp->p_accounting.enter_ਕਤਾਰ = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#ਜੇ DEBUG_SANITYCHECKS
  ਪ੍ਰਤਿਜ੍ਞਾ(runqueues_ok_local());
#ਜੇ_ਅੰਤ
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
ਸਥਿਰ ਬਣਤਰ proc * pick_proc(ਖਾਲੀ)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;			/* process to run */
  ਬਣਤਰ proc **rdy_ਸਿਰ;
  ਪੂਰਨ q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_ਸਿਰ = get_cpulocal_var(ਲੜੀਵਾਰ_q_ਸਿਰ);
  ਲਈ (q=0; q < NR_SCHED_QUEUES; q++) {	
	ਜੇ(!(rp = rdy_ਸਿਰ[q])) {
		TRACE(VF_PICKPROC, ਲਿਖੋ("cpu %d queue %d empty\n", cpuid, q););
		ਜਾਰੀ;
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(proc_is_runnable(rp));
	ਜੇ (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	ਵਾਪਸ rp;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
ਬਣਤਰ proc *endpoint_lookup(endpoint_t e)
{
	ਪੂਰਨ n;

	ਜੇ(!isokendpt(e, &n)) ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
ਪੂਰਨ isokendpt_f(ਅਚਲ ਅੱਖਰ * file, ਪੂਰਨ ਸਤਰ, endpoint_t e, ਪੂਰਨ * p,
	ਅਚਲ ਪੂਰਨ fatalflag)
#ਨਹੀਂਤਾਂ
ਪੂਰਨ isokendpt_f(endpoint_t e, ਪੂਰਨ * p, ਅਚਲ ਪੂਰਨ fatalflag)
#ਜੇ_ਅੰਤ
{
	ਪੂਰਨ ok = 0;
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
	ਜੇ(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	ਜੇ(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	ਵਾਪਸ ok;
}

ਸਥਿਰ ਖਾਲੀ notify_scheduler(ਬਣਤਰ proc *p)
{
	ਸੁਨੇਹਾ m_no_ਸਮਾਂ_ਹਿੱਸਾ;
	ਪੂਰਨ err;

	ਪ੍ਰਤਿਜ੍ਞਾ(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_ਸੈੱਟ(p, RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_ਸਰੋਤ = p->p_endpoint;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_type   = SCHEDULING_NO_ਸਮਾਂ_ਹਿੱਸਾ;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_ਕਤਾਰ = cpu_ਸਮਾਂ_2_ms(p->p_accounting.ਸਮਾਂ_ਵਿੱਚ_ਕਤਾਰ);
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_deqs      = p->p_accounting.dequeues;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_preempt   = p->p_accounting.preempted;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_cpu       = cpuid;
	m_no_ਸਮਾਂ_ਹਿੱਸਾ.m_krn_lsys_ਸਮਾਂ_ਸਾਰਣੀ.acnt_cpu_ਲੱਦੋ  = cpu_ਲੱਦੋ();

	/* Reset accounting */
	reset_proc_accounting(p);

	ਜੇ ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_ਸਮਾਂ_ਹਿੱਸਾ, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

ਖਾਲੀ proc_no_ਸਮਾਂ(ਬਣਤਰ proc * p)
{
	ਜੇ (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	ਨਹੀਂਤਾਂ {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_ਸਮਾਂ_ਖੱਬਾ = ms_2_cpu_ਸਮਾਂ(p->p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms);
#ਜੇ DEBUG_RACE
		RTS_ਸੈੱਟ(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#ਜੇ_ਅੰਤ
	}
}

ਖਾਲੀ reset_proc_accounting(ਬਣਤਰ proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.ਸਮਾਂ_ਵਿੱਚ_ਕਤਾਰ = 0;
  p->p_accounting.enter_ਕਤਾਰ = 0;
}
	
ਖਾਲੀ copr_not_ਉਪਲਬਧ_handler(ਖਾਲੀ)
{
	ਬਣਤਰ proc * p;
	ਬਣਤਰ proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	ਜੇ (*local_fpu_owner != ਨਿਰਾ) {
		ਪ੍ਰਤਿਜ੍ਞਾ(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	ਜੇ (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = ਨਿਰਾ;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		ਵਾਪਸ;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

ਖਾਲੀ release_fpu(ਬਣਤਰ proc * p) {
	ਬਣਤਰ proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	ਜੇ (*fpu_owner_ptr == p)
		*fpu_owner_ptr = ਨਿਰਾ;
}

ਖਾਲੀ ser_dump_proc(ਖਾਲੀ)
{
        ਬਣਤਰ proc *pp;

        ਲਈ (pp= BEG_PROC_ADDR; pp < ਸਿਰਾ_PROC_ADDR; pp++)
        {
                ਜੇ (isemptyp(pp))
                        ਜਾਰੀ;
                print_proc_recursive(pp);
        }
}
