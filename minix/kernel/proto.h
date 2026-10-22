/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#यदि_परिभाषितम् __kernel__

#यदि_अपरिभाषितम् PROTO_H
#परिभाषय PROTO_H

#अन्तर्भाव <minix/safecopies.h>
#अन्तर्भाव <machine/archtypes.h>
#अन्तर्भाव <machine/signal.h>
#अन्तर्भाव <machine/frame.h>

/* Struct declarations. */
संरचना proc;
संरचना ipc_filter_s;

/* clock.c */
शून्य init_clock(शून्य);
clock_t get_realtime(शून्य);
शून्य समुच्चयः_realtime(clock_t);
शून्य समुच्चयः_adjtime_delta(int32_t);
clock_t get_monotonic(शून्य);
शून्य समुच्चयः_boottime(time_t);
time_t get_boottime(शून्य);
शून्य समुच्चयः_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, पूर्णाङ्क arg);
शून्य reset_kernel_timer(minix_timer_t *tp);
शून्य ser_dump_proc(शून्य);

शून्य आवृत्तयः_accounting_init(शून्य);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
शून्य context_stop(संरचना proc * p);
/* this is a wrapper to make calling it from assembly easier */
शून्य context_stop_idle(शून्य);
शून्य get_cpu_ticks(अचिह्नित पूर्णाङ्क cpu, uint64_t ticks[MINIX_CPUSTATES]);
पूर्णाङ्क restore_fpu(संरचना proc *);
शून्य save_fpu(संरचना proc *);
शून्य save_local_fpu(संरचना proc *, पूर्णाङ्क retain);
शून्य fpu_sigcontext(संरचना proc *, संरचना sigframe_sigcontext *fr, संरचना
	sigcontext *sc);

/* main.c */
#यदि_अपरिभाषितम् UNPAGED
#परिभाषय kmain __k_unpaged_kmain
#यदि_अन्तः
शून्य kmain(kinfo_t *cbi);
शून्य prepare_shutdown(पूर्णाङ्क how);
__dead शून्य minix_shutdown(पूर्णाङ्क how);
शून्य bsp_finish_booting(शून्य);

/* proc.c */

पूर्णाङ्क do_ipc(reg_t r1, reg_t r2, reg_t r3);
शून्य proc_init(शून्य);
पूर्णाङ्क cancel_async(संरचना proc *src, संरचना proc *dst);
पूर्णाङ्क has_pending_notify(संरचना proc * caller, पूर्णाङ्क src_p);
पूर्णाङ्क has_pending_asend(संरचना proc * caller, पूर्णाङ्क src_p);
शून्य unset_notify_pending(संरचना proc * caller, पूर्णाङ्क src_p);
पूर्णाङ्क mini_notify(ध्रुव संरचना proc *src, endpoint_t dst);
शून्य vm_suspend(संरचना proc *caller, ध्रुव संरचना proc *target,
        ध्रुव vir_अष्टकानि linaddr, ध्रुव vir_अष्टकानि len, ध्रुव पूर्णाङ्क type,
        ध्रुव पूर्णाङ्क writeflag);
शून्य पङ्क्तौ_निक्षिप(संरचना proc *rp);
शून्य पङ्क्तेः_निष्कासय(संरचना proc *rp);
शून्य switch_गन्तव्यम्_user(शून्य);
शून्य arch_proc_reset(संरचना proc *rp);
शून्य arch_proc_setcontext(संरचना proc *rp, संरचना stackframe_s *state,
	पूर्णाङ्क user, पूर्णाङ्क restorestyle);
संरचना proc * arch_finish_switch_गन्तव्यम्_user(शून्य);
संरचना proc *endpoint_lookup(endpoint_t ep);
#यदि DEBUG_ENABLE_IPC_WARNINGS
पूर्णाङ्क isokendpt_f(ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, endpoint_t e, पूर्णाङ्क *p, पूर्णाङ्क
	f);
#परिभाषय isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#अन्यथा
पूर्णाङ्क isokendpt_f(endpoint_t e, पूर्णाङ्क *p, पूर्णाङ्क f);
#परिभाषय isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#यदि_अन्तः
शून्य proc_no_time(संरचना proc *p);
शून्य reset_proc_accounting(संरचना proc *p);
शून्य flag_account(संरचना proc *p, पूर्णाङ्क flag);
पूर्णाङ्क try_deliver_senda(संरचना proc *caller_ptr, asynmsg_t *सारणी, size_t
	आकार);

/* start.c */
शून्य cstart(शून्य);
वर्ण *env_get(ध्रुव वर्ण *कुञ्चिका);

