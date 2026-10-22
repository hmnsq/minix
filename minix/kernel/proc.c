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

#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "vm.h"
#ಅನ್ತರ್ಭಾವ "clock.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>

/* Scheduling and message passing functions */
ಸ್ಥಿರ ಶೂನ್ಯ idle(ಶೂನ್ಯ);
/**
 * Made public for use in clock.c (for user-space scheduling)
static int mini_send(struct proc *caller_ptr, endpoint_t dst_e, message
	*m_ptr, int flags);
*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mini_receive(ರಚನೆ proc *caller_ptr, endpoint_t src,
	ಸಂದೇಶ *m_buff_usr, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mini_senda(ರಚನೆ proc *caller_ptr, asynmsg_t *ಕೋಷ್ಟಕ, size_t
	ಗಾತ್ರ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ deadlock(ಪೂರ್ಣಾಂಕ function, ನೋಂದಣಿ ರಚನೆ proc *caller,
	endpoint_t src_dst_e);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ try_async(ರಚನೆ proc *caller_ptr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ try_one(endpoint_t receive_e, ರಚನೆ proc *src_ptr,
	ರಚನೆ proc *dst_ptr);
ಸ್ಥಿರ ರಚನೆ proc * pick_proc(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸರತಿಗೆ_ಸೇರಿಸು_ತಲೆ(ರಚನೆ proc *rp);

/* all idles share the same idle_priv structure */
ಸ್ಥಿರ ರಚನೆ priv idle_priv;

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_idle_name(ಅಕ್ಷರ * name, ಪೂರ್ಣಾಂಕ n)
{
        ಪೂರ್ಣಾಂಕ i, c;
        ಪೂರ್ಣಾಂಕ p_z = 0;

        ಆದರೆ (n > 999) 
                n = 999; 

        name[0] = 'i'; 
        name[1] = 'd'; 
        name[2] = 'l'; 
        name[3] = 'e'; 

        ಗಾಗಿ (i = 4, c = 100; c > 0; c /= 10) {
                ಪೂರ್ಣಾಂಕ digit;

                digit = n / c;  
                n -= digit * c;  

                ಆದರೆ (p_z || digit != 0 || c == 1) {
                        p_z = 1;
                        name[i++] = '0' + digit;
                }   
        }    

        name[i] = '\0';

}


#ವ್ಯಾಖ್ಯಾನಿಸು PICK_ANY	1
#ವ್ಯಾಖ್ಯಾನಿಸು PICK_HIGHERONLY	2

#ವ್ಯಾಖ್ಯಾನಿಸು Build_Notify_ಸಂದೇಶ(m_ptr, src, dst_ptr) \
	memset((m_ptr), 0, ಗಾತ್ರ(*(m_ptr)));				\
	(m_ptr)->m_type = NOTIFY_ಸಂದೇಶ;				\
	(m_ptr)->m_notify.timestamp = get_monotonic();		\
	ಆಯ್ಕೆ (src) {							\
	ಸ್ಥಿತಿ HARDWARE:							\
		(m_ptr)->m_notify.interrupts =			\
			priv(dst_ptr)->s_int_pending;			\
		priv(dst_ptr)->s_int_pending = 0;			\
		ನಿಲ್ಲಿಸು;							\
	ಸ್ಥಿತಿ SYSTEM:							\
		ಸ್ಮರಣೆ_ನಕಲು(&(m_ptr)->m_notify.sigset,			\
			&priv(dst_ptr)->s_sig_pending,			\
			ಗಾತ್ರ(sigset_t));				\
		sigemptyset(&priv(dst_ptr)->s_sig_pending);		\
		ನಿಲ್ಲಿಸು;							\
	}

ಸ್ಥಿರ ಸಂದೇಶ m_notify_buff = { 0, NOTIFY_ಸಂದೇಶ };

ಶೂನ್ಯ proc_init(ಶೂನ್ಯ)
{
	ರಚನೆ proc * rp;
	ರಚನೆ priv *sp;
	ಪೂರ್ಣಾಂಕ i;

	/* Clear the process table. Announce each slot as empty and set up
	 * mappings for proc_addr() and proc_nr() macros. Do the same for the
	 * table with privilege structures for the system processes. 
	 */
	ಗಾಗಿ (rp = BEG_PROC_ADDR, i = -NR_TASKS; rp < ಕೊನೆ_PROC_ADDR; ++rp, ++i) {
		rp->p_rts_flags = RTS_ಸ್ಥಾನ_FREE;/* initialize free slot */
		rp->p_magic = PMAGIC;
		rp->p_nr = i;			/* proc number from ptr */
		rp->p_endpoint = _ENDPOINT(0, rp->p_nr); /* generation no. 0 */
		rp->p_scheduler = ಖಾಲಿ;		/* no user space scheduler */
		rp->p_ಆದ್ಯತೆ = 0;		/* no priority */
		rp->p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms = 0;	/* no quantum size */

		/* arch-specific initialization */
		arch_proc_reset(rp);
	}
	ಗಾಗಿ (sp = BEG_PRIV_ADDR, i = 0; sp < ಕೊನೆ_PRIV_ADDR; ++sp, ++i) {
		sp->s_proc_nr = NONE;		/* initialize as free */
		sp->s_id = (sys_id_t) i;	/* priv structure index */
		ppriv_addr[i] = sp;		/* priv ptr from number */
		sp->s_sig_mgr = NONE;		/* clear signal managers */
		sp->s_bak_sig_mgr = NONE;
	}

	idle_priv.s_flags = IDL_F;
	/* initialize IDLE structures for every CPU */
	ಗಾಗಿ (i = 0; i < CONFIG_MAX_CPUS; i++) {
		ರಚನೆ proc * ip = get_cpu_var_ptr(i, idle_proc);
		ip->p_endpoint = IDLE;
		ip->p_priv = &idle_priv;
		/* must not let idle ever get scheduled */
		ip->p_rts_flags |= RTS_PROC_STOP;
		ಗಣ_idle_name(ip->p_name, i);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ switch_ವಿಳಾಸ_space_idle(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	/*
	 * currently we bet that VM is always alive and its pages available so
	 * when the CPU wakes up the kernel is mapped and no surprises happen.
	 * This is only a problem if more than 1 cpus are available
	 */
	switch_ವಿಳಾಸ_space(proc_addr(VM_PROC_NR));
#ಆದರೆ_ಅಂತ್ಯ
}

/*===========================================================================*
 *				idle					     * 
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ idle(ಶೂನ್ಯ)
{
	ರಚನೆ proc * p;

	/* This function is called whenever there is no work to do.
	 * Halt the CPU, and measure how many timestamp counter ticks are
	 * spent not doing anything. This allows test setups to measure
	 * the CPU utilization of certain workloads with high precision.
	 */

	p = get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	ಆದರೆ (priv(p)->s_flags & BILLABLE)
		get_cpulocal_var(bill_ptr) = p;

	switch_ವಿಳಾಸ_space_idle();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	get_cpulocal_var(cpu_is_idle) = 1;
	/* we don't need to keep time on APs as it is handled on the BSP */
	ಆದರೆ (cpuid != bsp_cpu_id)
		stop_local_timer();
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
		/*
		 * If the timer has expired while in kernel we must
		 * rearm it before we go to sleep
		 */
		restart_local_timer();
	}

	/* start accounting for the idle time */
	context_stop(proc_addr(KERNEL));
#ಆದರೆ !SPROFILE
	halt_cpu();
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (!sprofiling)
		halt_cpu();
	ಇಲ್ಲದಿದ್ದರೆ {
		ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ * v;

		v = get_cpulocal_var_ptr(idle_interrupted);
		interrupts_enable();
		ಆಗ (!*v)
			arch_pause();
		interrupts_disable();
		*v = 0;
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * end of accounting for the idle task does not happen here, the kernel
	 * is handling stuff for quite a while before it gets back here!
	 */
}

/*===========================================================================*
 *                              vm_suspend                                *
 *===========================================================================*/
ಶೂನ್ಯ vm_suspend(ರಚನೆ proc *caller, ಸ್ಥಿರಾಂಕ ರಚನೆ proc *ಗುರಿ,
        ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು linaddr, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು len, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ type,
        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ writeflag)
{
        /* This range is not OK for this process. Set parameters
         * of the request and notify VM about the pending request.
         */
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller, RTS_VMREQUEST));
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(ಗುರಿ, RTS_VMREQUEST));

        RTS_ಗಣ(caller, RTS_VMREQUEST);

        caller->p_vmrequest.req_type = VMPTYPE_CHECK;
        caller->p_vmrequest.ಗುರಿ = ಗುರಿ->p_endpoint;
        caller->p_vmrequest.params.check.ಆರಂಭ = linaddr;
        caller->p_vmrequest.params.check.ಉದ್ದಳತೆ = len;
        caller->p_vmrequest.params.check.writeflag = writeflag;
        caller->p_vmrequest.type = type;

        /* Connect caller on vmrequest wait queue. */
        ಆದರೆ(!(caller->p_vmrequest.nextrequestor = vmrequest))
                ಆದರೆ(OK != send_sig(VM_PROC_NR, SIGKMEM))
                        panic("send_sig failed");
        vmrequest = caller;
}

/*===========================================================================*
 *                              delivermsg                                *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ delivermsg(ರಚನೆ proc *rp)
{
        ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(rp, RTS_VMREQUEST));
        ಪ್ರತಿಜ್ಞಾ(rp->p_misc_flags & MF_DELIVERMSG);
        ಪ್ರತಿಜ್ಞಾ(rp->p_delivermsg.m_ಮೂಲ != NONE);

        ಆದರೆ (copy_ಸಂದೇಶ_ಗಮ್ಯ_user(&rp->p_delivermsg,
                                (ಸಂದೇಶ *) rp->p_delivermsg_vir)) {
                ಆದರೆ(rp->p_misc_flags & MF_MSGFAILED) {
                        /* 2nd consecutive failure means this won't succeed */
                        ಬರೆ("WARNING wrong user pointer 0x%08lx from "
                                "process %s / %d\n",
                                rp->p_delivermsg_vir,
                                rp->p_name,
                                rp->p_endpoint);
                        cause_sig(rp->p_nr, SIGSEGV);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        /* 1st failure means we have to ask VM to handle it */
                        vm_suspend(rp, rp, rp->p_delivermsg_vir,
                                ಗಾತ್ರ(ಸಂದೇಶ), VMSTYPE_DELIVERMSG, 1);
                        rp->p_misc_flags |= MF_MSGFAILED;
                }
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* Indicate message has been delivered; address is 'used'. */
                rp->p_delivermsg.m_ಮೂಲ = NONE;
                rp->p_misc_flags &= ~(MF_DELIVERMSG|MF_MSGFAILED);

                ಆದರೆ(!(rp->p_misc_flags & MF_CONTEXT_ಗಣ)) {
                        rp->p_reg.retreg = OK;
                }
        }
}

