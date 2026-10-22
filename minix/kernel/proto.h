/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#અન્તર્ભાવ <minix/safecopies.h>
#અન્તર્ભાવ <machine/archtypes.h>
#અન્તર્ભાવ <machine/signal.h>
#અન્તર્ભાવ <machine/frame.h>

/* Struct declarations. */
સંરચના proc;
સંરચના ipc_filter_s;

/* clock.c */
શૂન્ય init_clock(શૂન્ય);
clock_t get_realtime(શૂન્ય);
શૂન્ય set_realtime(clock_t);
શૂન્ય set_adjtime_delta(int32_t);
clock_t get_monotonic(શૂન્ય);
શૂન્ય set_boottime(time_t);
time_t get_boottime(શૂન્ય);
શૂન્ય set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, પૂર્ણાંક arg);
શૂન્ય reset_kernel_timer(minix_timer_t *tp);
શૂન્ય ser_dump_proc(શૂન્ય);

શૂન્ય cycles_accounting_init(શૂન્ય);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
શૂન્ય context_stop(સંરચના proc * p);
/* this is a wrapper to make calling it from assembly easier */
શૂન્ય context_stop_idle(શૂન્ય);
શૂન્ય get_cpu_ticks(unsigned પૂર્ણાંક cpu, uint64_t ticks[MINIX_CPUSTATES]);
પૂર્ણાંક restore_fpu(સંરચના proc *);
શૂન્ય save_fpu(સંરચના proc *);
શૂન્ય save_local_fpu(સંરચના proc *, પૂર્ણાંક retain);
શૂન્ય fpu_sigcontext(સંરચના proc *, સંરચના sigframe_sigcontext *fr, સંરચના
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
શૂન્ય kmain(kinfo_t *cbi);
શૂન્ય prepare_shutdown(પૂર્ણાંક how);
__dead શૂન્ય minix_shutdown(પૂર્ણાંક how);
શૂન્ય bsp_finish_booting(શૂન્ય);

/* proc.c */

પૂર્ણાંક do_ipc(reg_t r1, reg_t r2, reg_t r3);
શૂન્ય proc_init(શૂન્ય);
પૂર્ણાંક cancel_async(સંરચના proc *src, સંરચના proc *dst);
પૂર્ણાંક has_pending_notify(સંરચના proc * caller, પૂર્ણાંક src_p);
પૂર્ણાંક has_pending_asend(સંરચના proc * caller, પૂર્ણાંક src_p);
શૂન્ય unset_notify_pending(સંરચના proc * caller, પૂર્ણાંક src_p);
પૂર્ણાંક mini_notify(અચલ સંરચના proc *src, endpoint_t dst);
શૂન્ય vm_suspend(સંરચના proc *caller, અચલ સંરચના proc *target,
        અચલ vir_bytes linaddr, અચલ vir_bytes len, અચલ પૂર્ણાંક type,
        અચલ પૂર્ણાંક writeflag);
શૂન્ય enqueue(સંરચના proc *rp);
શૂન્ય dequeue(સંરચના proc *rp);
શૂન્ય switch_to_user(શૂન્ય);
શૂન્ય arch_proc_reset(સંરચના proc *rp);
શૂન્ય arch_proc_setcontext(સંરચના proc *rp, સંરચના stackframe_s *state,
	પૂર્ણાંક user, પૂર્ણાંક restorestyle);
સંરચના proc * arch_finish_switch_to_user(શૂન્ય);
સંરચના proc *endpoint_lookup(endpoint_t ep);
#જો DEBUG_ENABLE_IPC_WARNINGS
પૂર્ણાંક isokendpt_f(અચલ અક્ષર *file, પૂર્ણાંક લીટી, endpoint_t e, પૂર્ણાંક *p, પૂર્ણાંક
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#નહીંતર
પૂર્ણાંક isokendpt_f(endpoint_t e, પૂર્ણાંક *p, પૂર્ણાંક f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
શૂન્ય proc_no_time(સંરચના proc *p);
શૂન્ય reset_proc_accounting(સંરચના proc *p);
શૂન્ય flag_account(સંરચના proc *p, પૂર્ણાંક flag);
પૂર્ણાંક try_deliver_senda(સંરચના proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
શૂન્ય cstart(શૂન્ય);
અક્ષર *env_get(અચલ અક્ષર *ચાવી);

/* system.c */
પૂર્ણાંક get_priv(નોંધણી સંરચના proc *rc, પૂર્ણાંક proc_type);
શૂન્ય set_sendto_bit(અચલ સંરચના proc *rc, પૂર્ણાંક id);
શૂન્ય unset_sendto_bit(અચલ સંરચના proc *rc, પૂર્ણાંક id);
શૂન્ય fill_sendto_mask(અચલ સંરચના proc *rc, sys_map_t *map);
પૂર્ણાંક send_sig(endpoint_t proc_nr, પૂર્ણાંક sig_nr);
શૂન્ય cause_sig(proc_nr_t proc_nr, પૂર્ણાંક sig_nr);
શૂન્ય sig_delay_done(સંરચના proc *rp);
શૂન્ય send_diag_sig(શૂન્ય);
શૂન્ય kernel_call(message *m_user, સંરચના proc * caller);
શૂન્ય system_init(શૂન્ય);
શૂન્ય clear_endpoint(સંરચના proc *rc);
શૂન્ય clear_ipc_refs(સંરચના proc *rc, પૂર્ણાંક caller_ret);
શૂન્ય kernel_call_resume(સંરચના proc *p);
પૂર્ણાંક sched_proc(સંરચના proc *rp, પૂર્ણાંક priority, પૂર્ણાંક quantum, પૂર્ણાંક cpu, પૂર્ણાંક niced);
પૂર્ણાંક add_ipc_filter(સંરચના proc *rp, પૂર્ણાંક type,
    vir_bytes address, size_t length);
શૂન્ય clear_ipc_filters(સંરચના proc *rp);
પૂર્ણાંક check_ipc_filter(સંરચના ipc_filter_s *ipcf, પૂર્ણાંક fill_flags);
પૂર્ણાંક allow_ipc_filtered_સંદેશ(સંરચના proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
પૂર્ણાંક allow_ipc_filtered_memreq(સંરચના proc *src_rp, સંરચના proc *dst_rp);
પૂર્ણાંક priv_add_irq(સંરચના proc *rp, પૂર્ણાંક irq);
પૂર્ણાંક priv_add_io(સંરચના proc *rp, સંરચના io_range *ior);
પૂર્ણાંક priv_add_mem(સંરચના proc *rp, સંરચના minix_mem_range *memr);

/* system/do_vtimer.c */
શૂન્ય vtimer_check(સંરચના proc *rp);

/* interrupt.c */
શૂન્ય put_irq_handler(irq_hook_t *hook, પૂર્ણાંક irq, irq_handler_t handler);
શૂન્ય rm_irq_handler(અચલ irq_hook_t *hook);
શૂન્ય enable_irq(અચલ irq_hook_t *hook);
પૂર્ણાંક disable_irq(અચલ irq_hook_t *hook);

શૂન્ય interrupts_enable(શૂન્ય);
શૂન્ય interrupts_disable(શૂન્ય);

/* debug.c */
પૂર્ણાંક runqueues_ok(શૂન્ય);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#નહીંતર
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
પૂર્ણાંક runqueues_ok_cpu(unsigned cpu);
#endif
અક્ષર *rtsflagstr(u32_t flags);
અક્ષર *miscflagstr(u32_t flags);
અક્ષર *schedulerstr(સંરચના proc *scheduler);
/* prints process information */
શૂન્ય print_proc(સંરચના proc *pp);
/* prints the given process and recursively all processes it depends on */
શૂન્ય print_proc_recursive(સંરચના proc *pp);
શૂન્ય printmsg(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst,
    અક્ષર operation, પૂર્ણાંક printparams);
#જો DEBUG_IPC_HOOK
શૂન્ય hook_ipc_msgrecv(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst);
શૂન્ય hook_ipc_msgsend(message *સંદેશ, સંરચના proc *src, સંરચના proc *dst);
શૂન્ય hook_ipc_msgkcall(message *સંદેશ, સંરચના proc *proc);
શૂન્ય hook_ipc_msgkresult(message *સંદેશ, સંરચના proc *proc);
શૂન્ય hook_ipc_clear(સંરચના proc *proc);
#endif

/* system/do_safecopy.c */
સંરચના cp_sfinfo; /* external callers may only provide NULL */
પૂર્ણાંક verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, પૂર્ણાંક,
	vir_bytes, vir_bytes *, endpoint_t *, સંરચના cp_sfinfo *);

/* system/do_diagctl.c */
પૂર્ણાંક do_diagctl(સંરચના proc * caller, message *m);

#જો SPROFILE
/* profile.c */
શૂન્ય init_profile_clock(u32_t);
શૂન્ય stop_profile_clock(શૂન્ય);
#endif

/* functions defined in architecture-dependent files. */
શૂન્ય prot_init(શૂન્ય);
શૂન્ય arch_post_init(શૂન્ય);
શૂન્ય arch_set_secondary_ipc_return(સંરચના proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	ગણતરી);
શૂન્ય phys_copy_fault(શૂન્ય);
શૂન્ય phys_copy_fault_in_kernel(શૂન્ય);
શૂન્ય memset_fault(શૂન્ય);
શૂન્ય memset_fault_in_kernel(શૂન્ય);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(ખાલી, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
પૂર્ણાંક virtual_copy_f(સંરચના proc * caller, સંરચના vir_addr *src, સંરચના
	vir_addr *dst, vir_bytes bytes, પૂર્ણાંક vmcheck);
પૂર્ણાંક data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
પૂર્ણાંક data_copy_vmcheck(સંરચના proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(સંરચના proc* rp, પૂર્ણાંક seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
પૂર્ણાંક vm_memset(સંરચના proc *caller, endpoint_t who, phys_bytes dst,
	પૂર્ણાંક pattern, phys_bytes ગણતરી);
પૂર્ણાંક intr_init(પૂર્ણાંક);
શૂન્ય halt_cpu(શૂન્ય);
શૂન્ય arch_init(શૂન્ય);
શૂન્ય arch_boot_proc(સંરચના boot_image *b, સંરચના proc *p);
શૂન્ય cpu_identify(શૂન્ય);
/* arch dependent FPU initialization per CPU */
શૂન્ય fpu_init(શૂન્ય);
/* returns true if pfu is present and initialized */
પૂર્ણાંક is_fpu(શૂન્ય);
શૂન્ય ser_putc(અક્ષર);
__dead શૂન્ય arch_shutdown(પૂર્ણાંક);
શૂન્ય restore_user_context(સંરચના proc * p);
શૂન્ય read_tsc(u32_t *ઉપલું, u32_t *નીચલું);
પૂર્ણાંક arch_init_profile_clock(u32_t freq);
શૂન્ય arch_stop_profile_clock(શૂન્ય);
શૂન્ય arch_ack_profile_clock(શૂન્ય);
શૂન્ય do_ser_debug(શૂન્ય);
પૂર્ણાંક arch_get_params(અક્ષર *parm, પૂર્ણાંક max);
શૂન્ય memory_init(શૂન્ય);
શૂન્ય mem_clear_mapcache(શૂન્ય);
શૂન્ય arch_proc_init(સંરચના proc *pr, u32_t, u32_t, u32_t, અક્ષર *);
પૂર્ણાંક arch_do_vmctl(message *m_ptr, સંરચના proc *p);
પૂર્ણાંક vm_contiguous(અચલ સંરચના proc *targetproc, vir_bytes vir_buf,
	size_t ગણતરી);
શૂન્ય proc_stacktrace(સંરચના proc *proc);
પૂર્ણાંક vm_lookup(અચલ સંરચના proc *proc, vir_bytes આભાસી, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(અચલ સંરચના proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
શૂન્ય arch_do_syscall(સંરચના proc *proc);
પૂર્ણાંક arch_phys_map(પૂર્ણાંક index, phys_bytes *addr, phys_bytes *len, પૂર્ણાંક
	*flags);
પૂર્ણાંક arch_phys_map_reply(પૂર્ણાંક index, vir_bytes addr);
reg_t arch_get_sp(સંરચના proc *p);
પૂર્ણાંક arch_enable_paging(સંરચના proc * caller);
પૂર્ણાંક vm_check_range(સંરચના proc *caller,
       સંરચના proc *target, vir_bytes vir_addr, size_t bytes, પૂર્ણાંક writable);

પૂર્ણાંક copy_સંદેશ_from_user(message * user_mbuf, message * dst);
પૂર્ણાંક copy_સંદેશ_to_user(message * src, message * user_mbuf);
શૂન્ય switch_address_space(સંરચના proc * p);
શૂન્ય release_address_space(સંરચના proc *pr);

શૂન્ય enable_fpu_exception(શૂન્ય);
શૂન્ય disable_fpu_exception(શૂન્ય);
શૂન્ય release_fpu(સંરચના proc * p);
શૂન્ય arch_pause(શૂન્ય);
short cpu_load(શૂન્ય);
શૂન્ય busy_delay_ms(પૂર્ણાંક ms);

/* utility.c */
શૂન્ય cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
