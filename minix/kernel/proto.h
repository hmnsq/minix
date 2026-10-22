/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#ಅನ್ತರ್ಭಾವ <minix/safecopies.h>
#ಅನ್ತರ್ಭಾವ <machine/archtypes.h>
#ಅನ್ತರ್ಭಾವ <machine/signal.h>
#ಅನ್ತರ್ಭಾವ <machine/frame.h>

/* Struct declarations. */
ಸಂರಚನಾ proc;
ಸಂರಚನಾ ipc_filter_s;

/* clock.c */
ಶೂನ್ಯ init_clock(ಶೂನ್ಯ);
clock_t get_realtime(ಶೂನ್ಯ);
ಶೂನ್ಯ set_realtime(clock_t);
ಶೂನ್ಯ set_adjtime_delta(int32_t);
clock_t get_monotonic(ಶೂನ್ಯ);
ಶೂನ್ಯ set_boottime(time_t);
time_t get_boottime(ಶೂನ್ಯ);
ಶೂನ್ಯ set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, ಪೂರ್ಣಾಙ್ಕ arg);
ಶೂನ್ಯ reset_kernel_timer(minix_timer_t *tp);
ಶೂನ್ಯ ser_dump_proc(ಶೂನ್ಯ);

ಶೂನ್ಯ cycles_accounting_init(ಶೂನ್ಯ);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
ಶೂನ್ಯ context_stop(ಸಂರಚನಾ proc * p);
/* this is a wrapper to make calling it from assembly easier */
ಶೂನ್ಯ context_stop_idle(ಶೂನ್ಯ);
ಶೂನ್ಯ get_cpu_ticks(unsigned ಪೂರ್ಣಾಙ್ಕ cpu, uint64_t ticks[MINIX_CPUSTATES]);
ಪೂರ್ಣಾಙ್ಕ restore_fpu(ಸಂರಚನಾ proc *);
ಶೂನ್ಯ save_fpu(ಸಂರಚನಾ proc *);
ಶೂನ್ಯ save_local_fpu(ಸಂರಚನಾ proc *, ಪೂರ್ಣಾಙ್ಕ retain);
ಶೂನ್ಯ fpu_sigcontext(ಸಂರಚನಾ proc *, ಸಂರಚನಾ sigframe_sigcontext *fr, ಸಂರಚನಾ
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
ಶೂನ್ಯ kmain(kinfo_t *cbi);
ಶೂನ್ಯ prepare_shutdown(ಪೂರ್ಣಾಙ್ಕ how);
__dead ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಙ್ಕ how);
ಶೂನ್ಯ bsp_finish_booting(ಶೂನ್ಯ);

/* proc.c */

ಪೂರ್ಣಾಙ್ಕ do_ipc(reg_t r1, reg_t r2, reg_t r3);
ಶೂನ್ಯ proc_init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ cancel_async(ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst);
ಪೂರ್ಣಾಙ್ಕ has_pending_notify(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p);
ಪೂರ್ಣಾಙ್ಕ has_pending_asend(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p);
ಶೂನ್ಯ unset_notify_pending(ಸಂರಚನಾ proc * caller, ಪೂರ್ಣಾಙ್ಕ src_p);
ಪೂರ್ಣಾಙ್ಕ mini_notify(ಧ್ರುವ ಸಂರಚನಾ proc *src, endpoint_t dst);
ಶೂನ್ಯ vm_suspend(ಸಂರಚನಾ proc *caller, ಧ್ರುವ ಸಂರಚನಾ proc *target,
        ಧ್ರುವ vir_bytes linaddr, ಧ್ರುವ vir_bytes len, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ type,
        ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ writeflag);