/*===========================================================================*
 *				switch_to_user				     * 
 *===========================================================================*/
ಶೂನ್ಯ switch_ಗಮ್ಯ_user(ಶೂನ್ಯ)
{
	/* This function is called an instant before proc_ptr is
	 * to be scheduled again.
	 */
	ರಚನೆ proc * p;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಪೂರ್ಣಾಂಕ tlb_must_refresh = 0;
#ಆದರೆ_ಅಂತ್ಯ

	p = get_cpulocal_var(proc_ptr);
	/*
	 * if the current process is still runnable check the misc flags and let
	 * it run unless it becomes not runnable in the meantime
	 */
	ಆದರೆ (proc_is_runnable(p))
		ಹೋಗು check_misc_flags;
	/*
	 * if a process becomes not runnable while handling the misc flags, we
	 * need to pick a new one here and start from scratch. Also if the
	 * current process wasn't runnable, we pick a new one here
	 */
not_runnable_pick_new:
	ಆದರೆ (proc_is_preempted(p)) {
		p->p_rts_flags &= ~RTS_PREEMPTED;
		ಆದರೆ (proc_is_runnable(p)) {
			ಆದರೆ (p->p_cpu_ಸಮಯ_ಎಡ)
				ಸರತಿಗೆ_ಸೇರಿಸು_ತಲೆ(p);
			ಇಲ್ಲದಿದ್ದರೆ
				ಸರತಿಗೆ_ಸೇರಿಸು(p);
		}
	}

	/*
	 * if we have no process to run, set IDLE as the current process for
	 * time accounting and put the cpu in an idle state. After the next
	 * timer interrupt the execution resumes here and we can pick another
	 * process. If there is still nothing runnable we "schedule" IDLE again
	 */
	ಆಗ (!(p = pick_proc())) {
		idle();
	}

	/* update the global variable */
	get_cpulocal_var(proc_ptr) = p;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ (p->p_misc_flags & MF_FLUSH_TLB && get_cpulocal_var(ptproc) == p)
		tlb_must_refresh = 1;
#ಆದರೆ_ಅಂತ್ಯ
	switch_ವಿಳಾಸ_space(p);

check_misc_flags:

	ಪ್ರತಿಜ್ಞಾ(p);
	ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(p));
	ಆಗ (p->p_misc_flags &
		(MF_KCALL_RESUME | MF_DELIVERMSG |
		 MF_SC_DEFER | MF_SC_TRACE | MF_SC_ಸಕ್ರಿಯ)) {

		ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(p));
		ಆದರೆ (p->p_misc_flags & MF_KCALL_RESUME) {
			kernel_call_resume(p);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_misc_flags & MF_DELIVERMSG) {
			TRACE(VF_SCHEDULING, ಬರೆ("delivering to %s / %d\n",
				p->p_name, p->p_endpoint););
			delivermsg(p);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_misc_flags & MF_SC_DEFER) {
			/* Perform the system call that we deferred earlier. */

			ಪ್ರತಿಜ್ಞಾ (!(p->p_misc_flags & MF_SC_ಸಕ್ರಿಯ));

			arch_do_syscall(p);

			/* If the process is stopped for signal delivery, and
			 * not blocked sending a message after the system call,
			 * inform PM.
			 */
			ಆದರೆ ((p->p_misc_flags & MF_SIG_DELAY) &&
					!RTS_ISSET(p, RTS_SENDING))
				sig_delay_ಮುಗಿದ(p);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_misc_flags & MF_SC_TRACE) {
			/* Trigger a system call leave event if this was a
			 * system call. We must do this after processing the
			 * other flags above, both for tracing correctness and
			 * to be able to use 'break'.
			 */
			ಆದರೆ (!(p->p_misc_flags & MF_SC_ಸಕ್ರಿಯ))
				ನಿಲ್ಲಿಸು;

			p->p_misc_flags &=
				~(MF_SC_TRACE | MF_SC_ಸಕ್ರಿಯ);

			/* Signal the "leave system call" event.
			 * Block the process.
			 */
			cause_sig(proc_nr(p), SIGTRAP);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->p_misc_flags & MF_SC_ಸಕ್ರಿಯ) {
			/* If MF_SC_ACTIVE was set, remove it now:
			 * we're leaving the system call.
			 */
			p->p_misc_flags &= ~MF_SC_ಸಕ್ರಿಯ;

			ನಿಲ್ಲಿಸು;
		}

		/*
		 * the selected process might not be runnable anymore. We have
		 * to checkit and schedule another one
		 */
		ಆದರೆ (!proc_is_runnable(p))
			ಹೋಗು not_runnable_pick_new;
	}
	/*
	 * check the quantum left before it runs again. We must do it only here
	 * as we are sure that a possible out-of-quantum message to the
	 * scheduler will not collide with the regular ipc
	 */
	ಆದರೆ (!p->p_cpu_ಸಮಯ_ಎಡ)
		proc_no_ಸಮಯ(p);
	/*
	 * After handling the misc flags the selected process might not be
	 * runnable anymore. We have to checkit and schedule another one
	 */
	ಆದರೆ (!proc_is_runnable(p))
		ಹೋಗು not_runnable_pick_new;

	TRACE(VF_SCHEDULING, ಬರೆ("cpu %d starting %s / %d "
				"pc 0x%08x\n",
		cpuid, p->p_name, p->p_endpoint, p->p_reg.pc););
#ಆದರೆ DEBUG_TRACE
	p->p_schedules++;
#ಆದರೆ_ಅಂತ್ಯ

	p = arch_ಮುಕ್ತಾಯ_switch_ಗಮ್ಯ_user();
	ಪ್ರತಿಜ್ಞಾ(p->p_cpu_ಸಮಯ_ಎಡ);

	context_stop(proc_addr(KERNEL));

	/* If the process isn't the owner of FPU, enable the FPU exception */
	ಆದರೆ (get_cpulocal_var(fpu_owner) != p)
		enable_fpu_exception();
	ಇಲ್ಲದಿದ್ದರೆ
		disable_fpu_exception();

	/* If MF_CONTEXT_SET is set, don't clobber process state within
	 * the kernel. The next kernel entry is OK again though.
	 */
	p->p_misc_flags &= ~MF_CONTEXT_ಗಣ;

#ಆದರೆ defined(__i386__)
  	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_cr3 != 0);
#ಅಥವಾಆದರೆ defined(__arm__)
	ಪ್ರತಿಜ್ಞಾ(p->p_seg.p_ttbr != 0);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ (p->p_misc_flags & MF_FLUSH_TLB) {
		ಆದರೆ (tlb_must_refresh)
			refresh_tlb();
		p->p_misc_flags &= ~MF_FLUSH_TLB;
	}
