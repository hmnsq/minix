/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __kernel__

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PROTO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROTO_H

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/safecopies.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/archtypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/frame.h>

/* Struct declarations. */
ਬਣਤਰ proc;
ਬਣਤਰ ipc_filter_s;

/* clock.c */
ਖਾਲੀ init_clock(ਖਾਲੀ);
clock_t get_realtime(ਖਾਲੀ);
ਖਾਲੀ ਸੈੱਟ_realtime(clock_t);
ਖਾਲੀ ਸੈੱਟ_adjtime_delta(int32_t);
clock_t get_monotonic(ਖਾਲੀ);
ਖਾਲੀ ਸੈੱਟ_boottime(time_t);
time_t get_boottime(ਖਾਲੀ);
ਖਾਲੀ ਸੈੱਟ_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, ਪੂਰਨ arg);
ਖਾਲੀ reset_kernel_timer(minix_timer_t *tp);
ਖਾਲੀ ser_dump_proc(ਖਾਲੀ);

ਖਾਲੀ ਚੱਕਰਾਂ_accounting_init(ਖਾਲੀ);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
ਖਾਲੀ context_stop(ਬਣਤਰ proc * p);
/* this is a wrapper to make calling it from assembly easier */
ਖਾਲੀ context_stop_idle(ਖਾਲੀ);
ਖਾਲੀ get_cpu_ticks(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpu, uint64_t ticks[MINIX_CPUSTATES]);
ਪੂਰਨ restore_fpu(ਬਣਤਰ proc *);
ਖਾਲੀ save_fpu(ਬਣਤਰ proc *);
ਖਾਲੀ save_local_fpu(ਬਣਤਰ proc *, ਪੂਰਨ retain);
ਖਾਲੀ fpu_sigcontext(ਬਣਤਰ proc *, ਬਣਤਰ sigframe_sigcontext *fr, ਬਣਤਰ
	sigcontext *sc);

/* main.c */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNPAGED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ kmain __k_unpaged_kmain
#ਜੇ_ਅੰਤ
ਖਾਲੀ kmain(kinfo_t *cbi);
ਖਾਲੀ prepare_shutdown(ਪੂਰਨ how);
__dead ਖਾਲੀ minix_shutdown(ਪੂਰਨ how);
ਖਾਲੀ bsp_ਅੰਤ_booting(ਖਾਲੀ);

/* proc.c */

ਪੂਰਨ do_ipc(reg_t r1, reg_t r2, reg_t r3);
ਖਾਲੀ proc_init(ਖਾਲੀ);
ਪੂਰਨ cancel_async(ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst);
ਪੂਰਨ has_pending_notify(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p);
ਪੂਰਨ has_pending_asend(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p);
ਖਾਲੀ unset_notify_pending(ਬਣਤਰ proc * caller, ਪੂਰਨ src_p);
ਪੂਰਨ mini_notify(ਅਚਲ ਬਣਤਰ proc *src, endpoint_t dst);
ਖਾਲੀ vm_suspend(ਬਣਤਰ proc *caller, ਅਚਲ ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ,
        ਅਚਲ vir_ਬਾਈਟਾਂ linaddr, ਅਚਲ vir_ਬਾਈਟਾਂ len, ਅਚਲ ਪੂਰਨ type,
        ਅਚਲ ਪੂਰਨ writeflag);
ਖਾਲੀ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਬਣਤਰ proc *rp);
ਖਾਲੀ ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਬਣਤਰ proc *rp);
ਖਾਲੀ switch_ਮੰਜ਼ਿਲ_user(ਖਾਲੀ);
ਖਾਲੀ arch_proc_reset(ਬਣਤਰ proc *rp);
ਖਾਲੀ arch_proc_setcontext(ਬਣਤਰ proc *rp, ਬਣਤਰ stackframe_s *state,
	ਪੂਰਨ user, ਪੂਰਨ restorestyle);
ਬਣਤਰ proc * arch_ਅੰਤ_switch_ਮੰਜ਼ਿਲ_user(ਖਾਲੀ);
ਬਣਤਰ proc *endpoint_lookup(endpoint_t ep);
#ਜੇ DEBUG_ENABLE_IPC_WARNINGS
ਪੂਰਨ isokendpt_f(ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, endpoint_t e, ਪੂਰਨ *p, ਪੂਰਨ
	f);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ਨਹੀਂਤਾਂ
ਪੂਰਨ isokendpt_f(endpoint_t e, ਪੂਰਨ *p, ਪੂਰਨ f);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#ਜੇ_ਅੰਤ
ਖਾਲੀ proc_no_ਸਮਾਂ(ਬਣਤਰ proc *p);
ਖਾਲੀ reset_proc_accounting(ਬਣਤਰ proc *p);
ਖਾਲੀ flag_account(ਬਣਤਰ proc *p, ਪੂਰਨ flag);
ਪੂਰਨ try_deliver_senda(ਬਣਤਰ proc *caller_ptr, asynmsg_t *ਸਾਰਣੀ, size_t
	ਮਾਪ);

