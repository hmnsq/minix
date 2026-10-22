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
কাঠামো proc;
কাঠামো ipc_filter_s;

/* clock.c */
শূন্য init_clock(শূন্য);
clock_t get_realtime(শূন্য);
শূন্য set_realtime(clock_t);
শূন্য set_adjtime_delta(int32_t);
clock_t get_monotonic(শূন্য);
শূন্য set_boottime(time_t);
time_t get_boottime(শূন্য);
শূন্য set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, পূর্ণসংখ্যা arg);
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
শূন্য context_stop(কাঠামো proc * p);
/* this is a wrapper to make calling it from assembly easier */
শূন্য context_stop_idle(শূন্য);
শূন্য get_cpu_ticks(unsigned পূর্ণসংখ্যা cpu, uint64_t ticks[MINIX_CPUSTATES]);
পূর্ণসংখ্যা restore_fpu(কাঠামো proc *);
শূন্য save_fpu(কাঠামো proc *);
শূন্য save_local_fpu(কাঠামো proc *, পূর্ণসংখ্যা retain);
শূন্য fpu_sigcontext(কাঠামো proc *, কাঠামো sigframe_sigcontext *fr, কাঠামো
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
শূন্য kmain(kinfo_t *cbi);
শূন্য prepare_shutdown(পূর্ণসংখ্যা how);
__dead শূন্য minix_shutdown(পূর্ণসংখ্যা how);
শূন্য bsp_finish_booting(শূন্য);

/* proc.c */

পূর্ণসংখ্যা do_ipc(reg_t r1, reg_t r2, reg_t r3);
শূন্য proc_init(শূন্য);
পূর্ণসংখ্যা cancel_async(কাঠামো proc *src, কাঠামো proc *dst);
পূর্ণসংখ্যা has_pending_notify(কাঠামো proc * caller, পূর্ণসংখ্যা src_p);
পূর্ণসংখ্যা has_pending_asend(কাঠামো proc * caller, পূর্ণসংখ্যা src_p);
শূন্য unset_notify_pending(কাঠামো proc * caller, পূর্ণসংখ্যা src_p);
পূর্ণসংখ্যা mini_notify(ধ্রুবক কাঠামো proc *src, endpoint_t dst);
শূন্য vm_suspend(কাঠামো proc *caller, ধ্রুবক কাঠামো proc *target,
        ধ্রুবক vir_bytes linaddr, ধ্রুবক vir_bytes len, ধ্রুবক পূর্ণসংখ্যা type,
        ধ্রুবক পূর্ণসংখ্যা writeflag);
শূন্য enqueue(কাঠামো proc *rp);
শূন্য dequeue(কাঠামো proc *rp);
শূন্য switch_to_user(শূন্য);
শূন্য arch_proc_reset(কাঠামো proc *rp);
শূন্য arch_proc_setcontext(কাঠামো proc *rp, কাঠামো stackframe_s *state,
	পূর্ণসংখ্যা user, পূর্ণসংখ্যা restorestyle);
কাঠামো proc * arch_finish_switch_to_user(শূন্য);
কাঠামো proc *endpoint_lookup(endpoint_t ep);
#যদি DEBUG_ENABLE_IPC_WARNINGS
পূর্ণসংখ্যা isokendpt_f(ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, endpoint_t e, পূর্ণসংখ্যা *p, পূর্ণসংখ্যা
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#অন্যথা
পূর্ণসংখ্যা isokendpt_f(endpoint_t e, পূর্ণসংখ্যা *p, পূর্ণসংখ্যা f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
শূন্য proc_no_time(কাঠামো proc *p);
শূন্য reset_proc_accounting(কাঠামো proc *p);
শূন্য flag_account(কাঠামো proc *p, পূর্ণসংখ্যা flag);
পূর্ণসংখ্যা try_deliver_senda(কাঠামো proc *caller_ptr, asynmsg_t *table, size_t
	আকার_2);

/* start.c */
শূন্য cstart(শূন্য);
অক্ষর *env_get(ধ্রুবক অক্ষর *চাবি);

/* system.c */
পূর্ণসংখ্যা get_priv(নিবন্ধ কাঠামো proc *rc, পূর্ণসংখ্যা proc_type);
শূন্য set_sendto_bit(ধ্রুবক কাঠামো proc *rc, পূর্ণসংখ্যা id);
শূন্য unset_sendto_bit(ধ্রুবক কাঠামো proc *rc, পূর্ণসংখ্যা id);
শূন্য fill_sendto_mask(ধ্রুবক কাঠামো proc *rc, sys_map_t *map);
পূর্ণসংখ্যা send_sig(endpoint_t proc_nr, পূর্ণসংখ্যা sig_nr);
শূন্য cause_sig(proc_nr_t proc_nr, পূর্ণসংখ্যা sig_nr);
শূন্য sig_delay_done(কাঠামো proc *rp);
শূন্য send_diag_sig(শূন্য);
শূন্য kernel_call(message *m_user, কাঠামো proc * caller);
শূন্য system_init(শূন্য);
শূন্য clear_endpoint(কাঠামো proc *rc);
শূন্য clear_ipc_refs(কাঠামো proc *rc, পূর্ণসংখ্যা caller_ret);
শূন্য kernel_call_resume(কাঠামো proc *p);
পূর্ণসংখ্যা sched_proc(কাঠামো proc *rp, পূর্ণসংখ্যা priority, পূর্ণসংখ্যা quantum, পূর্ণসংখ্যা cpu, পূর্ণসংখ্যা niced);
পূর্ণসংখ্যা add_ipc_filter(কাঠামো proc *rp, পূর্ণসংখ্যা type,
    vir_bytes address, size_t দৈর্ঘ্য);
শূন্য clear_ipc_filters(কাঠামো proc *rp);
পূর্ণসংখ্যা check_ipc_filter(কাঠামো ipc_filter_s *ipcf, পূর্ণসংখ্যা fill_flags);
পূর্ণসংখ্যা allow_ipc_filtered_বার্তা(কাঠামো proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
পূর্ণসংখ্যা allow_ipc_filtered_memreq(কাঠামো proc *src_rp, কাঠামো proc *dst_rp);
পূর্ণসংখ্যা priv_add_irq(কাঠামো proc *rp, পূর্ণসংখ্যা irq);
পূর্ণসংখ্যা priv_add_io(কাঠামো proc *rp, কাঠামো io_range *ior);
পূর্ণসংখ্যা priv_add_mem(কাঠামো proc *rp, কাঠামো minix_mem_range *memr);

/* system/do_vtimer.c */
শূন্য vtimer_check(কাঠামো proc *rp);

/* interrupt.c */
শূন্য put_irq_handler(irq_hook_t *hook, পূর্ণসংখ্যা irq, irq_handler_t handler);
শূন্য rm_irq_handler(ধ্রুবক irq_hook_t *hook);
শূন্য enable_irq(ধ্রুবক irq_hook_t *hook);
পূর্ণসংখ্যা disable_irq(ধ্রুবক irq_hook_t *hook);

শূন্য interrupts_enable(শূন্য);
শূন্য interrupts_disable(শূন্য);

/* debug.c */
পূর্ণসংখ্যা runqueues_ok(শূন্য);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#অন্যথা
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
পূর্ণসংখ্যা runqueues_ok_cpu(unsigned cpu);
#endif
অক্ষর *rtsflagstr(u32_t flags);
অক্ষর *miscflagstr(u32_t flags);
অক্ষর *schedulerstr(কাঠামো proc *scheduler);
/* prints process information */
শূন্য print_proc(কাঠামো proc *pp);
/* prints the given process and recursively all processes it depends on */
শূন্য print_proc_recursive(কাঠামো proc *pp);
শূন্য printmsg(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst,
    অক্ষর operation, পূর্ণসংখ্যা printparams);
#যদি DEBUG_IPC_HOOK
শূন্য hook_ipc_msgrecv(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst);
শূন্য hook_ipc_msgsend(message *বার্তা, কাঠামো proc *src, কাঠামো proc *dst);
শূন্য hook_ipc_msgkcall(message *বার্তা, কাঠামো proc *proc);
শূন্য hook_ipc_msgkresult(message *বার্তা, কাঠামো proc *proc);
শূন্য hook_ipc_clear(কাঠামো proc *proc);
#endif

/* system/do_safecopy.c */
কাঠামো cp_sfinfo; /* external callers may only provide NULL */
পূর্ণসংখ্যা verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, পূর্ণসংখ্যা,
	vir_bytes, vir_bytes *, endpoint_t *, কাঠামো cp_sfinfo *);

/* system/do_diagctl.c */
পূর্ণসংখ্যা do_diagctl(কাঠামো proc * caller, message *m);

#যদি SPROFILE
/* profile.c */
শূন্য init_profile_clock(u32_t);
শূন্য stop_profile_clock(শূন্য);
#endif

/* functions defined in architecture-dependent files. */
শূন্য prot_init(শূন্য);
শূন্য arch_post_init(শূন্য);
শূন্য arch_set_secondary_ipc_return(কাঠামো proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	গুনতি);
শূন্য phys_copy_fault(শূন্য);
শূন্য phys_copy_fault_in_kernel(শূন্য);
শূন্য memset_fault(শূন্য);
শূন্য memset_fault_in_kernel(শূন্য);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(ফাঁকা, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
পূর্ণসংখ্যা virtual_copy_f(কাঠামো proc * caller, কাঠামো vir_addr *src, কাঠামো
	vir_addr *dst, vir_bytes bytes, পূর্ণসংখ্যা vmcheck);
পূর্ণসংখ্যা তথ্য_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
পূর্ণসংখ্যা তথ্য_copy_vmcheck(কাঠামো proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(কাঠামো proc* rp, পূর্ণসংখ্যা seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
পূর্ণসংখ্যা vm_memset(কাঠামো proc *caller, endpoint_t who, phys_bytes dst,
	পূর্ণসংখ্যা pattern, phys_bytes গুনতি);
পূর্ণসংখ্যা intr_init(পূর্ণসংখ্যা);
শূন্য halt_cpu(শূন্য);
শূন্য arch_init(শূন্য);
শূন্য arch_boot_proc(কাঠামো boot_image *b, কাঠামো proc *p);
শূন্য cpu_identify(শূন্য);
/* arch dependent FPU initialization per CPU */
শূন্য fpu_init(শূন্য);
/* returns true if pfu is present and initialized */
পূর্ণসংখ্যা is_fpu(শূন্য);
শূন্য ser_putc(অক্ষর);
__dead শূন্য arch_shutdown(পূর্ণসংখ্যা);
শূন্য restore_user_context(কাঠামো proc * p);
শূন্য read_tsc(u32_t *উপরের, u32_t *নিচের);
পূর্ণসংখ্যা arch_init_profile_clock(u32_t freq);
শূন্য arch_stop_profile_clock(শূন্য);
শূন্য arch_ack_profile_clock(শূন্য);
শূন্য do_ser_debug(শূন্য);
পূর্ণসংখ্যা arch_get_params(অক্ষর *parm, পূর্ণসংখ্যা max);
শূন্য memory_init(শূন্য);
শূন্য mem_clear_mapcache(শূন্য);
শূন্য arch_proc_init(কাঠামো proc *pr, u32_t, u32_t, u32_t, অক্ষর *);
পূর্ণসংখ্যা arch_do_vmctl(message *m_ptr, কাঠামো proc *p);
পূর্ণসংখ্যা vm_contiguous(ধ্রুবক কাঠামো proc *targetproc, vir_bytes vir_buf,
	size_t গুনতি);
শূন্য proc_stacktrace(কাঠামো proc *proc);
পূর্ণসংখ্যা vm_lookup(ধ্রুবক কাঠামো proc *proc, vir_bytes আভাসী, phys_bytes
	*ফলাফল, u32_t *ptent);
size_t vm_lookup_range(ধ্রুবক কাঠামো proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
শূন্য arch_do_syscall(কাঠামো proc *proc);
পূর্ণসংখ্যা arch_phys_map(পূর্ণসংখ্যা সূচক, phys_bytes *addr, phys_bytes *len, পূর্ণসংখ্যা
	*flags);
পূর্ণসংখ্যা arch_phys_map_reply(পূর্ণসংখ্যা সূচক, vir_bytes addr);
reg_t arch_get_sp(কাঠামো proc *p);
পূর্ণসংখ্যা arch_enable_paging(কাঠামো proc * caller);
পূর্ণসংখ্যা vm_check_range(কাঠামো proc *caller,
       কাঠামো proc *target, vir_bytes vir_addr, size_t bytes, পূর্ণসংখ্যা writable);

পূর্ণসংখ্যা copy_বার্তা_from_user(message * user_mbuf, message * dst);
পূর্ণসংখ্যা copy_বার্তা_to_user(message * src, message * user_mbuf);
শূন্য switch_address_space(কাঠামো proc * p);
শূন্য release_address_space(কাঠামো proc *pr);

শূন্য enable_fpu_exception(শূন্য);
শূন্য disable_fpu_exception(শূন্য);
শূন্য release_fpu(কাঠামো proc * p);
শূন্য arch_pause(শূন্য);
short cpu_load(শূন্য);
শূন্য busy_delay_ms(পূর্ণসংখ্যা ms);

/* utility.c */
শূন্য cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