#ಆದರೆ_ಅಂತ್ಯ
	
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_sync_ipc(ರಚನೆ proc * caller_ptr, /* who made the call */
			ಪೂರ್ಣಾಂಕ call_nr,	/* system call number and flags */
			endpoint_t src_dst_e,	/* src or dst of the call */
			ಸಂದೇಶ *m_ptr)	/* users pointer to a message */
{
  ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;					/* the system call's result */
  ಪೂರ್ಣಾಂಕ src_dst_p;				/* Process slot number */
  ಅಕ್ಷರ *callname;

  /* Check destination. RECEIVE is the only call that accepts ANY (in addition
   * to a real endpoint). The other calls (SEND, SENDREC, and NOTIFY) require an
   * endpoint to corresponds to a process. In addition, it is necessary to check
   * whether a process is allowed to send to a given destination.
   */
  ಪ್ರತಿಜ್ಞಾ(call_nr != SENDA);

  /* Only allow non-negative call_nr values less than 32 */
  ಆದರೆ (call_nr < 0 || call_nr > IPCNO_HIGHEST || call_nr >= 32
      || !(callname = ipc_call_ಹೆಸರುಗಳು[call_nr])) {
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
      ಬರೆ("sys_call: trap %d not allowed, caller %d, src_dst %d\n", 
          call_nr, proc_nr(caller_ptr), src_dst_e);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ಆದರೆ (src_dst_e == ANY)
  {
	ಆದರೆ (call_nr != RECEIVE)
	{
#ಆದರೆ 0
		ಬರೆ("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ EINVAL;
	}
	src_dst_p = (ಪೂರ್ಣಾಂಕ) src_dst_e;
  }
  ಇಲ್ಲದಿದ್ದರೆ
  {
	/* Require a valid source and/or destination process. */
	ಆದರೆ(!isokendpt(src_dst_e, &src_dst_p)) {
#ಆದರೆ 0
		ಬರೆ("sys_call: %s by %d with bad endpoint %d\n", 
			callname,
			proc_nr(caller_ptr), src_dst_e);
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ EDEADSRCDST;
	}

	/* If the call is to send to a process, i.e., for SEND, SENDNB,
	 * SENDREC or NOTIFY, verify that the caller is allowed to send to
	 * the given destination. 
	 */
	ಆದರೆ (call_nr != RECEIVE)
	{
		ಆದರೆ (!may_send_ಗಮ್ಯ(caller_ptr, src_dst_p)) {
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
			ಬರೆ(
			"sys_call: ipc mask denied %s from %d to %d\n",
				callname,
				caller_ptr->p_endpoint, src_dst_e);
#ಆದರೆ_ಅಂತ್ಯ
			ಹಿಂತಿರುಗಿ(ECALLDENIED);	/* call denied by ipc mask */
		}
	}
  }

  /* Check if the process has privileges for the requested call. Calls to the 
   * kernel may only be SENDREC, because tasks always reply and may not block 
   * if the caller doesn't do receive(). 
   */
  ಆದರೆ (!(priv(caller_ptr)->s_trap_ಮುಖವಾಡ & (1 << call_nr))) {
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
      ಬರೆ("sys_call: %s not allowed, caller %d, src_dst %d\n", 
          callname, proc_nr(caller_ptr), src_dst_p);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ಆದರೆ (call_nr != SENDREC && call_nr != RECEIVE && iskerneln(src_dst_p)) {
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
      ಬರೆ("sys_call: trap %s not allowed, caller %d, src_dst %d\n",
           callname, proc_nr(caller_ptr), src_dst_e);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ(ETRAPDENIED);		/* trap denied by mask or kernel */
  }

  ಆಯ್ಕೆ(call_nr) {
  ಸ್ಥಿತಿ SENDREC:
	/* A flag is set so that notifications cannot interrupt SENDREC. */
	caller_ptr->p_misc_flags |= MF_ಉತ್ತರ_PEND;
	/* fall through */
  ಸ್ಥಿತಿ SEND:			
	ಫಲಿತಾಂಶ = mini_send(caller_ptr, src_dst_e, m_ptr, 0);
	ಆದರೆ (call_nr == SEND || ಫಲಿತಾಂಶ != OK)
		ನಿಲ್ಲಿಸು;				/* done, or SEND failed */
	/* fall through for SENDREC */
  ಸ್ಥಿತಿ RECEIVE:			
	ಆದರೆ (call_nr == RECEIVE) {
		caller_ptr->p_misc_flags &= ~MF_ಉತ್ತರ_PEND;
		IPC_ಸ್ಥಿತಿ_CLEAR(caller_ptr);  /* clear IPC status code */
	}
	ಫಲಿತಾಂಶ = mini_receive(caller_ptr, src_dst_e, m_ptr, 0);
	ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NOTIFY:
	ಫಲಿತಾಂಶ = mini_notify(caller_ptr, src_dst_e);
	ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SENDNB:
        ಫಲಿತಾಂಶ = mini_send(caller_ptr, src_dst_e, m_ptr, NON_BLOCKING);
        ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
	ಫಲಿತಾಂಶ = EBADCALL;			/* illegal system call */
  }

  /* Now, return the result of the system call to the caller. */
  ಹಿಂತಿರುಗಿ(ಫಲಿತಾಂಶ);
}

ಪೂರ್ಣಾಂಕ do_ipc(reg_t r1, reg_t r2, reg_t r3)
{
  ರಚನೆ proc *ಸ್ಥಿರಾಂಕ caller_ptr = get_cpulocal_var(proc_ptr);	/* get pointer to caller */
  ಪೂರ್ಣಾಂಕ call_nr = (ಪೂರ್ಣಾಂಕ) r1;

  ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(caller_ptr, RTS_ಸ್ಥಾನ_FREE));

  /* bill kernel time to this process. */
  kbill_ipc = caller_ptr;

  /* If this process is subject to system call tracing, handle that first. */
  ಆದರೆ (caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) {
	/* Are we tracing this process, and is it the first sys_call entry? */
	ಆದರೆ ((caller_ptr->p_misc_flags & (MF_SC_TRACE | MF_SC_DEFER)) ==
							MF_SC_TRACE) {
		/* We must notify the tracer before processing the actual
		 * system call. If we don't, the tracer could not obtain the
		 * input message. Postpone the entire system call.
		 */
		caller_ptr->p_misc_flags &= ~MF_SC_TRACE;
		ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_SC_DEFER));
		caller_ptr->p_misc_flags |= MF_SC_DEFER;
		caller_ptr->p_defer.r1 = r1;
		caller_ptr->p_defer.r2 = r2;
		caller_ptr->p_defer.r3 = r3;

		/* Signal the "enter system call" event. Block the process. */
		cause_sig(proc_nr(caller_ptr), SIGTRAP);

		/* Preserve the return register's value. */
		ಹಿಂತಿರುಗಿ caller_ptr->p_reg.retreg;
	}

	/* If the MF_SC_DEFER flag is set, the syscall is now being resumed. */
	caller_ptr->p_misc_flags &= ~MF_SC_DEFER;

	ಪ್ರತಿಜ್ಞಾ (!(caller_ptr->p_misc_flags & MF_SC_ಸಕ್ರಿಯ));

	/* Set a flag to allow reliable tracing of leaving the system call. */
	caller_ptr->p_misc_flags |= MF_SC_ಸಕ್ರಿಯ;
  }

  ಆದರೆ(caller_ptr->p_misc_flags & MF_DELIVERMSG) {
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
  ಆಯ್ಕೆ(call_nr) {
  	ಸ್ಥಿತಿ SENDREC:
  	ಸ್ಥಿತಿ SEND:			
  	ಸ್ಥಿತಿ RECEIVE:			
  	ಸ್ಥಿತಿ NOTIFY:
  	ಸ್ಥಿತಿ SENDNB:
  	{
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_sync++;

  	    ಹಿಂತಿರುಗಿ do_sync_ipc(caller_ptr, call_nr, (endpoint_t) r2,
			    (ಸಂದೇಶ *) r3);
  	}
  	ಸ್ಥಿತಿ SENDA:
  	{
 	    /*
  	     * Get and check the size of the argument in bytes as it is a
  	     * table
  	     */
  	    size_t ಸಂದೇಶ_ಗಾತ್ರ = (size_t) r2;
  
  	    /* Process accounting for scheduling */
	    caller_ptr->p_accounting.ipc_async++;
 
  	    /* Limit size to something reasonable. An arbitrary choice is 16
  	     * times the number of process table entries.
  	     */
  	    ಆದರೆ (ಸಂದೇಶ_ಗಾತ್ರ > 16*(NR_TASKS + NR_PROCS))
	        ಹಿಂತಿರುಗಿ EDOM;
  	    ಹಿಂತಿರುಗಿ mini_senda(caller_ptr, (asynmsg_t *) r3, ಸಂದೇಶ_ಗಾತ್ರ);
  	}
  	ಸ್ಥಿತಿ MINIX_KERNINFO:
	{
		/* It might not be initialized yet. */
	  	ಆದರೆ(!minix_kerninfo_user) {
			ಹಿಂತಿರುಗಿ EBADCALL;
		}

  		arch_ಗಣ_secondary_ipc_return(caller_ptr, minix_kerninfo_user);
  		ಹಿಂತಿರುಗಿ OK;
	}
  	ಪೂರ್ವನಿಯೋಜಿತ:
	ಹಿಂತಿರುಗಿ EBADCALL;		/* illegal system call */
  }
}

/*===========================================================================*
 *				deadlock				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ deadlock(
  ಪೂರ್ಣಾಂಕ function,				/* trap number */
  ನೋಂದಣಿ ರಚನೆ proc *cp,		/* pointer to caller */
  endpoint_t src_dst_e			/* src or dst process */
)
{
/* Check for deadlock. This can happen if 'caller_ptr' and 'src_dst' have
 * a cyclic dependency of blocking send and receive calls. The only cyclic 
 * dependency that is not fatal is if the caller and target directly SEND(REC)
 * and RECEIVE to each other. If a deadlock is found, the group size is 
 * returned. Otherwise zero is returned. 
 */
  ನೋಂದಣಿ ರಚನೆ proc *xp;			/* process pointer */
  ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಗಾತ್ರ = 1;				/* start with only caller */
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
  ಸ್ಥಿರ ರಚನೆ proc *processes[NR_PROCS + NR_TASKS];
  processes[0] = cp;
#ಆದರೆ_ಅಂತ್ಯ

  ಆಗ (src_dst_e != ANY) { 			/* check while process nr */
      ಪೂರ್ಣಾಂಕ src_dst_ಸ್ಥಾನ;
      okendpt(src_dst_e, &src_dst_ಸ್ಥಾನ);
      xp = proc_addr(src_dst_ಸ್ಥಾನ);		/* follow chain of processes */
      ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(xp));
      ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(xp, RTS_ಸ್ಥಾನ_FREE));
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
      processes[ಗುಂಪು_ಗಾತ್ರ] = xp;
#ಆದರೆ_ಅಂತ್ಯ
      ಗುಂಪು_ಗಾತ್ರ ++;				/* extra process in group */

      /* Check whether the last process in the chain has a dependency. If it 
       * has not, the cycle cannot be closed and we are done.
       */
      ಆದರೆ((src_dst_e = P_BLOCKEDON(xp)) == NONE)
	ಹಿಂತಿರುಗಿ 0;

      /* Now check if there is a cyclic dependency. For group sizes of two,  
       * a combination of SEND(REC) and RECEIVE is not fatal. Larger groups
       * or other combinations indicate a deadlock.  
       */
      ಆದರೆ (src_dst_e == cp->p_endpoint) {	/* possible deadlock */
	  ಆದರೆ (ಗುಂಪು_ಗಾತ್ರ == 2) {		/* caller and src_dst */
	      /* The function number is magically converted to flags. */
	      ಆದರೆ ((xp->p_rts_flags ^ (function << 2)) & RTS_SENDING) { 
	          ಹಿಂತಿರುಗಿ(0);			/* not a deadlock */
	      }
	  }
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
	  {
		ಪೂರ್ಣಾಂಕ i;
		ಬರೆ("deadlock between these processes:\n");
		ಗಾಗಿ(i = 0; i < ಗುಂಪು_ಗಾತ್ರ; i++) {
			ಬರೆ(" %10s ", processes[i]->p_name);
		}
		ಬರೆ("\n\n");
		ಗಾಗಿ(i = 0; i < ಗುಂಪು_ಗಾತ್ರ; i++) {
			print_proc(processes[i]);
			proc_stacktrace(processes[i]);
		}
	  }
#ಆದರೆ_ಅಂತ್ಯ
          ಹಿಂತಿರುಗಿ(ಗುಂಪು_ಗಾತ್ರ);			/* deadlock found */
      }
  }
  ಹಿಂತಿರುಗಿ(0);					/* not a deadlock */
}