/* start.c */
ਖਾਲੀ cstart(ਖਾਲੀ);
ਅੱਖਰ *env_get(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ);

/* system.c */
ਪੂਰਨ get_priv(ਰਜਿਸਟਰ ਬਣਤਰ proc *rc, ਪੂਰਨ proc_type);
ਖਾਲੀ ਸੈੱਟ_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਣਤਰ proc *rc, ਪੂਰਨ id);
ਖਾਲੀ unset_sendto_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਣਤਰ proc *rc, ਪੂਰਨ id);
ਖਾਲੀ fill_sendto_ਨਕਾਬ(ਅਚਲ ਬਣਤਰ proc *rc, sys_map_t *map);
ਪੂਰਨ send_sig(endpoint_t proc_nr, ਪੂਰਨ sig_nr);
ਖਾਲੀ cause_sig(proc_nr_t proc_nr, ਪੂਰਨ sig_nr);
ਖਾਲੀ sig_delay_ਪੂਰਾ(ਬਣਤਰ proc *rp);
ਖਾਲੀ send_diag_sig(ਖਾਲੀ);
ਖਾਲੀ kernel_call(ਸੁਨੇਹਾ *m_user, ਬਣਤਰ proc * caller);
ਖਾਲੀ system_init(ਖਾਲੀ);
ਖਾਲੀ clear_endpoint(ਬਣਤਰ proc *rc);
ਖਾਲੀ clear_ipc_refs(ਬਣਤਰ proc *rc, ਪੂਰਨ caller_ret);
ਖਾਲੀ kernel_call_resume(ਬਣਤਰ proc *p);
ਪੂਰਨ sched_proc(ਬਣਤਰ proc *rp, ਪੂਰਨ ਤਰਜੀਹ, ਪੂਰਨ ਸਮਾਂ_ਹਿੱਸਾ, ਪੂਰਨ cpu, ਪੂਰਨ niced);
ਪੂਰਨ ਜੋੜੋ_ipc_filter(ਬਣਤਰ proc *rp, ਪੂਰਨ type,
    vir_ਬਾਈਟਾਂ ਪਤਾ, size_t ਲੰਮਾਈ);
ਖਾਲੀ clear_ipc_filters(ਬਣਤਰ proc *rp);
ਪੂਰਨ check_ipc_filter(ਬਣਤਰ ipc_filter_s *ipcf, ਪੂਰਨ fill_flags);
ਪੂਰਨ allow_ipc_filtered_ਸੁਨੇਹਾ(ਬਣਤਰ proc *rp, endpoint_t src_e,
    vir_ਬਾਈਟਾਂ m_src_v, ਸੁਨੇਹਾ *m_src_p);
ਪੂਰਨ allow_ipc_filtered_memreq(ਬਣਤਰ proc *src_rp, ਬਣਤਰ proc *dst_rp);
ਪੂਰਨ priv_ਜੋੜੋ_irq(ਬਣਤਰ proc *rp, ਪੂਰਨ irq);
ਪੂਰਨ priv_ਜੋੜੋ_io(ਬਣਤਰ proc *rp, ਬਣਤਰ io_range *ior);
ਪੂਰਨ priv_ਜੋੜੋ_mem(ਬਣਤਰ proc *rp, ਬਣਤਰ minix_mem_range *memr);

/* system/do_vtimer.c */
ਖਾਲੀ vtimer_check(ਬਣਤਰ proc *rp);

/* interrupt.c */
ਖਾਲੀ put_irq_handler(irq_hook_t *hook, ਪੂਰਨ irq, irq_handler_t handler);
ਖਾਲੀ rm_irq_handler(ਅਚਲ irq_hook_t *hook);
ਖਾਲੀ enable_irq(ਅਚਲ irq_hook_t *hook);
ਪੂਰਨ disable_irq(ਅਚਲ irq_hook_t *hook);

ਖਾਲੀ interrupts_enable(ਖਾਲੀ);
ਖਾਲੀ interrupts_disable(ਖਾਲੀ);

