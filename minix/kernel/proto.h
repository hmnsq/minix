/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#অন্তর্ভাব <minix/safecopies.h>
#অন্তর্ভাব <machine/archtypes.h>
#অন্তর্ভাব <machine/signal.h>
#অন্তর্ভাব <machine/frame.h>

/* Struct declarations. */
সংরচনা proc;
সংরচনা ipc_filter_s;

/* clock.c */
শূন্য init_clock(শূন্য);
clock_t get_realtime(শূন্য);
শূন্য set_realtime(clock_t);
শূন্য set_adjtime_delta(int32_t);
clock_t get_monotonic(শূন্য);
শূন্য set_boottime(time_t);
time_t get_boottime(শূন্য);
শূন্য set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, পূর্ণাঙ্ক arg);
শূন্য reset_kernel_timer(minix_timer_t *tp);
শূন্য ser_dump_proc(শূন্য);

শূন্য cycles_accounting_init(শূন্য);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
শূন্য context_stop(সংরচনা proc * p);
/* this is a wrapper to make calling it from assembly easier */
শূন্য context_stop_idle(শূন্য);
শূন্য get_cpu_ticks(unsigned পূর্ণাঙ্ক cpu, uint64_t ticks[MINIX_CPUSTATES]);
পূর্ণাঙ্ক restore_fpu(সংরচনা proc *);
শূন্য save_fpu(সংরচনা proc *);
শূন্য save_local_fpu(সংরচনা proc *, পূর্ণাঙ্ক retain);
শূন্য fpu_sigcontext(সংরচনা proc *, সংরচনা sigframe_sigcontext *fr, সংরচনা
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
শূন্য kmain(kinfo_t *cbi);
শূন্য prepare_shutdown(পূর্ণাঙ্ক how);
__dead শূন্য minix_shutdown(পূর্ণাঙ্ক how);
শূন্য bsp_finish_booting(শূন্য);

/* proc.c */

পূর্ণাঙ্ক do_ipc(reg_t r1, reg_t r2, reg_t r3);
শূন্য proc_init(শূন্য);
পূর্ণাঙ্ক cancel_async(সংরচনা proc *src, সংরচনা proc *dst);
পূর্ণাঙ্ক has_pending_notify(সংরচনা proc * caller, পূর্ণাঙ্ক src_p);
পূর্ণাঙ্ক has_pending_asend(সংরচনা proc * caller, পূর্ণাঙ্ক src_p);
শূন্য unset_notify_pending(সংরচনা proc * caller, পূর্ণাঙ্ক src_p);
পূর্ণাঙ্ক mini_notify(ধ্রুব সংরচনা proc *src, endpoint_t dst);
শূন্য vm_suspend(সংরচনা proc *caller, ধ্রুব সংরচনা proc *target,
        ধ্রুব vir_bytes linaddr, ধ্রুব vir_bytes len, ধ্রুব পূর্ণাঙ্ক type,
        ধ্রুব পূর্ণাঙ্ক writeflag);
শূন্য enqueue(সংরচনা proc *rp);
শূন্য dequeue(সংরচনা proc *rp);
শূন্য switch_to_user(শূন্য);
শূন্য arch_proc_reset(সংরচনা proc *rp);
শূন্য arch_proc_setcontext(সংরচনা proc *rp, সংরচনা stackframe_s *state,
	পূর্ণাঙ্ক user, পূর্ণাঙ্ক restorestyle);
সংরচনা proc * arch_finish_switch_to_user(শূন্য);
সংরচনা proc *endpoint_lookup(endpoint_t ep);
#যদি DEBUG_ENABLE_IPC_WARNINGS
পূর্ণাঙ্ক isokendpt_f(ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি, endpoint_t e, পূর্ণাঙ্ক *p, পূর্ণাঙ্ক
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#অন্যথা
পূর্ণাঙ্ক isokendpt_f(endpoint_t e, পূর্ণাঙ্ক *p, পূর্ণাঙ্ক f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
শূন্য proc_no_time(সংরচনা proc *p);
শূন্য reset_proc_accounting(সংরচনা proc *p);
শূন্য flag_account(সংরচনা proc *p, পূর্ণাঙ্ক flag);
পূর্ণাঙ্ক try_deliver_senda(সংরচনা proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
শূন্য cstart(শূন্য);
বর্ণ *env_get(ধ্রুব বর্ণ *চাবি);

/* system.c */
পূর্ণাঙ্ক get_priv(পঞ্জিকা সংরচনা proc *rc, পূর্ণাঙ্ক proc_type);
শূন্য set_sendto_bit(ধ্রুব সংরচনা proc *rc, পূর্ণাঙ্ক id);
শূন্য unset_sendto_bit(ধ্রুব সংরচনা proc *rc, পূর্ণাঙ্ক id);
শূন্য fill_sendto_mask(ধ্রুব সংরচনা proc *rc, sys_map_t *map);
পূর্ণাঙ্ক send_sig(endpoint_t proc_nr, পূর্ণাঙ্ক sig_nr);
শূন্য cause_sig(proc_nr_t proc_nr, পূর্ণাঙ্ক sig_nr);
শূন্য sig_delay_done(সংরচনা proc *rp);
শূন্য send_diag_sig(শূন্য);
শূন্য kernel_call(message *m_user, সংরচনা proc * caller);
শূন্য system_init(শূন্য);
শূন্য clear_endpoint(সংরচনা proc *rc);
শূন্য clear_ipc_refs(সংরচনা proc *rc, পূর্ণাঙ্ক caller_ret);
শূন্য kernel_call_resume(সংরচনা proc *p);
পূর্ণাঙ্ক sched_proc(সংরচনা proc *rp, পূর্ণাঙ্ক priority, পূর্ণাঙ্ক quantum, পূর্ণাঙ্ক cpu, পূর্ণাঙ্ক niced);
পূর্ণাঙ্ক add_ipc_filter(সংরচনা proc *rp, পূর্ণাঙ্ক type,
    vir_bytes address, size_t length);
শূন্য clear_ipc_filters(সংরচনা proc *rp);
পূর্ণাঙ্ক check_ipc_filter(সংরচনা ipc_filter_s *ipcf, পূর্ণাঙ্ক fill_flags);
পূর্ণাঙ্ক allow_ipc_filtered_বার্তা(সংরচনা proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
পূর্ণাঙ্ক allow_ipc_filtered_memreq(সংরচনা proc *src_rp, সংরচনা proc *dst_rp);
পূর্ণাঙ্ক priv_add_irq(সংরচনা proc *rp, পূর্ণাঙ্ক irq);
পূর্ণাঙ্ক priv_add_io(সংরচনা proc *rp, সংরচনা io_range *ior);
পূর্ণাঙ্ক priv_add_mem(সংরচনা proc *rp, সংরচনা minix_mem_range *memr);

/* system/do_vtimer.c */
শূন্য vtimer_check(সংরচনা proc *rp);

/* interrupt.c */
শূন্য put_irq_handler(irq_hook_t *hook, পূর্ণাঙ্ক irq, irq_handler_t handler);
শূন্য rm_irq_handler(ধ্রুব irq_hook_t *hook);
শূন্য enable_irq(ধ্রুব irq_hook_t *hook);
পূর্ণাঙ্ক disable_irq(ধ্রুব irq_hook_t *hook);

শূন্য interrupts_enable(শূন্য);
শূন্য interrupts_disable(শূন্য);

/* debug.c */
পূর্ণাঙ্ক runqueues_ok(শূন্য);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#অন্যথা
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
পূর্ণাঙ্ক runqueues_ok_cpu(unsigned cpu);
#endif
বর্ণ *rtsflagstr(u32_t flags);
বর্ণ *miscflagstr(u32_t flags);
বর্ণ *schedulerstr(সংরচনা proc *scheduler);
/* prints process information */
শূন্য print_proc(সংরচনা proc *pp);
/* prints the given process and recursively all processes it depends on */
শূন্য print_proc_recursive(সংরচনা proc *pp);
শূন্য printmsg(message *বার্তা, সংরচনা proc *src, সংরচনা proc *dst,
    বর্ণ operation, পূর্ণাঙ্ক printparams);
#যদি DEBUG_IPC_HOOK
শূন্য hook_ipc_msgrecv(message *বার্তা, সংরচনা proc *src, সংরচনা proc *dst);
শূন্য hook_ipc_msgsend(message *বার্তা, সংরচনা proc *src, সংরচনা proc *dst);
শূন্য hook_ipc_msgkcall(message *বার্তা, সংরচনা proc *proc);
শূন্য hook_ipc_msgkresult(message *বার্তা, সংরচনা proc *proc);
শূন্য hook_ipc_clear(সংরচনা proc *proc);
#endif

/* system/do_safecopy.c */
সংরচনা cp_sfinfo; /* external callers may only provide NULL */
পূর্ণাঙ্ক verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, পূর্ণাঙ্ক,
	vir_bytes, vir_bytes *, endpoint_t *, সংরচনা cp_sfinfo *);

/* system/do_diagctl.c */
পূর্ণাঙ্ক do_diagctl(সংরচনা proc * caller, message *m);

#যদি SPROFILE
/* profile.c */
শূন্য init_profile_clock(u32_t);
শূন্য stop_profile_clock(শূন্য);
#endif

/* functions defined in architecture-dependent files. */
শূন্য prot_init(শূন্য);
শূন্য arch_post_init(শূন্য);
শূন্য arch_set_secondary_ipc_return(সংরচনা proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	গুনতি);
শূন্য phys_copy_fault(শূন্য);
শূন্য phys_copy_fault_in_kernel(শূন্য);
শূন্য memset_fault(শূন্য);
শূন্য memset_fault_in_kernel(শূন্য);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(রিক্ত, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
পূর্ণাঙ্ক virtual_copy_f(সংরচনা proc * caller, সংরচনা vir_addr *src, সংরচনা
	vir_addr *dst, vir_bytes bytes, পূর্ণাঙ্ক vmcheck);
পূর্ণাঙ্ক data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
পূর্ণাঙ্ক data_copy_vmcheck(সংরচনা proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(সংরচনা proc* rp, পূর্ণাঙ্ক seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
পূর্ণাঙ্ক vm_memset(সংরচনা proc *caller, endpoint_t who, phys_bytes dst,
	পূর্ণাঙ্ক pattern, phys_bytes গুনতি);
পূর্ণাঙ্ক intr_init(পূর্ণাঙ্ক);
শূন্য halt_cpu(শূন্য);
শূন্য arch_init(শূন্য);
শূন্য arch_boot_proc(সংরচনা boot_image *b, সংরচনা proc *p);
শূন্য cpu_identify(শূন্য);
/* arch dependent FPU initialization per CPU */
শূন্য fpu_init(শূন্য);
/* returns true if pfu is present and initialized */
পূর্ণাঙ্ক is_fpu(শূন্য);
শূন্য ser_putc(বর্ণ);
__dead শূন্য arch_shutdown(পূর্ণাঙ্ক);
শূন্য restore_user_context(সংরচনা proc * p);
শূন্য read_tsc(u32_t *উপরের, u32_t *নিচের);
পূর্ণাঙ্ক arch_init_profile_clock(u32_t freq);
শূন্য arch_stop_profile_clock(শূন্য);
শূন্য arch_ack_profile_clock(শূন্য);
শূন্য do_ser_debug(শূন্য);
পূর্ণাঙ্ক arch_get_params(বর্ণ *parm, পূর্ণাঙ্ক max);
শূন্য memory_init(শূন্য);
শূন্য mem_clear_mapcache(শূন্য);
শূন্য arch_proc_init(সংরচনা proc *pr, u32_t, u32_t, u32_t, বর্ণ *);
পূর্ণাঙ্ক arch_do_vmctl(message *m_ptr, সংরচনা proc *p);
পূর্ণাঙ্ক vm_contiguous(ধ্রুব সংরচনা proc *targetproc, vir_bytes vir_buf,
	size_t গুনতি);
শূন্য proc_stacktrace(সংরচনা proc *proc);
পূর্ণাঙ্ক vm_lookup(ধ্রুব সংরচনা proc *proc, vir_bytes আভাস, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(ধ্রুব সংরচনা proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
শূন্য arch_do_syscall(সংরচনা proc *proc);
পূর্ণাঙ্ক arch_phys_map(পূর্ণাঙ্ক index, phys_bytes *addr, phys_bytes *len, পূর্ণাঙ্ক
	*flags);
পূর্ণাঙ্ক arch_phys_map_reply(পূর্ণাঙ্ক index, vir_bytes addr);
reg_t arch_get_sp(সংরচনা proc *p);
পূর্ণাঙ্ক arch_enable_paging(সংরচনা proc * caller);
পূর্ণাঙ্ক vm_check_range(সংরচনা proc *caller,
       সংরচনা proc *target, vir_bytes vir_addr, size_t bytes, পূর্ণাঙ্ক writable);

পূর্ণাঙ্ক copy_বার্তা_from_user(message * user_mbuf, message * dst);
পূর্ণাঙ্ক copy_বার্তা_to_user(message * src, message * user_mbuf);
শূন্য switch_address_space(সংরচনা proc * p);
শূন্য release_address_space(সংরচনা proc *pr);

শূন্য enable_fpu_exception(শূন্য);
শূন্য disable_fpu_exception(শূন্য);
শূন্য release_fpu(সংরচনা proc * p);
শূন্য arch_pause(শূন্য);
short cpu_load(শূন্য);
শূন্য busy_delay_ms(পূর্ণাঙ্ক ms);

/* utility.c */
শূন্য cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