/*===========================================================================*
 *				has_pending				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_pending(sys_map_t *map, ಪೂರ್ಣಾಂಕ src_p, ಪೂರ್ಣಾಂಕ asynm)
{
/* Check to see if there is a pending message from the desired source
 * available.
 */

  ಪೂರ್ಣಾಂಕ src_id;
  sys_id_t id = NULL_PRIV_ID;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  ರಚನೆ proc * p;
#ಆದರೆ_ಅಂತ್ಯ

  /* Either check a specific bit in the mask map, or find the first bit set in
   * it (if any), depending on whether the receive was called on a specific
   * source endpoint.
   */
  ಆದರೆ (src_p != ANY) {
	src_id = nr_ಗಮ್ಯ_id(src_p);
	ಆದರೆ (get_sys_ಬಿಟ್(*map, src_id)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
		p = proc_addr(id_ಗಮ್ಯ_nr(src_id));
		ಆದರೆ (asynm && RTS_ISSET(p, RTS_VMINHIBIT))
			p->p_misc_flags |= MF_SENDA_VM_ತಪ್ಪಿದ;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			id = src_id;
	}
  } ಇಲ್ಲದಿದ್ದರೆ {
	/* Find a source with a pending message */
	ಗಾಗಿ (src_id = 0; src_id < NR_SYS_PROCS; src_id += BITCHUNK_ಬಿಟ್ಟುಗಳು) {
		ಆದರೆ (get_sys_ಬಿಟ್ಟುಗಳು(*map, src_id) != 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
			ಆಗ (src_id < NR_SYS_PROCS) {
				ಆಗ (!get_sys_ಬಿಟ್(*map, src_id)) {
					ಆದರೆ (src_id == NR_SYS_PROCS)
						ಹೋಗು quit_ಹುಡುಕು;
					src_id++;
				}
				p = proc_addr(id_ಗಮ್ಯ_nr(src_id));
				/*
				 * We must not let kernel fiddle with pages of a
				 * process which are currently being changed by
				 * VM.  It is dangerous! So do not report such a
				 * process as having pending async messages.
				 * Skip it.
				 */
				ಆದರೆ (asynm && RTS_ISSET(p, RTS_VMINHIBIT)) {
					p->p_misc_flags |= MF_SENDA_VM_ತಪ್ಪಿದ;
					src_id++;
				} ಇಲ್ಲದಿದ್ದರೆ
					ಹೋಗು quit_ಹುಡುಕು;
			}
#ಇಲ್ಲದಿದ್ದರೆ
			ಆಗ (!get_sys_ಬಿಟ್(*map, src_id)) src_id++;
			ಹೋಗು quit_ಹುಡುಕು;
#ಆದರೆ_ಅಂತ್ಯ
		}
	}

quit_ಹುಡುಕು:
	ಆದರೆ (src_id < NR_SYS_PROCS)	/* Found one */
		id = src_id;
  }

  ಹಿಂತಿರುಗಿ(id);
}

/*===========================================================================*
 *				has_pending_notify			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ has_pending_notify(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	ಹಿಂತಿರುಗಿ has_pending(map, src_p, 0);
}

/*===========================================================================*
 *				has_pending_asend			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ has_pending_asend(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_asyn_pending;
	ಹಿಂತಿರುಗಿ has_pending(map, src_p, 1);
}

/*===========================================================================*
 *				unset_notify_pending			     *
 *===========================================================================*/
ಶೂನ್ಯ unset_notify_pending(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p)
{
	sys_map_t * map = &priv(caller)->s_notify_pending;
	unset_sys_ಬಿಟ್(*map, src_p);
}

/*===========================================================================*
 *				mini_send				     * 
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ mini_send(
  ನೋಂದಣಿ ರಚನೆ proc *caller_ptr,	/* who is trying to send a message? */
  endpoint_t dst_e,			/* to whom is message being sent? */
  ಸಂದೇಶ *m_ptr,			/* pointer to message buffer */
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags
)
{
/* Send a message from 'caller_ptr' to 'dst'. If 'dst' is blocked waiting
 * for this message, copy the message to it and unblock 'dst'. If 'dst' is
 * not waiting at all, or is waiting for another source, queue 'caller_ptr'.
 */
  ನೋಂದಣಿ ರಚನೆ proc *dst_ptr;
  ನೋಂದಣಿ ರಚನೆ proc **xpp;
  ಪೂರ್ಣಾಂಕ dst_p;
  dst_p = _ENDPOINT_P(dst_e);
  dst_ptr = proc_addr(dst_p);

  ಆದರೆ (RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT))
  {
	ಹಿಂತಿರುಗಿ EDEADSRCDST;
  }

  /* Check if 'dst' is blocked waiting for this message. The destination's 
   * RTS_SENDING flag may be set when its SENDREC call blocked while sending.  
   */
  ಆದರೆ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, (vir_ಬೈಟುಗಳು)m_ptr, ಖಾಲಿ)) {
	ಪೂರ್ಣಾಂಕ call;
	/* Destination is indeed waiting for this message. */
	ಪ್ರತಿಜ್ಞಾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));	

	ಆದರೆ (!(flags & FROM_KERNEL)) {
		ಆದರೆ(copy_ಸಂದೇಶ_from_user(m_ptr, &dst_ptr->p_delivermsg))
			ಹಿಂತಿರುಗಿ EFAULT;
	} ಇಲ್ಲದಿದ್ದರೆ {
		dst_ptr->p_delivermsg = *m_ptr;
		IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_FLAGS(dst_ptr, IPC_FLG_ಸಂದೇಶ_FROM_KERNEL);
	}

	dst_ptr->p_delivermsg.m_ಮೂಲ = caller_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;

	call = (caller_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND ? SENDREC
		: (flags & NON_BLOCKING ? SENDNB : SEND));
	IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(dst_ptr, call);

	ಆದರೆ (dst_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND)
		dst_ptr->p_misc_flags &= ~MF_ಉತ್ತರ_PEND;

	RTS_UNSET(dst_ptr, RTS_RECEIVING);

#ಆದರೆ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#ಆದರೆ_ಅಂತ್ಯ
  } ಇಲ್ಲದಿದ್ದರೆ {
	ಆದರೆ(flags & NON_BLOCKING) {
		ಹಿಂತಿರುಗಿ(ENOTREADY);
	}

	/* Check for a possible deadlock before actually blocking. */
	ಆದರೆ (deadlock(SEND, caller_ptr, dst_e)) {
		ಹಿಂತಿರುಗಿ(ELOCKED);
	}

	/* Destination is not waiting.  Block and dequeue caller. */
	ಆದರೆ (!(flags & FROM_KERNEL)) {
		ಆದರೆ(copy_ಸಂದೇಶ_from_user(m_ptr, &caller_ptr->p_sendmsg))
			ಹಿಂತಿರುಗಿ EFAULT;
	} ಇಲ್ಲದಿದ್ದರೆ {
		caller_ptr->p_sendmsg = *m_ptr;
		/*
		 * we need to remember that this message is from kernel so we
		 * can set the delivery status flags when the message is
		 * actually delivered
		 */
		caller_ptr->p_misc_flags |= MF_SENDING_FROM_KERNEL;
	}

	RTS_ಗಣ(caller_ptr, RTS_SENDING);
	caller_ptr->p_sendto_e = dst_e;

	/* Process is now blocked.  Put in on the destination's queue. */
	ಪ್ರತಿಜ್ಞಾ(caller_ptr->p_q_link == ಖಾಲಿ);
	xpp = &dst_ptr->p_caller_q;		/* find end of list */
	ಆಗ (*xpp) xpp = &(*xpp)->p_q_link;	
	*xpp = caller_ptr;			/* add caller to end */

#ಆದರೆ DEBUG_IPC_HOOK
	hook_ipc_msgsend(&caller_ptr->p_sendmsg, caller_ptr, dst_ptr);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ(OK);
}