/* debug.c */
ਪੂਰਨ runqueues_ok(ਖਾਲੀ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ runqueues_ok_local runqueues_ok
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ runqueues_ok_local() runqueues_ok_cpu(cpuid)
ਪੂਰਨ runqueues_ok_cpu(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
#ਜੇ_ਅੰਤ
ਅੱਖਰ *rtsflagstr(u32_t flags);
ਅੱਖਰ *miscflagstr(u32_t flags);
ਅੱਖਰ *schedulerstr(ਬਣਤਰ proc *scheduler);
/* prints process information */
ਖਾਲੀ print_proc(ਬਣਤਰ proc *pp);
/* prints the given process and recursively all processes it depends on */
ਖਾਲੀ print_proc_recursive(ਬਣਤਰ proc *pp);
ਖਾਲੀ printmsg(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst,
    ਅੱਖਰ operation, ਪੂਰਨ printparams);
#ਜੇ DEBUG_IPC_HOOK
ਖਾਲੀ hook_ipc_msgrecv(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst);
ਖਾਲੀ hook_ipc_msgsend(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *src, ਬਣਤਰ proc *dst);
ਖਾਲੀ hook_ipc_msgkcall(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *proc);
ਖਾਲੀ hook_ipc_msgkresult(ਸੁਨੇਹਾ *ਸੁਨੇਹਾ_2, ਬਣਤਰ proc *proc);
ਖਾਲੀ hook_ipc_clear(ਬਣਤਰ proc *proc);
#ਜੇ_ਅੰਤ

/* system/do_safecopy.c */
ਬਣਤਰ cp_sfinfo; /* external callers may only provide NULL */
ਪੂਰਨ verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_ਬਾਈਟਾਂ, ਪੂਰਨ,
	vir_ਬਾਈਟਾਂ, vir_ਬਾਈਟਾਂ *, endpoint_t *, ਬਣਤਰ cp_sfinfo *);

/* system/do_diagctl.c */
ਪੂਰਨ do_diagctl(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m);

#ਜੇ SPROFILE
/* profile.c */
ਖਾਲੀ init_profile_clock(u32_t);
ਖਾਲੀ stop_profile_clock(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

/* functions defined in architecture-dependent files. */
ਖਾਲੀ prot_init(ਖਾਲੀ);
ਖਾਲੀ arch_post_init(ਖਾਲੀ);
ਖਾਲੀ arch_ਸੈੱਟ_secondary_ipc_return(ਬਣਤਰ proc *, u32_t val);
phys_ਬਾਈਟਾਂ phys_copy(phys_ਬਾਈਟਾਂ ਸਰੋਤ, phys_ਬਾਈਟਾਂ dest, phys_ਬਾਈਟਾਂ
	ਗਿਣਤੀ_2);
ਖਾਲੀ phys_copy_fault(ਖਾਲੀ);
ਖਾਲੀ phys_copy_fault_ਵਿੱਚ_kernel(ਖਾਲੀ);
ਖਾਲੀ memset_fault(ਖਾਲੀ);
ਖਾਲੀ memset_fault_ਵਿੱਚ_kernel(ਖਾਲੀ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ virtual_copy(src, dst, ਬਾਈਟਾਂ) \
				virtual_copy_f(ਨਿਰਾ, src, dst, ਬਾਈਟਾਂ, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ virtual_copy_vmcheck(caller, src, dst, ਬਾਈਟਾਂ) \
				virtual_copy_f(caller, src, dst, ਬਾਈਟਾਂ, 1)
ਪੂਰਨ virtual_copy_f(ਬਣਤਰ proc * caller, ਬਣਤਰ vir_addr *src, ਬਣਤਰ
	vir_addr *dst, vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ, ਪੂਰਨ vmcheck);
ਪੂਰਨ ਅੰਕੜੇ_copy(endpoint_t from, vir_ਬਾਈਟਾਂ from_addr, endpoint_t ਮੰਜ਼ਿਲ,
	vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr, size_t ਬਾਈਟਾਂ);
ਪੂਰਨ ਅੰਕੜੇ_copy_vmcheck(ਬਣਤਰ proc *, endpoint_t from, vir_ਬਾਈਟਾਂ
	from_addr, endpoint_t ਮੰਜ਼ਿਲ, vir_ਬਾਈਟਾਂ ਮੰਜ਼ਿਲ_addr, size_t ਬਾਈਟਾਂ);
phys_ਬਾਈਟਾਂ umap_virtual(ਬਣਤਰ proc* rp, ਪੂਰਨ seg, vir_ਬਾਈਟਾਂ vir_addr,
	vir_ਬਾਈਟਾਂ ਬਾਈਟਾਂ);
phys_ਬਾਈਟਾਂ seg2phys(u16_t);
ਪੂਰਨ vm_memset(ਬਣਤਰ proc *caller, endpoint_t who, phys_ਬਾਈਟਾਂ dst,
	ਪੂਰਨ ਨਮੂਨਾ, phys_ਬਾਈਟਾਂ ਗਿਣਤੀ_2);
ਪੂਰਨ intr_init(ਪੂਰਨ);
ਖਾਲੀ halt_cpu(ਖਾਲੀ);
ਖਾਲੀ arch_init(ਖਾਲੀ);
ਖਾਲੀ arch_boot_proc(ਬਣਤਰ boot_image *b, ਬਣਤਰ proc *p);
ਖਾਲੀ cpu_identify(ਖਾਲੀ);
/* arch dependent FPU initialization per CPU */
ਖਾਲੀ fpu_init(ਖਾਲੀ);
/* returns true if pfu is present and initialized */
ਪੂਰਨ is_fpu(ਖਾਲੀ);
ਖਾਲੀ ser_putc(ਅੱਖਰ);
__dead ਖਾਲੀ arch_shutdown(ਪੂਰਨ);
ਖਾਲੀ restore_user_context(ਬਣਤਰ proc * p);
ਖਾਲੀ read_tsc(u32_t *ਉੱਚਾ, u32_t *ਨੀਵਾਂ);
ਪੂਰਨ arch_init_profile_clock(u32_t freq);
ਖਾਲੀ arch_stop_profile_clock(ਖਾਲੀ);
ਖਾਲੀ arch_ack_profile_clock(ਖਾਲੀ);
ਖਾਲੀ do_ser_debug(ਖਾਲੀ);
ਪੂਰਨ arch_get_params(ਅੱਖਰ *parm, ਪੂਰਨ max);
ਖਾਲੀ ਸਮ੍ਰਿਤੀ_init(ਖਾਲੀ);
ਖਾਲੀ mem_clear_mapcache(ਖਾਲੀ);
ਖਾਲੀ arch_proc_init(ਬਣਤਰ proc *pr, u32_t, u32_t, u32_t, ਅੱਖਰ *);
ਪੂਰਨ arch_do_vmctl(ਸੁਨੇਹਾ *m_ptr, ਬਣਤਰ proc *p);
ਪੂਰਨ vm_contiguous(ਅਚਲ ਬਣਤਰ proc *targetproc, vir_ਬਾਈਟਾਂ vir_buf,
	size_t ਗਿਣਤੀ_2);
ਖਾਲੀ proc_stacktrace(ਬਣਤਰ proc *proc);
ਪੂਰਨ vm_lookup(ਅਚਲ ਬਣਤਰ proc *proc, vir_ਬਾਈਟਾਂ ਆਭਾਸੀ, phys_ਬਾਈਟਾਂ
	*ਨਤੀਜਾ, u32_t *ptent);
size_t vm_lookup_range(ਅਚਲ ਬਣਤਰ proc *proc,
       vir_ਬਾਈਟਾਂ vir_addr, phys_ਬਾਈਟਾਂ *phys_addr, size_t ਬਾਈਟਾਂ);
ਖਾਲੀ arch_do_syscall(ਬਣਤਰ proc *proc);
ਪੂਰਨ arch_phys_map(ਪੂਰਨ ਸੂਚਕਾਂਕ, phys_ਬਾਈਟਾਂ *addr, phys_ਬਾਈਟਾਂ *len, ਪੂਰਨ
	*flags);
ਪੂਰਨ arch_phys_map_ਜਵਾਬ(ਪੂਰਨ ਸੂਚਕਾਂਕ, vir_ਬਾਈਟਾਂ addr);
reg_t arch_get_sp(ਬਣਤਰ proc *p);
ਪੂਰਨ arch_enable_paging(ਬਣਤਰ proc * caller);
ਪੂਰਨ vm_check_range(ਬਣਤਰ proc *caller,
       ਬਣਤਰ proc *ਨਿਸ਼ਾਨਾ, vir_ਬਾਈਟਾਂ vir_addr, size_t ਬਾਈਟਾਂ, ਪੂਰਨ writable);

ਪੂਰਨ copy_ਸੁਨੇਹਾ_from_user(ਸੁਨੇਹਾ * user_mbuf, ਸੁਨੇਹਾ * dst);
ਪੂਰਨ copy_ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_user(ਸੁਨੇਹਾ * src, ਸੁਨੇਹਾ * user_mbuf);
ਖਾਲੀ switch_ਪਤਾ_space(ਬਣਤਰ proc * p);
ਖਾਲੀ release_ਪਤਾ_space(ਬਣਤਰ proc *pr);

ਖਾਲੀ enable_fpu_exception(ਖਾਲੀ);
ਖਾਲੀ disable_fpu_exception(ਖਾਲੀ);
ਖਾਲੀ release_fpu(ਬਣਤਰ proc * p);
ਖਾਲੀ arch_pause(ਖਾਲੀ);
ਛੋਟਾ cpu_ਲੱਦੋ(ਖਾਲੀ);
ਖਾਲੀ busy_delay_ms(ਪੂਰਨ ms);

/* utility.c */
ਖਾਲੀ cpu_print_freq(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
#ਜੇ_ਅੰਤ /* __kernel__ */

#ਜੇ_ਅੰਤ /* PROTO_H */