/* system.c */
पूर्णाङ्क get_priv(पञ्जिका संरचना proc *rc, पूर्णाङ्क proc_type);
शून्य समुच्चयः_sendto_bit(ध्रुव संरचना proc *rc, पूर्णाङ्क id);
शून्य unset_sendto_bit(ध्रुव संरचना proc *rc, पूर्णाङ्क id);
शून्य fill_sendto_आवरणम्(ध्रुव संरचना proc *rc, sys_map_t *map);
पूर्णाङ्क send_sig(endpoint_t proc_nr, पूर्णाङ्क sig_nr);
शून्य cause_sig(proc_nr_t proc_nr, पूर्णाङ्क sig_nr);
शून्य sig_delay_समाप्तम्(संरचना proc *rp);
शून्य send_diag_sig(शून्य);
शून्य kernel_call(सन्देशः *m_user, संरचना proc * caller);
शून्य system_init(शून्य);
शून्य clear_endpoint(संरचना proc *rc);
शून्य clear_ipc_refs(संरचना proc *rc, पूर्णाङ्क caller_ret);
शून्य kernel_call_resume(संरचना proc *p);
पूर्णाङ्क sched_proc(संरचना proc *rp, पूर्णाङ्क प्राथमिकता, पूर्णाङ्क कालांशः, पूर्णाङ्क cpu, पूर्णाङ्क niced);
पूर्णाङ्क सङ्कलय_ipc_filter(संरचना proc *rp, पूर्णाङ्क type,
    vir_अष्टकानि स्थानाङ्कः, size_t दीर्घता);
शून्य clear_ipc_filters(संरचना proc *rp);
पूर्णाङ्क check_ipc_filter(संरचना ipc_filter_s *ipcf, पूर्णाङ्क fill_flags);
पूर्णाङ्क allow_ipc_filtered_सन्देशः(संरचना proc *rp, endpoint_t src_e,
    vir_अष्टकानि m_src_v, सन्देशः *m_src_p);
पूर्णाङ्क allow_ipc_filtered_memreq(संरचना proc *src_rp, संरचना proc *dst_rp);
पूर्णाङ्क priv_सङ्कलय_irq(संरचना proc *rp, पूर्णाङ्क irq);
पूर्णाङ्क priv_सङ्कलय_io(संरचना proc *rp, संरचना io_range *ior);
पूर्णाङ्क priv_सङ्कलय_mem(संरचना proc *rp, संरचना minix_mem_range *memr);

/* system/do_vtimer.c */
शून्य vtimer_check(संरचना proc *rp);

/* interrupt.c */
शून्य put_irq_handler(irq_hook_t *hook, पूर्णाङ्क irq, irq_handler_t handler);
शून्य rm_irq_handler(ध्रुव irq_hook_t *hook);
शून्य enable_irq(ध्रुव irq_hook_t *hook);
पूर्णाङ्क disable_irq(ध्रुव irq_hook_t *hook);

शून्य interrupts_enable(शून्य);
शून्य interrupts_disable(शून्य);

/* debug.c */
पूर्णाङ्क runqueues_ok(शून्य);
#यदि_अपरिभाषितम् CONFIG_SMP
#परिभाषय runqueues_ok_local runqueues_ok
#अन्यथा
#परिभाषय runqueues_ok_local() runqueues_ok_cpu(cpuid)
पूर्णाङ्क runqueues_ok_cpu(अचिह्नित cpu);
#यदि_अन्तः
वर्ण *rtsflagstr(u32_t flags);
वर्ण *miscflagstr(u32_t flags);
वर्ण *schedulerstr(संरचना proc *scheduler);
/* prints process information */
शून्य print_proc(संरचना proc *pp);
/* prints the given process and recursively all processes it depends on */
शून्य print_proc_recursive(संरचना proc *pp);
शून्य printmsg(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst,
    वर्ण operation, पूर्णाङ्क printparams);
#यदि DEBUG_IPC_HOOK
शून्य hook_ipc_msgrecv(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst);
शून्य hook_ipc_msgsend(सन्देशः *सन्देशः_2, संरचना proc *src, संरचना proc *dst);
शून्य hook_ipc_msgkcall(सन्देशः *सन्देशः_2, संरचना proc *proc);
शून्य hook_ipc_msgkresult(सन्देशः *सन्देशः_2, संरचना proc *proc);
शून्य hook_ipc_clear(संरचना proc *proc);
#यदि_अन्तः

/* system/do_safecopy.c */
संरचना cp_sfinfo; /* external callers may only provide NULL */
पूर्णाङ्क verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_अष्टकानि, पूर्णाङ्क,
	vir_अष्टकानि, vir_अष्टकानि *, endpoint_t *, संरचना cp_sfinfo *);

/* system/do_diagctl.c */
पूर्णाङ्क do_diagctl(संरचना proc * caller, सन्देशः *m);

#यदि SPROFILE
/* profile.c */
शून्य init_profile_clock(u32_t);
शून्य stop_profile_clock(शून्य);
#यदि_अन्तः

/* functions defined in architecture-dependent files. */
शून्य prot_init(शून्य);
शून्य arch_post_init(शून्य);
शून्य arch_समुच्चयः_secondary_ipc_return(संरचना proc *, u32_t val);
phys_अष्टकानि phys_copy(phys_अष्टकानि source, phys_अष्टकानि dest, phys_अष्टकानि
	गणनम्);
शून्य phys_copy_fault(शून्य);
शून्य phys_copy_fault_अन्तर्_kernel(शून्य);
शून्य memset_fault(शून्य);
शून्य memset_fault_अन्तर्_kernel(शून्य);
#परिभाषय virtual_copy(src, dst, अष्टकानि) \
				virtual_copy_f(रिक्त, src, dst, अष्टकानि, 0)