/*===========================================================================*
 *				mini_receive				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mini_receive(ರಚನೆ proc * caller_ptr,
			endpoint_t src_e, /* which message source is wanted */
			ಸಂದೇಶ * m_buff_usr, /* pointer to message buffer */
			ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags)
{
/* A process or task wants to get a message.  If a message is already queued,
 * acquire it and deblock the sender.  If no message from the desired source
 * is available block the caller.
 */
  ನೋಂದಣಿ ರಚನೆ proc **xpp;
  ಪೂರ್ಣಾಂಕ r, src_id, ಸಿಕ್ಕಿತು, src_proc_nr, src_p;
  endpoint_t sender_e;

  ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));

  /* This is where we want our message. */
  caller_ptr->p_delivermsg_vir = (vir_ಬೈಟುಗಳು) m_buff_usr;

  ಆದರೆ(src_e == ANY) src_p = ANY;
  ಇಲ್ಲದಿದ್ದರೆ
  {
	okendpt(src_e, &src_p);
	ಆದರೆ (RTS_ISSET(proc_addr(src_p), RTS_NO_ENDPOINT))
	{
		ಹಿಂತಿರುಗಿ EDEADSRCDST;
	}
  }


  /* Check to see if a message from desired source is already available.  The
   * caller's RTS_SENDING flag may be set if SENDREC couldn't send. If it is
   * set, the process should be blocked.
   */
  ಆದರೆ (!RTS_ISSET(caller_ptr, RTS_SENDING)) {

    /* Check if there are pending notifications, except for SENDREC. */
    ಆದರೆ (! (caller_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND)) {

	/* Check for pending notifications */
        src_id = has_pending_notify(caller_ptr, src_p);
        ಸಿಕ್ಕಿತು = src_id != NULL_PRIV_ID;
        ಆದರೆ(ಸಿಕ್ಕಿತು) {
            src_proc_nr = id_ಗಮ್ಯ_nr(src_id);		/* get source proc */
            sender_e = proc_addr(src_proc_nr)->p_endpoint;
        }

        ಆದರೆ (ಸಿಕ್ಕಿತು && CANRECEIVE(src_e, sender_e, caller_ptr, 0,
          &m_notify_buff)) {

#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
	    ಆದರೆ(src_proc_nr == NONE) {
		ಬರೆ("mini_receive: sending notify from NONE\n");
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಪ್ರತಿಜ್ಞಾ(src_proc_nr != NONE);
            unset_notify_pending(caller_ptr, src_id);	/* no longer pending */

            /* Found a suitable source, deliver the notification message. */
	    ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));	
	    ಪ್ರತಿಜ್ಞಾ(src_e == ANY || sender_e == src_e);

	    /* assemble message */
	    Build_Notify_ಸಂದೇಶ(&caller_ptr->p_delivermsg, src_proc_nr, caller_ptr);
	    caller_ptr->p_delivermsg.m_ಮೂಲ = sender_e;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;

	    IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(caller_ptr, NOTIFY);

	    ಹೋಗು receive_ಮುಗಿದ;
        }
    }

    /* Check for pending asynchronous messages */
    ಆದರೆ (has_pending_asend(caller_ptr, src_p) != NULL_PRIV_ID) {
        ಆದರೆ (src_p != ANY)
		r = try_one(src_e, proc_addr(src_p), caller_ptr);
        ಇಲ್ಲದಿದ್ದರೆ
        	r = try_async(caller_ptr);

	ಆದರೆ (r == OK) {
            IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(caller_ptr, SENDA);
            ಹೋಗು receive_ಮುಗಿದ;
        }
    }

    /* Check caller queue. Use pointer pointers to keep code simple. */
    xpp = &caller_ptr->p_caller_q;
    ಆಗ (*xpp) {
	ರಚನೆ proc * sender = *xpp;
	endpoint_t sender_e = sender->p_endpoint;

        ಆದರೆ (CANRECEIVE(src_e, sender_e, caller_ptr, 0, &sender->p_sendmsg)) {
            ಪೂರ್ಣಾಂಕ call;
	    ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(sender, RTS_ಸ್ಥಾನ_FREE));
	    ಪ್ರತಿಜ್ಞಾ(!RTS_ISSET(sender, RTS_NO_ENDPOINT));

	    /* Found acceptable message. Copy it and update status. */
  	    ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	    caller_ptr->p_delivermsg = sender->p_sendmsg;
	    caller_ptr->p_delivermsg.m_ಮೂಲ = sender->p_endpoint;
	    caller_ptr->p_misc_flags |= MF_DELIVERMSG;
	    RTS_UNSET(sender, RTS_SENDING);

	    call = (sender->p_misc_flags & MF_ಉತ್ತರ_PEND ? SENDREC : SEND);
	    IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(caller_ptr, call);

	    /*
	     * if the message is originally from the kernel on behalf of this
	     * process, we must send the status flags accordingly
	     */
	    ಆದರೆ (sender->p_misc_flags & MF_SENDING_FROM_KERNEL) {
		IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_FLAGS(caller_ptr, IPC_FLG_ಸಂದೇಶ_FROM_KERNEL);
		/* we can clean the flag now, not need anymore */
		sender->p_misc_flags &= ~MF_SENDING_FROM_KERNEL;
	    }
	    ಆದರೆ (sender->p_misc_flags & MF_SIG_DELAY)
		sig_delay_ಮುಗಿದ(sender);

#ಆದರೆ DEBUG_IPC_HOOK
            hook_ipc_msgrecv(&caller_ptr->p_delivermsg, *xpp, caller_ptr);
#ಆದರೆ_ಅಂತ್ಯ
		
            *xpp = sender->p_q_link;		/* remove from queue */
	    sender->p_q_link = ಖಾಲಿ;
	    ಹೋಗು receive_ಮುಗಿದ;
	}
	xpp = &sender->p_q_link;		/* proceed to next */
    }
  }

  /* No suitable message is available or the caller couldn't send in SENDREC. 
   * Block the process trying to receive, unless the flags tell otherwise.
   */
  ಆದರೆ ( ! (flags & NON_BLOCKING)) {
      /* Check for a possible deadlock before actually blocking. */
      ಆದರೆ (deadlock(RECEIVE, caller_ptr, src_e)) {
          ಹಿಂತಿರುಗಿ(ELOCKED);
      }

      caller_ptr->p_getfrom_e = src_e;		
      RTS_ಗಣ(caller_ptr, RTS_RECEIVING);
      ಹಿಂತಿರುಗಿ(OK);
  } ಇಲ್ಲದಿದ್ದರೆ {
	ಹಿಂತಿರುಗಿ(ENOTREADY);
  }

receive_ಮುಗಿದ:
  ಆದರೆ (caller_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND)
	  caller_ptr->p_misc_flags &= ~MF_ಉತ್ತರ_PEND;
  ಹಿಂತಿರುಗಿ OK;
}

/*===========================================================================*
 *				mini_notify				     * 
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ mini_notify(
  ಸ್ಥಿರಾಂಕ ರಚನೆ proc *caller_ptr,	/* sender of the notification */
  endpoint_t dst_e			/* which process to notify */
)
{
  ನೋಂದಣಿ ರಚನೆ proc *dst_ptr;
  ಪೂರ್ಣಾಂಕ src_id;				/* source id for late delivery */
  ಪೂರ್ಣಾಂಕ dst_p;

  ಆದರೆ (!isokendpt(dst_e, &dst_p)) {
	util_stacktrace();
	ಬರೆ("mini_notify: bogus endpoint %d\n", dst_e);
	ಹಿಂತಿರುಗಿ EDEADSRCDST;
  }

  dst_ptr = proc_addr(dst_p);

  /* Check to see if target is blocked waiting for this message. A process 
   * can be both sending and receiving during a SENDREC system call.
   */
  ಆದರೆ (WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr, 0, &m_notify_buff) &&
    !(dst_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND)) {
      /* Destination is indeed waiting for a message. Assemble a notification 
       * message and deliver it. Copy from pseudo-source HARDWARE, since the
       * message is in the kernel's address space.
       */ 
      ಪ್ರತಿಜ್ಞಾ(!(dst_ptr->p_misc_flags & MF_DELIVERMSG));

      Build_Notify_ಸಂದೇಶ(&dst_ptr->p_delivermsg, proc_nr(caller_ptr), dst_ptr);
      dst_ptr->p_delivermsg.m_ಮೂಲ = caller_ptr->p_endpoint;
      dst_ptr->p_misc_flags |= MF_DELIVERMSG;

      IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(dst_ptr, NOTIFY);
      RTS_UNSET(dst_ptr, RTS_RECEIVING);

      ಹಿಂತಿರುಗಿ(OK);
  } 

  /* Destination is not ready to receive the notification. Add it to the 
   * bit map with pending notifications. Note the indirectness: the privilege id
   * instead of the process number is used in the pending bit map.
   */ 
  src_id = priv(caller_ptr)->s_id;
  ಗಣ_sys_ಬಿಟ್(priv(dst_ptr)->s_notify_pending, src_id); 
  ಹಿಂತಿರುಗಿ(OK);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ASCOMPLAIN(caller, ನಮೂದು, ಕ್ಷೇತ್ರ)	\
	ಬರೆ("kernel:%s:%d: asyn failed for %s in %s "	\
	"(%d/%zu, tab 0x%lx)\n",__FILE__,__LINE__,	\
ಕ್ಷೇತ್ರ, caller->p_name, ನಮೂದು, priv(caller)->s_asynsize, priv(caller)->s_asyntab)

#ವ್ಯಾಖ್ಯಾನಿಸು A_RETR(ನಮೂದು) ಮಾಡು {			\
  ಆದರೆ (ದತ್ತಾಂಶ_copy(				\
  		caller_ptr->p_endpoint, ಕೋಷ್ಟಕ_v + (ನಮೂದು)*ಗಾತ್ರ(asynmsg_t),\
  		KERNEL, (vir_ಬೈಟುಗಳು) &tabent,	\
  		ಗಾತ್ರ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, ನಮೂದು, "message entry");	\
  			r = EFAULT;		\
	                ಹೋಗು asyn_error; \
  }						\
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tabent.dst == SELF) { \
      tabent.dst = caller_ptr->p_endpoint; \
  } \
  			 } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು A_INSRT(ನಮೂದು) ಮಾಡು {			\
  ಆದರೆ (ದತ್ತಾಂಶ_copy(KERNEL, (vir_ಬೈಟುಗಳು) &tabent,	\
  		caller_ptr->p_endpoint, ಕೋಷ್ಟಕ_v + (ನಮೂದು)*ಗಾತ್ರ(asynmsg_t),\
  		ಗಾತ್ರ(tabent)) != OK) {	\
  			ASCOMPLAIN(caller_ptr, ನಮೂದು, "message entry");	\
			/* Do NOT set r or goto asyn_error here! */ \
  }						\
  			  } ಆಗ(0)	