ಶೂನ್ಯ enqueue(ಸಂರಚನಾ proc *rp);
ಶೂನ್ಯ dequeue(ಸಂರಚನಾ proc *rp);
ಶೂನ್ಯ switch_to_user(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_proc_reset(ಸಂರಚನಾ proc *rp);
ಶೂನ್ಯ arch_proc_setcontext(ಸಂರಚನಾ proc *rp, ಸಂರಚನಾ stackframe_s *state,
	ಪೂರ್ಣಾಙ್ಕ user, ಪೂರ್ಣಾಙ್ಕ restorestyle);
ಸಂರಚನಾ proc * arch_finish_switch_to_user(ಶೂನ್ಯ);
ಸಂರಚನಾ proc *endpoint_lookup(endpoint_t ep);
#ಯದಿ DEBUG_ENABLE_IPC_WARNINGS
ಪೂರ್ಣಾಙ್ಕ isokendpt_f(ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, endpoint_t e, ಪೂರ್ಣಾಙ್ಕ *p, ಪೂರ್ಣಾಙ್ಕ
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ಅನ್ಯಥಾ
ಪೂರ್ಣಾಙ್ಕ isokendpt_f(endpoint_t e, ಪೂರ್ಣಾಙ್ಕ *p, ಪೂರ್ಣಾಙ್ಕ f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
ಶೂನ್ಯ proc_no_time(ಸಂರಚನಾ proc *p);
ಶೂನ್ಯ reset_proc_accounting(ಸಂರಚನಾ proc *p);
ಶೂನ್ಯ flag_account(ಸಂರಚನಾ proc *p, ಪೂರ್ಣಾಙ್ಕ flag);
ಪೂರ್ಣಾಙ್ಕ try_deliver_senda(ಸಂರಚನಾ proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
ಶೂನ್ಯ cstart(ಶೂನ್ಯ);
ವರ್ಣ *env_get(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ);

/* system.c */
ಪೂರ್ಣಾಙ್ಕ get_priv(ಪಞ್ಜಿಕಾ ಸಂರಚನಾ proc *rc, ಪೂರ್ಣಾಙ್ಕ proc_type);
ಶೂನ್ಯ set_sendto_bit(ಧ್ರುವ ಸಂರಚನಾ proc *rc, ಪೂರ್ಣಾಙ್ಕ id);
ಶೂನ್ಯ unset_sendto_bit(ಧ್ರುವ ಸಂರಚನಾ proc *rc, ಪೂರ್ಣಾಙ್ಕ id);
ಶೂನ್ಯ fill_sendto_mask(ಧ್ರುವ ಸಂರಚನಾ proc *rc, sys_map_t *map);
ಪೂರ್ಣಾಙ್ಕ send_sig(endpoint_t proc_nr, ಪೂರ್ಣಾಙ್ಕ sig_nr);
ಶೂನ್ಯ cause_sig(proc_nr_t proc_nr, ಪೂರ್ಣಾಙ್ಕ sig_nr);
ಶೂನ್ಯ sig_delay_done(ಸಂರಚನಾ proc *rp);
ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call(message *m_user, ಸಂರಚನಾ proc * caller);
ಶೂನ್ಯ system_init(ಶೂನ್ಯ);
ಶೂನ್ಯ clear_endpoint(ಸಂರಚನಾ proc *rc);
ಶೂನ್ಯ clear_ipc_refs(ಸಂರಚನಾ proc *rc, ಪೂರ್ಣಾಙ್ಕ caller_ret);
ಶೂನ್ಯ kernel_call_resume(ಸಂರಚನಾ proc *p);
ಪೂರ್ಣಾಙ್ಕ sched_proc(ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ priority, ಪೂರ್ಣಾಙ್ಕ quantum, ಪೂರ್ಣಾಙ್ಕ cpu, ಪೂರ್ಣಾಙ್ಕ niced);
ಪೂರ್ಣಾಙ್ಕ add_ipc_filter(ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ type,
    vir_bytes address, size_t length);
ಶೂನ್ಯ clear_ipc_filters(ಸಂರಚನಾ proc *rp);
ಪೂರ್ಣಾಙ್ಕ check_ipc_filter(ಸಂರಚನಾ ipc_filter_s *ipcf, ಪೂರ್ಣಾಙ್ಕ fill_flags);
ಪೂರ್ಣಾಙ್ಕ allow_ipc_filtered_ಸಂದೇಶ(ಸಂರಚನಾ proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
ಪೂರ್ಣಾಙ್ಕ allow_ipc_filtered_memreq(ಸಂರಚನಾ proc *src_rp, ಸಂರಚನಾ proc *dst_rp);
ಪೂರ್ಣಾಙ್ಕ priv_add_irq(ಸಂರಚನಾ proc *rp, ಪೂರ್ಣಾಙ್ಕ irq);
ಪೂರ್ಣಾಙ್ಕ priv_add_io(ಸಂರಚನಾ proc *rp, ಸಂರಚನಾ io_range *ior);
ಪೂರ್ಣಾಙ್ಕ priv_add_mem(ಸಂರಚನಾ proc *rp, ಸಂರಚನಾ minix_mem_range *memr);

/* system/do_vtimer.c */
ಶೂನ್ಯ vtimer_check(ಸಂರಚನಾ proc *rp);

/* interrupt.c */
ಶೂನ್ಯ put_irq_handler(irq_hook_t *hook, ಪೂರ್ಣಾಙ್ಕ irq, irq_handler_t handler);
ಶೂನ್ಯ rm_irq_handler(ಧ್ರುವ irq_hook_t *hook);
ಶೂನ್ಯ enable_irq(ಧ್ರುವ irq_hook_t *hook);
ಪೂರ್ಣಾಙ್ಕ disable_irq(ಧ್ರುವ irq_hook_t *hook);

ಶೂನ್ಯ interrupts_enable(ಶೂನ್ಯ);
ಶೂನ್ಯ interrupts_disable(ಶೂನ್ಯ);

/* debug.c */
ಪೂರ್ಣಾಙ್ಕ runqueues_ok(ಶೂನ್ಯ);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#ಅನ್ಯಥಾ
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
ಪೂರ್ಣಾಙ್ಕ runqueues_ok_cpu(unsigned cpu);
#endif
ವರ್ಣ *rtsflagstr(u32_t flags);
ವರ್ಣ *miscflagstr(u32_t flags);
ವರ್ಣ *schedulerstr(ಸಂರಚನಾ proc *scheduler);
/* prints process information */
ಶೂನ್ಯ print_proc(ಸಂರಚನಾ proc *pp);
/* prints the given process and recursively all processes it depends on */
ಶೂನ್ಯ print_proc_recursive(ಸಂರಚನಾ proc *pp);
ಶೂನ್ಯ printmsg(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst,
    ವರ್ಣ operation, ಪೂರ್ಣಾಙ್ಕ printparams);
#ಯದಿ DEBUG_IPC_HOOK
ಶೂನ್ಯ hook_ipc_msgrecv(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst);
ಶೂನ್ಯ hook_ipc_msgsend(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *src, ಸಂರಚನಾ proc *dst);
ಶೂನ್ಯ hook_ipc_msgkcall(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *proc);
ಶೂನ್ಯ hook_ipc_msgkresult(message *ಸಂದೇಶ, ಸಂರಚನಾ proc *proc);
ಶೂನ್ಯ hook_ipc_clear(ಸಂರಚನಾ proc *proc);
#endif

/* system/do_safecopy.c */
ಸಂರಚನಾ cp_sfinfo; /* external callers may only provide NULL */
ಪೂರ್ಣಾಙ್ಕ verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, ಪೂರ್ಣಾಙ್ಕ,
	vir_bytes, vir_bytes *, endpoint_t *, ಸಂರಚನಾ cp_sfinfo *);

/* system/do_diagctl.c */
ಪೂರ್ಣಾಙ್ಕ do_diagctl(ಸಂರಚನಾ proc * caller, message *m);

#ಯದಿ SPROFILE
/* profile.c */
ಶೂನ್ಯ init_profile_clock(u32_t);
ಶೂನ್ಯ stop_profile_clock(ಶೂನ್ಯ);
#endif

/* functions defined in architecture-dependent files. */
ಶೂನ್ಯ prot_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_post_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_set_secondary_ipc_return(ಸಂರಚನಾ proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	ಎಣಿಕೆ);
ಶೂನ್ಯ phys_copy_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ phys_copy_fault_in_kernel(ಶೂನ್ಯ);
ಶೂನ್ಯ memset_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ memset_fault_in_kernel(ಶೂನ್ಯ);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(ರಿಕ್ತ, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
ಪೂರ್ಣಾಙ್ಕ virtual_copy_f(ಸಂರಚನಾ proc * caller, ಸಂರಚನಾ vir_addr *src, ಸಂರಚನಾ
	vir_addr *dst, vir_bytes bytes, ಪೂರ್ಣಾಙ್ಕ vmcheck);
ಪೂರ್ಣಾಙ್ಕ data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
ಪೂರ್ಣಾಙ್ಕ data_copy_vmcheck(ಸಂರಚನಾ proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(ಸಂರಚನಾ proc* rp, ಪೂರ್ಣಾಙ್ಕ seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
ಪೂರ್ಣಾಙ್ಕ vm_memset(ಸಂರಚನಾ proc *caller, endpoint_t who, phys_bytes dst,
	ಪೂರ್ಣಾಙ್ಕ pattern, phys_bytes ಎಣಿಕೆ);
ಪೂರ್ಣಾಙ್ಕ intr_init(ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ halt_cpu(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_boot_proc(ಸಂರಚನಾ boot_image *b, ಸಂರಚನಾ proc *p);
ಶೂನ್ಯ cpu_identify(ಶೂನ್ಯ);
/* arch dependent FPU initialization per CPU */
ಶೂನ್ಯ fpu_init(ಶೂನ್ಯ);
/* returns true if pfu is present and initialized */
ಪೂರ್ಣಾಙ್ಕ is_fpu(ಶೂನ್ಯ);
ಶೂನ್ಯ ser_putc(ವರ್ಣ);
__dead ಶೂನ್ಯ arch_shutdown(ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ restore_user_context(ಸಂರಚನಾ proc * p);
ಶೂನ್ಯ read_tsc(u32_t *ಮೇಲಿನ, u32_t *ಕೆಳಗಿನ);
ಪೂರ್ಣಾಙ್ಕ arch_init_profile_clock(u32_t freq);
ಶೂನ್ಯ arch_stop_profile_clock(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_ack_profile_clock(ಶೂನ್ಯ);
ಶೂನ್ಯ do_ser_debug(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ arch_get_params(ವರ್ಣ *parm, ಪೂರ್ಣಾಙ್ಕ max);
ಶೂನ್ಯ memory_init(ಶೂನ್ಯ);
ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_proc_init(ಸಂರಚನಾ proc *pr, u32_t, u32_t, u32_t, ವರ್ಣ *);
ಪೂರ್ಣಾಙ್ಕ arch_do_vmctl(message *m_ptr, ಸಂರಚನಾ proc *p);
ಪೂರ್ಣಾಙ್ಕ vm_contiguous(ಧ್ರುವ ಸಂರಚನಾ proc *targetproc, vir_bytes vir_buf,
	size_t ಎಣಿಕೆ);
ಶೂನ್ಯ proc_stacktrace(ಸಂರಚನಾ proc *proc);
ಪೂರ್ಣಾಙ್ಕ vm_lookup(ಧ್ರುವ ಸಂರಚನಾ proc *proc, vir_bytes ಆಭಾಸ, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(ಧ್ರುವ ಸಂರಚನಾ proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
ಶೂನ್ಯ arch_do_syscall(ಸಂರಚನಾ proc *proc);
ಪೂರ್ಣಾಙ್ಕ arch_phys_map(ಪೂರ್ಣಾಙ್ಕ index, phys_bytes *addr, phys_bytes *len, ಪೂರ್ಣಾಙ್ಕ
	*flags);
ಪೂರ್ಣಾಙ್ಕ arch_phys_map_reply(ಪೂರ್ಣಾಙ್ಕ index, vir_bytes addr);
reg_t arch_get_sp(ಸಂರಚನಾ proc *p);
ಪೂರ್ಣಾಙ್ಕ arch_enable_paging(ಸಂರಚನಾ proc * caller);
ಪೂರ್ಣಾಙ್ಕ vm_check_range(ಸಂರಚನಾ proc *caller,
       ಸಂರಚನಾ proc *target, vir_bytes vir_addr, size_t bytes, ಪೂರ್ಣಾಙ್ಕ writable);

ಪೂರ್ಣಾಙ್ಕ copy_ಸಂದೇಶ_from_user(message * user_mbuf, message * dst);
ಪೂರ್ಣಾಙ್ಕ copy_ಸಂದೇಶ_to_user(message * src, message * user_mbuf);
ಶೂನ್ಯ switch_address_space(ಸಂರಚನಾ proc * p);
ಶೂನ್ಯ release_address_space(ಸಂರಚನಾ proc *pr);

ಶೂನ್ಯ enable_fpu_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ disable_fpu_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ release_fpu(ಸಂರಚನಾ proc * p);
ಶೂನ್ಯ arch_pause(ಶೂನ್ಯ);
short cpu_load(ಶೂನ್ಯ);
ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಙ್ಕ ms);

/* utility.c */
ಶೂನ್ಯ cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