#परिभाषय virtual_copy_vmcheck(caller, src, dst, अष्टकानि) \
				virtual_copy_f(caller, src, dst, अष्टकानि, 1)
पूर्णाङ्क virtual_copy_f(संरचना proc * caller, संरचना vir_addr *src, संरचना
	vir_addr *dst, vir_अष्टकानि अष्टकानि, पूर्णाङ्क vmcheck);
पूर्णाङ्क दत्त_copy(endpoint_t from, vir_अष्टकानि from_addr, endpoint_t गन्तव्यम्,
	vir_अष्टकानि गन्तव्यम्_addr, size_t अष्टकानि);
पूर्णाङ्क दत्त_copy_vmcheck(संरचना proc *, endpoint_t from, vir_अष्टकानि
	from_addr, endpoint_t गन्तव्यम्, vir_अष्टकानि गन्तव्यम्_addr, size_t अष्टकानि);
phys_अष्टकानि umap_virtual(संरचना proc* rp, पूर्णाङ्क seg, vir_अष्टकानि vir_addr,
	vir_अष्टकानि अष्टकानि);
phys_अष्टकानि seg2phys(u16_t);
पूर्णाङ्क vm_memset(संरचना proc *caller, endpoint_t who, phys_अष्टकानि dst,
	पूर्णाङ्क आदर्शः, phys_अष्टकानि गणनम्);
पूर्णाङ्क intr_init(पूर्णाङ्क);
शून्य halt_cpu(शून्य);
शून्य arch_init(शून्य);
शून्य arch_boot_proc(संरचना boot_image *b, संरचना proc *p);
शून्य cpu_identify(शून्य);
/* arch dependent FPU initialization per CPU */
शून्य fpu_init(शून्य);
/* returns true if pfu is present and initialized */
पूर्णाङ्क is_fpu(शून्य);
शून्य ser_putc(वर्ण);
__dead शून्य arch_shutdown(पूर्णाङ्क);
शून्य restore_user_context(संरचना proc * p);
शून्य read_tsc(u32_t *उच्चः, u32_t *नीचः);
पूर्णाङ्क arch_init_profile_clock(u32_t freq);
शून्य arch_stop_profile_clock(शून्य);
शून्य arch_ack_profile_clock(शून्य);
शून्य do_ser_debug(शून्य);
पूर्णाङ्क arch_get_params(वर्ण *parm, पूर्णाङ्क max);
शून्य स्मृतिः_init(शून्य);
शून्य mem_clear_mapcache(शून्य);
शून्य arch_proc_init(संरचना proc *pr, u32_t, u32_t, u32_t, वर्ण *);
पूर्णाङ्क arch_do_vmctl(सन्देशः *m_ptr, संरचना proc *p);
पूर्णाङ्क vm_contiguous(ध्रुव संरचना proc *targetproc, vir_अष्टकानि vir_buf,
	size_t गणनम्);
शून्य proc_stacktrace(संरचना proc *proc);
पूर्णाङ्क vm_lookup(ध्रुव संरचना proc *proc, vir_अष्टकानि आभास, phys_अष्टकानि
	*परिणाम, u32_t *ptent);
size_t vm_lookup_range(ध्रुव संरचना proc *proc,
       vir_अष्टकानि vir_addr, phys_अष्टकानि *phys_addr, size_t अष्टकानि);
शून्य arch_do_syscall(संरचना proc *proc);
पूर्णाङ्क arch_phys_map(पूर्णाङ्क सूचकांक, phys_अष्टकानि *addr, phys_अष्टकानि *len, पूर्णाङ्क
	*flags);
पूर्णाङ्क arch_phys_map_प्रत्युत्तरम्(पूर्णाङ्क सूचकांक, vir_अष्टकानि addr);
reg_t arch_get_sp(संरचना proc *p);
पूर्णाङ्क arch_enable_paging(संरचना proc * caller);
पूर्णाङ्क vm_check_range(संरचना proc *caller,
       संरचना proc *target, vir_अष्टकानि vir_addr, size_t अष्टकानि, पूर्णाङ्क writable);

पूर्णाङ्क copy_सन्देशः_from_user(सन्देशः * user_mbuf, सन्देशः * dst);
पूर्णाङ्क copy_सन्देशः_गन्तव्यम्_user(सन्देशः * src, सन्देशः * user_mbuf);
शून्य switch_स्थानाङ्कः_space(संरचना proc * p);
शून्य release_स्थानाङ्कः_space(संरचना proc *pr);

शून्य enable_fpu_exception(शून्य);
शून्य disable_fpu_exception(शून्य);
शून्य release_fpu(संरचना proc * p);
शून्य arch_pause(शून्य);
ह्रस्व cpu_आरोपय(शून्य);
शून्य busy_delay_ms(पूर्णाङ्क ms);

/* utility.c */
शून्य cpu_print_freq(अचिह्नित cpu);
#यदि_अन्तः /* __kernel__ */

#यदि_अन्तः /* PROTO_H */