/*===========================================================================*
 *				try_deliver_senda			     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ try_deliver_senda(ರಚನೆ proc *caller_ptr,
				asynmsg_t *ಕೋಷ್ಟಕ,
				size_t ಗಾತ್ರ_2)
{
  ಪೂರ್ಣಾಂಕ r, dst_p, ಮುಗಿದ, do_notify;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ಚಿಹ್ನೆರಹಿತ flags;
  endpoint_t dst;
  ರಚನೆ proc *dst_ptr;
  ರಚನೆ priv *privp;
  asynmsg_t tabent;
  ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು ಕೋಷ್ಟಕ_v = (vir_ಬೈಟುಗಳು) ಕೋಷ್ಟಕ;
  ಸಂದೇಶ *m_ptr = ಖಾಲಿ;

  privp = priv(caller_ptr);

  /* Clear table */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  privp->s_asynendpoint = caller_ptr->p_endpoint;

  ಆದರೆ (ಗಾತ್ರ_2 == 0) ಹಿಂತಿರುಗಿ(OK);  /* Nothing to do, just return */

  /* Scan the table */
  do_notify = FALSE;
  ಮುಗಿದ = TRUE;

  /* Limit size to something reasonable. An arbitrary choice is 16
   * times the number of process table entries.
   *
   * (this check has been duplicated in sys_call but is left here
   * as a sanity check)
   */
  ಆದರೆ (ಗಾತ್ರ_2 > 16*(NR_TASKS + NR_PROCS)) {
    r = EDOM;
    ಹಿಂತಿರುಗಿ r;
  }

  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++) {
	/* Process each entry in the table and store the result in the table.
	 * If we're done handling a message, copy the result to the sender. */

	dst = NONE;
	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಆದರೆ (flags == 0) ಮುಂದುವರಿ; /* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಆದರೆ(flags & ~(AMF_VALID|AMF_ಮುಗಿದ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR)) {
		r = EINVAL;
		ಹೋಗು asyn_error;
	}
	ಆದರೆ (!(flags & AMF_VALID)) { /* Must contain message */
		r = EINVAL;
		ಹೋಗು asyn_error;
	}
	ಆದರೆ (flags & AMF_ಮುಗಿದ) ಮುಂದುವರಿ;	/* Already done processing */

	r = OK;
	ಆದರೆ (!isokendpt(tabent.dst, &dst_p)) 
		r = EDEADSRCDST; /* Bad destination, report the error */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iskerneln(dst_p)) 
		r = ECALLDENIED; /* Asyn sends to the kernel are not allowed */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!may_asynsend_ಗಮ್ಯ(caller_ptr, dst_p))
		r = ECALLDENIED; /* Send denied by IPC mask */
	ಇಲ್ಲದಿದ್ದರೆ 	/* r == OK */
		dst_ptr = proc_addr(dst_p);

	/* XXX: RTS_NO_ENDPOINT should be removed */
	ಆದರೆ (r == OK && RTS_ISSET(dst_ptr, RTS_NO_ENDPOINT)) {
		r = EDEADSRCDST;
	}

	/* Check if 'dst' is blocked waiting for this message.
	 * If AMF_NOREPLY is set, do not satisfy the receiving part of
	 * a SENDREC.
	 */
	ಆದರೆ (r == OK && WILLRECEIVE(caller_ptr->p_endpoint, dst_ptr,
	    (vir_ಬೈಟುಗಳು)&ಕೋಷ್ಟಕ[i].ಸಂದೇಶ_2, ಖಾಲಿ) &&
	    (!(flags&AMF_NOREPLY) || !(dst_ptr->p_misc_flags&MF_ಉತ್ತರ_PEND))) {
		/* Destination is indeed waiting for this message. */
		dst_ptr->p_delivermsg = tabent.ಸಂದೇಶ_2;
		dst_ptr->p_delivermsg.m_ಮೂಲ = caller_ptr->p_endpoint;
		dst_ptr->p_misc_flags |= MF_DELIVERMSG;
		IPC_ಸ್ಥಿತಿ_ಕೂಡಿಸು_CALL(dst_ptr, SENDA);
		RTS_UNSET(dst_ptr, RTS_RECEIVING);
#ಆದರೆ DEBUG_IPC_HOOK
		hook_ipc_msgrecv(&dst_ptr->p_delivermsg, caller_ptr, dst_ptr);
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == OK) {
		/* Inform receiver that something is pending */
		ಗಣ_sys_ಬಿಟ್(priv(dst_ptr)->s_asyn_pending, 
			    priv(caller_ptr)->s_id); 
		ಮುಗಿದ = FALSE;
		ಮುಂದುವರಿ;
	} 

	/* Store results */
	tabent.ಫಲಿತಾಂಶ = r;
	tabent.flags = flags | AMF_ಮುಗಿದ;
	ಆದರೆ (flags & AMF_NOTIFY)
		do_notify = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r != OK && (flags & AMF_NOTIFY_ERR))
		do_notify = TRUE;
	A_INSRT(i);	/* Copy results to caller; ignore errors */
	ಮುಂದುವರಿ;

asyn_error:
	ಆದರೆ (dst != NONE)
		ಬರೆ("KERNEL senda error %d to %d\n", r, dst);
	ಇಲ್ಲದಿದ್ದರೆ
		ಬರೆ("KERNEL senda error %d\n", r);
  }

  ಆದರೆ (do_notify) 
	mini_notify(proc_addr(ASYNCM), caller_ptr->p_endpoint);

  ಆದರೆ (!ಮುಗಿದ) {
	privp->s_asyntab = (vir_ಬೈಟುಗಳು) ಕೋಷ್ಟಕ;
	privp->s_asynsize = ಗಾತ್ರ_2;
  }

  ಹಿಂತಿರುಗಿ(OK);
}

/*===========================================================================*
 *				mini_senda				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mini_senda(ರಚನೆ proc *caller_ptr, asynmsg_t *ಕೋಷ್ಟಕ, size_t ಗಾತ್ರ_2)
{
  ರಚನೆ priv *privp;

  privp = priv(caller_ptr);
  ಆದರೆ (!(privp->s_flags & SYS_PROC)) {
	ಬರೆ( "mini_senda: warning caller has no privilege structure\n");
	ಹಿಂತಿರುಗಿ(EPERM);
  }

  ಹಿಂತಿರುಗಿ try_deliver_senda(caller_ptr, ಕೋಷ್ಟಕ, ಗಾತ್ರ_2);
}


/*===========================================================================*
 *				try_async				     * 
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ try_async(ರಚನೆ proc * caller_ptr)
{
  ಪೂರ್ಣಾಂಕ r;
  ರಚನೆ priv *privp;
  ರಚನೆ proc *src_ptr;
  sys_map_t *map;

  map = &priv(caller_ptr)->s_asyn_pending;

  /* Try all privilege structures */
  ಗಾಗಿ (privp = BEG_PRIV_ADDR; privp < ಕೊನೆ_PRIV_ADDR; ++privp)  {
	ಆದರೆ (privp->s_proc_nr == NONE)
		ಮುಂದುವರಿ;

	ಆದರೆ (!get_sys_ಬಿಟ್(*map, privp->s_id)) 
		ಮುಂದುವರಿ;

	src_ptr = proc_addr(privp->s_proc_nr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	/*
	 * Do not copy from a process which does not have a stable address space
	 * due to VM fiddling with it
	 */
	ಆದರೆ (RTS_ISSET(src_ptr, RTS_VMINHIBIT)) {
		src_ptr->p_misc_flags |= MF_SENDA_VM_ತಪ್ಪಿದ;
		ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಪ್ರತಿಜ್ಞಾ(!(caller_ptr->p_misc_flags & MF_DELIVERMSG));
	ಆದರೆ ((r = try_one(ANY, src_ptr, caller_ptr)) == OK)
		ಹಿಂತಿರುಗಿ(r);
  }

  ಹಿಂತಿರುಗಿ(ESRCH);
}


/*===========================================================================*
 *				try_one					     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ try_one(endpoint_t receive_e, ರಚನೆ proc *src_ptr,
    ರಚನೆ proc *dst_ptr)
{
/* Try to receive an asynchronous message from 'src_ptr' */
  ಪೂರ್ಣಾಂಕ r = EAGAIN, ಮುಗಿದ, do_notify;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags, i;
  size_t ಗಾತ್ರ_2;
  endpoint_t dst, src_e;
  ರಚನೆ proc *caller_ptr;
  ರಚನೆ priv *privp;
  asynmsg_t tabent;
  vir_ಬೈಟುಗಳು ಕೋಷ್ಟಕ_v;

  privp = priv(src_ptr);
  ಆದರೆ (!(privp->s_flags & SYS_PROC)) ಹಿಂತಿರುಗಿ(EPERM);
  ಗಾತ್ರ_2 = privp->s_asynsize;
  ಕೋಷ್ಟಕ_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  unset_sys_ಬಿಟ್(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ಆದರೆ (ಗಾತ್ರ_2 == 0) ಹಿಂತಿರುಗಿ(EAGAIN);
  ಆದರೆ (privp->s_asynendpoint != src_ptr->p_endpoint) ಹಿಂತಿರುಗಿ EAGAIN;
  ಆದರೆ (!may_asynsend_ಗಮ್ಯ(src_ptr, proc_nr(dst_ptr))) ಹಿಂತಿರುಗಿ (ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */
  src_e = src_ptr->p_endpoint;

  /* Scan the table */
  do_notify = FALSE;
  ಮುಗಿದ = TRUE;

  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the meantime.
  	 */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಆದರೆ (flags == 0) ಮುಂದುವರಿ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಆದರೆ(flags & ~(AMF_VALID|AMF_ಮುಗಿದ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & AMF_ಮುಗಿದ) ಮುಂದುವರಿ; /* Already done processing */

	/* Clear done flag. The sender is done sending when all messages in the
	 * table are marked done or empty. However, we will know that only
	 * the next time we enter this function or when the sender decides to
	 * send additional asynchronous messages and manages to deliver them
	 * all.
	 */
	ಮುಗಿದ = FALSE;

	ಆದರೆ (r == EINVAL)
		ಹೋಗು store_ಫಲಿತಾಂಶ;

	/* Message must be directed at receiving end */
	ಆದರೆ (dst != dst_ptr->p_endpoint) ಮುಂದುವರಿ;

	ಆದರೆ (!CANRECEIVE(receive_e, src_e, dst_ptr,
		ಕೋಷ್ಟಕ_v + i*ಗಾತ್ರ(asynmsg_t) + offsetof(ರಚನೆ asynmsg,ಸಂದೇಶ_2),
		ಖಾಲಿ)) {
		ಮುಂದುವರಿ;
	}

	/* If AMF_NOREPLY is set, then this message is not a reply to a
	 * SENDREC and thus should not satisfy the receiving part of the
	 * SENDREC. This message is to be delivered later.
	 */
	ಆದರೆ ((flags & AMF_NOREPLY) && (dst_ptr->p_misc_flags & MF_ಉತ್ತರ_PEND)) 
		ಮುಂದುವರಿ;

	/* Destination is ready to receive the message; deliver it */
	r = OK;
	dst_ptr->p_delivermsg = tabent.ಸಂದೇಶ_2;
	dst_ptr->p_delivermsg.m_ಮೂಲ = src_ptr->p_endpoint;
	dst_ptr->p_misc_flags |= MF_DELIVERMSG;
#ಆದರೆ DEBUG_IPC_HOOK
	hook_ipc_msgrecv(&dst_ptr->p_delivermsg, src_ptr, dst_ptr);
#ಆದರೆ_ಅಂತ್ಯ

store_ಫಲಿತಾಂಶ:
	/* Store results for sender. We may just have started delivering a
	 * message, so we must not return an error to the caller in the case
	 * that storing the results triggers an error!
	 */
	tabent.ಫಲಿತಾಂಶ = r;
	tabent.flags = flags | AMF_ಮುಗಿದ;
	ಆದರೆ (flags & AMF_NOTIFY) do_notify = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */

	ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ಆದರೆ (ಮುಗಿದ) {
	privp->s_asyntab = -1;
	privp->s_asynsize = 0;
  } ಇಲ್ಲದಿದ್ದರೆ {
	ಗಣ_sys_ಬಿಟ್(priv(dst_ptr)->s_asyn_pending, privp->s_id);
  }

asyn_error:
  ಹಿಂತಿರುಗಿ(r);
}

/*===========================================================================*
 *				cancel_async				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ cancel_async(ರಚನೆ proc *src_ptr, ರಚನೆ proc *dst_ptr)
{
/* Cancel asynchronous messages from src to dst, because dst is not interested
 * in them (e.g., dst has been restarted) */
  ಪೂರ್ಣಾಂಕ ಮುಗಿದ, do_notify;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags, i;
  size_t ಗಾತ್ರ_2;
  endpoint_t dst;
  ರಚನೆ proc *caller_ptr;
  ರಚನೆ priv *privp;
  asynmsg_t tabent;
  vir_ಬೈಟುಗಳು ಕೋಷ್ಟಕ_v;

  privp = priv(src_ptr);
  ಆದರೆ (!(privp->s_flags & SYS_PROC)) ಹಿಂತಿರುಗಿ(EPERM);
  ಗಾತ್ರ_2 = privp->s_asynsize;
  ಕೋಷ್ಟಕ_v = privp->s_asyntab;

  /* Clear table pending message flag. We're done unless we're not. */
  privp->s_asyntab = -1;
  privp->s_asynsize = 0;
  unset_sys_ಬಿಟ್(priv(dst_ptr)->s_asyn_pending, privp->s_id);

  ಆದರೆ (ಗಾತ್ರ_2 == 0) ಹಿಂತಿರುಗಿ(EAGAIN);
  ಆದರೆ (!may_send_ಗಮ್ಯ(src_ptr, proc_nr(dst_ptr))) ಹಿಂತಿರುಗಿ(ECALLDENIED);

  caller_ptr = src_ptr;	/* Needed for A_ macros later on */

  /* Scan the table */
  do_notify = FALSE;
  ಮುಗಿದ = TRUE;


  ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++) {
  	/* Process each entry in the table and store the result in the table.
  	 * If we're done handling a message, copy the result to the sender.
  	 * Some checks done in mini_senda are duplicated here, as the sender
  	 * could've altered the contents of the table in the mean time.
  	 */

  	ಪೂರ್ಣಾಂಕ r = EDEADSRCDST;	/* Cancel delivery due to dead dst */

	/* Copy message to kernel */
	A_RETR(i);
	flags = tabent.flags;
	dst = tabent.dst;

	ಆದರೆ (flags == 0) ಮುಂದುವರಿ;	/* Skip empty entries */

	/* 'flags' field must contain only valid bits */
	ಆದರೆ(flags & ~(AMF_VALID|AMF_ಮುಗಿದ|AMF_NOTIFY|AMF_NOREPLY|AMF_NOTIFY_ERR))
		r = EINVAL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & AMF_VALID)) /* Must contain message */
		r = EINVAL; 
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & AMF_ಮುಗಿದ) ಮುಂದುವರಿ; /* Already done processing */

	/* Message must be directed at receiving end */
	ಆದರೆ (dst != dst_ptr->p_endpoint) {
		ಮುಗಿದ = FALSE;
		ಮುಂದುವರಿ;
	}

	/* Store results for sender */
	tabent.ಫಲಿತಾಂಶ = r;
	tabent.flags = flags | AMF_ಮುಗಿದ;
	ಆದರೆ (flags & AMF_NOTIFY) do_notify = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r != OK && (flags & AMF_NOTIFY_ERR)) do_notify = TRUE;
	A_INSRT(i);	/* Copy results to sender; ignore errors */
  }

  ಆದರೆ (do_notify) 
	mini_notify(proc_addr(ASYNCM), src_ptr->p_endpoint);

  ಆದರೆ (!ಮುಗಿದ) {
	privp->s_asyntab = ಕೋಷ್ಟಕ_v;
	privp->s_asynsize = ಗಾತ್ರ_2;
  }

asyn_error:
  ಹಿಂತಿರುಗಿ(OK);
}

/*===========================================================================*
 *				enqueue					     * 
 *===========================================================================*/
ಶೂನ್ಯ ಸರತಿಗೆ_ಸೇರಿಸು(
  ನೋಂದಣಿ ರಚನೆ proc *rp	/* this process is now runnable */
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
  ಪೂರ್ಣಾಂಕ q = rp->p_ಆದ್ಯತೆ;	 		/* scheduling queue to use */
  ರಚನೆ proc **rdy_ತಲೆ, **rdy_ಬಾಲ;
  
  ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));

  ಪ್ರತಿಜ್ಞಾ(q >= 0);

  rdy_ತಲೆ = get_cpu_var(rp->p_cpu, ಸತತ_q_ತಲೆ);
  rdy_ಬಾಲ = get_cpu_var(rp->p_cpu, ಸತತ_q_ಬಾಲ);

  /* Now add the process to the queue. */
  ಆದರೆ (!rdy_ತಲೆ[q]) {		/* add to empty queue */
      rdy_ತಲೆ[q] = rdy_ಬಾಲ[q] = rp; 		/* create a new queue */
      rp->p_nextready = ಖಾಲಿ;		/* mark new end */
  } 
  ಇಲ್ಲದಿದ್ದರೆ {					/* add to tail of queue */
      rdy_ಬಾಲ[q]->p_nextready = rp;		/* chain tail of queue */	
      rdy_ಬಾಲ[q] = rp;				/* set new queue tail */
      rp->p_nextready = ಖಾಲಿ;		/* mark new end */
  }

  ಆದರೆ (cpuid == rp->p_cpu) {
	  /*
	   * enqueueing a process with a higher priority than the current one,
	   * it gets preempted. The current process must be preemptible. Testing
	   * the priority also makes sure that a process does not preempt itself
	   */
	  ರಚನೆ proc * p;
	  p = get_cpulocal_var(proc_ptr);
	  ಪ್ರತಿಜ್ಞಾ(p);
	  ಆದರೆ((p->p_ಆದ್ಯತೆ > rp->p_ಆದ್ಯತೆ) &&
			  (priv(p)->s_flags & PREEMPTIBLE))
		  RTS_ಗಣ(p, RTS_PREEMPTED); /* calls dequeue() */
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  /*
   * if the process was enqueued on a different cpu and the cpu is idle, i.e.
   * the time is off, we need to wake up that cpu and let it schedule this new
   * process
   */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_cpu_var(rp->p_cpu, cpu_is_idle)) {
	  smp_ವೇಳಾಪಟ್ಟಿ(rp->p_cpu);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr)->p_accounting.enter_ಸರತಿ));


#ಆದರೆ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
#ಆದರೆ_ಅಂತ್ಯ
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
ಸ್ಥಿರ ಶೂನ್ಯ ಸರತಿಗೆ_ಸೇರಿಸು_ತಲೆ(ರಚನೆ proc *rp)
{
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ q = rp->p_ಆದ್ಯತೆ;	 		/* scheduling queue to use */

  ರಚನೆ proc **rdy_ತಲೆ, **rdy_ಬಾಲ;

  ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(rp));
  ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));

  /*
   * the process was runnable without its quantum expired when dequeued. A
   * process with no time left should have been handled else and differently
   */
  ಪ್ರತಿಜ್ಞಾ(rp->p_cpu_ಸಮಯ_ಎಡ);

  ಪ್ರತಿಜ್ಞಾ(q >= 0);


  rdy_ತಲೆ = get_cpu_var(rp->p_cpu, ಸತತ_q_ತಲೆ);
  rdy_ಬಾಲ = get_cpu_var(rp->p_cpu, ಸತತ_q_ಬಾಲ);

  /* Now add the process to the queue. */
  ಆದರೆ (!rdy_ತಲೆ[q]) {		/* add to empty queue */
	rdy_ತಲೆ[q] = rdy_ಬಾಲ[q] = rp; 	/* create a new queue */
	rp->p_nextready = ಖಾಲಿ;			/* mark new end */
  } ಇಲ್ಲದಿದ್ದರೆ {					/* add to head of queue */
	rp->p_nextready = rdy_ತಲೆ[q];		/* chain head of queue */
	rdy_ತಲೆ[q] = rp;			/* set new queue head */
  }

  /* Make note of when this process was added to queue */
  read_tsc_64(&(get_cpulocal_var(proc_ptr->p_accounting.enter_ಸರತಿ)));


  /* Process accounting for scheduling */
  rp->p_accounting.dequeues--;
  rp->p_accounting.preempted++;

#ಆದರೆ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
#ಆದರೆ_ಅಂತ್ಯ
}

/*===========================================================================*
 *				dequeue					     * 
 *===========================================================================*/
ಶೂನ್ಯ ಸರತಿಯಿಂದ_ತೆಗೆ(ರಚನೆ proc *rp)
/* this process is no longer runnable */
{
/* A process must be removed from the scheduling queues, for example, because
 * it has blocked.  If the currently active process is removed, a new process
 * is picked to run by calling pick_proc().
 *
 * This function can operate x-cpu as it always removes the process from the
 * queue of the cpu the process is currently assigned to.
 */
  ಪೂರ್ಣಾಂಕ q = rp->p_ಆದ್ಯತೆ;		/* queue to use */
  ರಚನೆ proc **xpp;			/* iterate over queue */
  ರಚನೆ proc *prev_xp;
  u64_t tsc, tsc_delta;

  ರಚನೆ proc **rdy_ಬಾಲ;

  ಪ್ರತಿಜ್ಞಾ(proc_ptr_ok(rp));
  ಪ್ರತಿಜ್ಞಾ(!proc_is_runnable(rp));

  /* Side-effect for kernel: check if the task's stack still is ok? */
  ಪ್ರತಿಜ್ಞಾ (!iskernelp(rp) || *priv(rp)->s_ರಾಶಿ_guard == ರಾಶಿ_GUARD);

  rdy_ಬಾಲ = get_cpu_var(rp->p_cpu, ಸತತ_q_ಬಾಲ);

  /* Now make sure that the process is not in its ready queue. Remove the 
   * process if it is found. A process can be made unready even if it is not 
   * running by being sent a signal that kills it.
   */
  prev_xp = ಖಾಲಿ;				
  ಗಾಗಿ (xpp = get_cpu_var_ptr(rp->p_cpu, ಸತತ_q_ತಲೆ[q]); *xpp;
		  xpp = &(*xpp)->p_nextready) {
      ಆದರೆ (*xpp == rp) {				/* found process to remove */
          *xpp = (*xpp)->p_nextready;		/* replace with next chain */
          ಆದರೆ (rp == rdy_ಬಾಲ[q]) {		/* queue tail removed */
              rdy_ಬಾಲ[q] = prev_xp;		/* set new tail */
	  }

          ನಿಲ್ಲಿಸು;
      }
      prev_xp = *xpp;				/* save previous in chain */
  }

	
  /* Process accounting for scheduling */
  rp->p_accounting.dequeues++;

  /* this is not all that accurate on virtual machines, especially with
     IO bound processes that only spend a short amount of time in the queue
     at a time. */
  ಆದರೆ (rp->p_accounting.enter_ಸರತಿ) {
	read_tsc_64(&tsc);
	tsc_delta = tsc - rp->p_accounting.enter_ಸರತಿ;
	rp->p_accounting.ಸಮಯ_ಒಳ_ಸರತಿ = rp->p_accounting.ಸಮಯ_ಒಳ_ಸರತಿ +
		tsc_delta;
	rp->p_accounting.enter_ಸರತಿ = 0;
  }

  /* For ps(1), remember when the process was last dequeued. */
  rp->p_dequeued = get_monotonic();

#ಆದರೆ DEBUG_SANITYCHECKS
  ಪ್ರತಿಜ್ಞಾ(runqueues_ok_local());
#ಆದರೆ_ಅಂತ್ಯ
}

/*===========================================================================*
 *				pick_proc				     * 
 *===========================================================================*/
ಸ್ಥಿರ ರಚನೆ proc * pick_proc(ಶೂನ್ಯ)
{
/* Decide who to run now.  A new process is selected and returned.
 * When a billable process is selected, record it in 'bill_ptr', so that the 
 * clock task can tell who to bill for system time.
 *
 * This function always uses the run queues of the local cpu!
 */
  ನೋಂದಣಿ ರಚನೆ proc *rp;			/* process to run */
  ರಚನೆ proc **rdy_ತಲೆ;
  ಪೂರ್ಣಾಂಕ q;				/* iterate over queues */

  /* Check each of the scheduling queues for ready processes. The number of
   * queues is defined in proc.h, and priorities are set in the task table.
   * If there are no processes ready to run, return NULL.
   */
  rdy_ತಲೆ = get_cpulocal_var(ಸತತ_q_ತಲೆ);
  ಗಾಗಿ (q=0; q < NR_SCHED_QUEUES; q++) {	
	ಆದರೆ(!(rp = rdy_ತಲೆ[q])) {
		TRACE(VF_PICKPROC, ಬರೆ("cpu %d queue %d empty\n", cpuid, q););
		ಮುಂದುವರಿ;
	}
	ಪ್ರತಿಜ್ಞಾ(proc_is_runnable(rp));
	ಆದರೆ (priv(rp)->s_flags & BILLABLE)	 	
		get_cpulocal_var(bill_ptr) = rp; /* bill for system time */
	ಹಿಂತಿರುಗಿ rp;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*===========================================================================*
 *				endpoint_lookup				     *
 *===========================================================================*/
ರಚನೆ proc *endpoint_lookup(endpoint_t e)
{
	ಪೂರ್ಣಾಂಕ n;

	ಆದರೆ(!isokendpt(e, &n)) ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ proc_addr(n);
}

/*===========================================================================*
 *				isokendpt_f				     *
 *===========================================================================*/
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
ಪೂರ್ಣಾಂಕ isokendpt_f(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * file, ಪೂರ್ಣಾಂಕ ಸಾಲು, endpoint_t e, ಪೂರ್ಣಾಂಕ * p,
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fatalflag)
#ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ isokendpt_f(endpoint_t e, ಪೂರ್ಣಾಂಕ * p, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fatalflag)
#ಆದರೆ_ಅಂತ್ಯ
{
	ಪೂರ್ಣಾಂಕ ok = 0;
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
	ಆದರೆ(isokprocn(*p) && !isemptyn(*p) && proc_addr(*p)->p_endpoint == e)
		ok = 1;
	ಆದರೆ(!ok && fatalflag)
		panic("invalid endpoint: %d",  e);
	ಹಿಂತಿರುಗಿ ok;
}

ಸ್ಥಿರ ಶೂನ್ಯ notify_scheduler(ರಚನೆ proc *p)
{
	ಸಂದೇಶ m_no_ಕಾಲಾಂಶ;
	ಪೂರ್ಣಾಂಕ err;

	ಪ್ರತಿಜ್ಞಾ(!proc_kernel_scheduler(p));

	/* dequeue the process */
	RTS_ಗಣ(p, RTS_NO_ಕಾಲಾಂಶ);
	/*
	 * Notify the process's scheduler that it has run out of
	 * quantum. This is done by sending a message to the scheduler
	 * on the process's behalf
	 */
	m_no_ಕಾಲಾಂಶ.m_ಮೂಲ = p->p_endpoint;
	m_no_ಕಾಲಾಂಶ.m_type   = SCHEDULING_NO_ಕಾಲಾಂಶ;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_ಸರತಿ = cpu_ಸಮಯ_2_ms(p->p_accounting.ಸಮಯ_ಒಳ_ಸರತಿ);
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_deqs      = p->p_accounting.dequeues;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_ipc_sync  = p->p_accounting.ipc_sync;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_ipc_async = p->p_accounting.ipc_async;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_preempt   = p->p_accounting.preempted;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_cpu       = cpuid;
	m_no_ಕಾಲಾಂಶ.m_krn_lsys_ವೇಳಾಪಟ್ಟಿ.acnt_cpu_ತುಂಬು  = cpu_ತುಂಬು();

	/* Reset accounting */
	reset_proc_accounting(p);

	ಆದರೆ ((err = mini_send(p, p->p_scheduler->p_endpoint,
					&m_no_ಕಾಲಾಂಶ, FROM_KERNEL))) {
		panic("WARNING: Scheduling: mini_send returned %d\n", err);
	}
}

ಶೂನ್ಯ proc_no_ಸಮಯ(ರಚನೆ proc * p)
{
	ಆದರೆ (!proc_kernel_scheduler(p) && priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		notify_scheduler(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ {
		/*
		 * non-preemptible processes only need their quantum to
		 * be renewed. In fact, they by pass scheduling
		 */
		p->p_cpu_ಸಮಯ_ಎಡ = ms_2_cpu_ಸಮಯ(p->p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms);
#ಆದರೆ DEBUG_RACE
		RTS_ಗಣ(p, RTS_PREEMPTED);
		RTS_UNSET(p, RTS_PREEMPTED);
#ಆದರೆ_ಅಂತ್ಯ
	}
}

ಶೂನ್ಯ reset_proc_accounting(ರಚನೆ proc *p)
{
  p->p_accounting.preempted = 0;
  p->p_accounting.ipc_sync  = 0;
  p->p_accounting.ipc_async = 0;
  p->p_accounting.dequeues  = 0;
  p->p_accounting.ಸಮಯ_ಒಳ_ಸರತಿ = 0;
  p->p_accounting.enter_ಸರತಿ = 0;
}
	
ಶೂನ್ಯ copr_not_ಲಭ್ಯ_handler(ಶೂನ್ಯ)
{
	ರಚನೆ proc * p;
	ರಚನೆ proc ** local_fpu_owner;
	/*
	 * Disable the FPU exception (both for the kernel and for the process
	 * once it's scheduled), and initialize or restore the FPU state.
	 */

	disable_fpu_exception();

	p = get_cpulocal_var(proc_ptr);

	/* if FPU is not owned by anyone, do not store anything */
	local_fpu_owner = get_cpulocal_var_ptr(fpu_owner);
	ಆದರೆ (*local_fpu_owner != ಖಾಲಿ) {
		ಪ್ರತಿಜ್ಞಾ(*local_fpu_owner != p);
		save_local_fpu(*local_fpu_owner, FALSE /*retain*/);
	}

	/*
	 * restore the current process' state and let it run again, do not
	 * schedule!
	 */
	ಆದರೆ (restore_fpu(p) != OK) {
		/* Restoring FPU state failed. This is always the process's own
		 * fault. Send a signal, and schedule another process instead.
		 */
		*local_fpu_owner = ಖಾಲಿ;		/* release FPU */
		cause_sig(proc_nr(p), SIGFPE);
		ಹಿಂತಿರುಗಿ;
	}

	*local_fpu_owner = p;
	context_stop(proc_addr(KERNEL));
	restore_user_context(p);
	NOT_REACHABLE;
}

ಶೂನ್ಯ release_fpu(ರಚನೆ proc * p) {
	ರಚನೆ proc ** fpu_owner_ptr;

	fpu_owner_ptr = get_cpu_var_ptr(p->p_cpu, fpu_owner);

	ಆದರೆ (*fpu_owner_ptr == p)
		*fpu_owner_ptr = ಖಾಲಿ;
}

ಶೂನ್ಯ ser_dump_proc(ಶೂನ್ಯ)
{
        ರಚನೆ proc *pp;

        ಗಾಗಿ (pp= BEG_PROC_ADDR; pp < ಕೊನೆ_PROC_ADDR; pp++)
        {
                ಆದರೆ (isemptyp(pp))
                        ಮುಂದುವರಿ;
                print_proc_recursive(pp);
        }
}
