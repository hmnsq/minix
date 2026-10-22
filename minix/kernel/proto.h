/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#அந்தர்பாவ <minix/safecopies.h>
#அந்தர்பாவ <machine/archtypes.h>
#அந்தர்பாவ <machine/signal.h>
#அந்தர்பாவ <machine/frame.h>

/* Struct declarations. */
கட்டமைப்பு proc;
கட்டமைப்பு ipc_filter_s;

/* clock.c */
வெறுமை init_clock(வெறுமை);
clock_t get_realtime(வெறுமை);
வெறுமை set_realtime(clock_t);
வெறுமை set_adjtime_delta(int32_t);
clock_t get_monotonic(வெறுமை);
வெறுமை set_boottime(time_t);
time_t get_boottime(வெறுமை);
வெறுமை set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, முழு arg);
வெறுமை reset_kernel_timer(minix_timer_t *tp);
வெறுமை ser_dump_proc(வெறுமை);

வெறுமை cycles_accounting_init(வெறுமை);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
வெறுமை context_stop(கட்டமைப்பு proc * p);
/* this is a wrapper to make calling it from assembly easier */
வெறுமை context_stop_idle(வெறுமை);
வெறுமை get_cpu_ticks(unsigned முழு cpu, uint64_t ticks[MINIX_CPUSTATES]);
முழு restore_fpu(கட்டமைப்பு proc *);
வெறுமை save_fpu(கட்டமைப்பு proc *);
வெறுமை save_local_fpu(கட்டமைப்பு proc *, முழு retain);
வெறுமை fpu_sigcontext(கட்டமைப்பு proc *, கட்டமைப்பு sigframe_sigcontext *fr, கட்டமைப்பு
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
வெறுமை kmain(kinfo_t *cbi);
வெறுமை prepare_shutdown(முழு how);
__dead வெறுமை minix_shutdown(முழு how);
வெறுமை bsp_finish_booting(வெறுமை);

/* proc.c */

முழு do_ipc(reg_t r1, reg_t r2, reg_t r3);
வெறுமை proc_init(வெறுமை);
முழு cancel_async(கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst);
முழு has_pending_notify(கட்டமைப்பு proc * caller, முழு src_p);
முழு has_pending_asend(கட்டமைப்பு proc * caller, முழு src_p);
வெறுமை unset_notify_pending(கட்டமைப்பு proc * caller, முழு src_p);
முழு mini_notify(மாறிலி கட்டமைப்பு proc *src, endpoint_t dst);
வெறுமை vm_suspend(கட்டமைப்பு proc *caller, மாறிலி கட்டமைப்பு proc *target,
        மாறிலி vir_bytes linaddr, மாறிலி vir_bytes len, மாறிலி முழு type,
        மாறிலி முழு writeflag);
வெறுமை enqueue(கட்டமைப்பு proc *rp);
வெறுமை dequeue(கட்டமைப்பு proc *rp);
வெறுமை switch_to_user(வெறுமை);
வெறுமை arch_proc_reset(கட்டமைப்பு proc *rp);
வெறுமை arch_proc_setcontext(கட்டமைப்பு proc *rp, கட்டமைப்பு stackframe_s *state,
	முழு user, முழு restorestyle);
கட்டமைப்பு proc * arch_finish_switch_to_user(வெறுமை);
கட்டமைப்பு proc *endpoint_lookup(endpoint_t ep);
#எனில் DEBUG_ENABLE_IPC_WARNINGS
முழு isokendpt_f(மாறிலி எழுத்து *file, முழு வரி, endpoint_t e, முழு *p, முழு
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#இல்லையெனில்
முழு isokendpt_f(endpoint_t e, முழு *p, முழு f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
வெறுமை proc_no_time(கட்டமைப்பு proc *p);
வெறுமை reset_proc_accounting(கட்டமைப்பு proc *p);
வெறுமை flag_account(கட்டமைப்பு proc *p, முழு flag);
முழு try_deliver_senda(கட்டமைப்பு proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
வெறுமை cstart(வெறுமை);
எழுத்து *env_get(மாறிலி எழுத்து *சாவி);

/* system.c */
முழு get_priv(பதிவு கட்டமைப்பு proc *rc, முழு proc_type);
வெறுமை set_sendto_bit(மாறிலி கட்டமைப்பு proc *rc, முழு id);
வெறுமை unset_sendto_bit(மாறிலி கட்டமைப்பு proc *rc, முழு id);
வெறுமை fill_sendto_mask(மாறிலி கட்டமைப்பு proc *rc, sys_map_t *map);
முழு send_sig(endpoint_t proc_nr, முழு sig_nr);
வெறுமை cause_sig(proc_nr_t proc_nr, முழு sig_nr);
வெறுமை sig_delay_done(கட்டமைப்பு proc *rp);
வெறுமை send_diag_sig(வெறுமை);
வெறுமை kernel_call(message *m_user, கட்டமைப்பு proc * caller);
வெறுமை system_init(வெறுமை);
வெறுமை clear_endpoint(கட்டமைப்பு proc *rc);
வெறுமை clear_ipc_refs(கட்டமைப்பு proc *rc, முழு caller_ret);
வெறுமை kernel_call_resume(கட்டமைப்பு proc *p);
முழு sched_proc(கட்டமைப்பு proc *rp, முழு priority, முழு quantum, முழு cpu, முழு niced);
முழு add_ipc_filter(கட்டமைப்பு proc *rp, முழு type,
    vir_bytes address, size_t length);
வெறுமை clear_ipc_filters(கட்டமைப்பு proc *rp);
முழு check_ipc_filter(கட்டமைப்பு ipc_filter_s *ipcf, முழு fill_flags);
முழு allow_ipc_filtered_செய்தி(கட்டமைப்பு proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
முழு allow_ipc_filtered_memreq(கட்டமைப்பு proc *src_rp, கட்டமைப்பு proc *dst_rp);
முழு priv_add_irq(கட்டமைப்பு proc *rp, முழு irq);
முழு priv_add_io(கட்டமைப்பு proc *rp, கட்டமைப்பு io_range *ior);
முழு priv_add_mem(கட்டமைப்பு proc *rp, கட்டமைப்பு minix_mem_range *memr);

/* system/do_vtimer.c */
வெறுமை vtimer_check(கட்டமைப்பு proc *rp);

/* interrupt.c */
வெறுமை put_irq_handler(irq_hook_t *hook, முழு irq, irq_handler_t handler);
வெறுமை rm_irq_handler(மாறிலி irq_hook_t *hook);
வெறுமை enable_irq(மாறிலி irq_hook_t *hook);
முழு disable_irq(மாறிலி irq_hook_t *hook);

வெறுமை interrupts_enable(வெறுமை);
வெறுமை interrupts_disable(வெறுமை);

/* debug.c */
முழு runqueues_ok(வெறுமை);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#இல்லையெனில்
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
முழு runqueues_ok_cpu(unsigned cpu);
#endif
எழுத்து *rtsflagstr(u32_t flags);
எழுத்து *miscflagstr(u32_t flags);
எழுத்து *schedulerstr(கட்டமைப்பு proc *scheduler);
/* prints process information */
வெறுமை print_proc(கட்டமைப்பு proc *pp);
/* prints the given process and recursively all processes it depends on */
வெறுமை print_proc_recursive(கட்டமைப்பு proc *pp);
வெறுமை printmsg(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst,
    எழுத்து operation, முழு printparams);
#எனில் DEBUG_IPC_HOOK
வெறுமை hook_ipc_msgrecv(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst);
வெறுமை hook_ipc_msgsend(message *செய்தி, கட்டமைப்பு proc *src, கட்டமைப்பு proc *dst);
வெறுமை hook_ipc_msgkcall(message *செய்தி, கட்டமைப்பு proc *proc);
வெறுமை hook_ipc_msgkresult(message *செய்தி, கட்டமைப்பு proc *proc);
வெறுமை hook_ipc_clear(கட்டமைப்பு proc *proc);
#endif

/* system/do_safecopy.c */
கட்டமைப்பு cp_sfinfo; /* external callers may only provide NULL */
முழு verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, முழு,
	vir_bytes, vir_bytes *, endpoint_t *, கட்டமைப்பு cp_sfinfo *);

/* system/do_diagctl.c */
முழு do_diagctl(கட்டமைப்பு proc * caller, message *m);

#எனில் SPROFILE
/* profile.c */
வெறுமை init_profile_clock(u32_t);
வெறுமை stop_profile_clock(வெறுமை);
#endif

/* functions defined in architecture-dependent files. */
வெறுமை prot_init(வெறுமை);
வெறுமை arch_post_init(வெறுமை);
வெறுமை arch_set_secondary_ipc_return(கட்டமைப்பு proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	மொத்தம்);
வெறுமை phys_copy_fault(வெறுமை);
வெறுமை phys_copy_fault_in_kernel(வெறுமை);
வெறுமை memset_fault(வெறுமை);
வெறுமை memset_fault_in_kernel(வெறுமை);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(வெற்று, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
முழு virtual_copy_f(கட்டமைப்பு proc * caller, கட்டமைப்பு vir_addr *src, கட்டமைப்பு
	vir_addr *dst, vir_bytes bytes, முழு vmcheck);
முழு data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
முழு data_copy_vmcheck(கட்டமைப்பு proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(கட்டமைப்பு proc* rp, முழு seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
முழு vm_memset(கட்டமைப்பு proc *caller, endpoint_t who, phys_bytes dst,
	முழு pattern, phys_bytes மொத்தம்);
முழு intr_init(முழு);
வெறுமை halt_cpu(வெறுமை);
வெறுமை arch_init(வெறுமை);
வெறுமை arch_boot_proc(கட்டமைப்பு boot_image *b, கட்டமைப்பு proc *p);
வெறுமை cpu_identify(வெறுமை);
/* arch dependent FPU initialization per CPU */
வெறுமை fpu_init(வெறுமை);
/* returns true if pfu is present and initialized */
முழு is_fpu(வெறுமை);
வெறுமை ser_putc(எழுத்து);
__dead வெறுமை arch_shutdown(முழு);
வெறுமை restore_user_context(கட்டமைப்பு proc * p);
வெறுமை read_tsc(u32_t *உயர்வு, u32_t *கீழ்);
முழு arch_init_profile_clock(u32_t freq);
வெறுமை arch_stop_profile_clock(வெறுமை);
வெறுமை arch_ack_profile_clock(வெறுமை);
வெறுமை do_ser_debug(வெறுமை);
முழு arch_get_params(எழுத்து *parm, முழு max);
வெறுமை memory_init(வெறுமை);
வெறுமை mem_clear_mapcache(வெறுமை);
வெறுமை arch_proc_init(கட்டமைப்பு proc *pr, u32_t, u32_t, u32_t, எழுத்து *);
முழு arch_do_vmctl(message *m_ptr, கட்டமைப்பு proc *p);
முழு vm_contiguous(மாறிலி கட்டமைப்பு proc *targetproc, vir_bytes vir_buf,
	size_t மொத்தம்);
வெறுமை proc_stacktrace(கட்டமைப்பு proc *proc);
முழு vm_lookup(மாறிலி கட்டமைப்பு proc *proc, vir_bytes மெய்நிகர், phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(மாறிலி கட்டமைப்பு proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
வெறுமை arch_do_syscall(கட்டமைப்பு proc *proc);
முழு arch_phys_map(முழு index, phys_bytes *addr, phys_bytes *len, முழு
	*flags);
முழு arch_phys_map_reply(முழு index, vir_bytes addr);
reg_t arch_get_sp(கட்டமைப்பு proc *p);
முழு arch_enable_paging(கட்டமைப்பு proc * caller);
முழு vm_check_range(கட்டமைப்பு proc *caller,
       கட்டமைப்பு proc *target, vir_bytes vir_addr, size_t bytes, முழு writable);

முழு copy_செய்தி_from_user(message * user_mbuf, message * dst);
முழு copy_செய்தி_to_user(message * src, message * user_mbuf);
வெறுமை switch_address_space(கட்டமைப்பு proc * p);
வெறுமை release_address_space(கட்டமைப்பு proc *pr);

வெறுமை enable_fpu_exception(வெறுமை);
வெறுமை disable_fpu_exception(வெறுமை);
வெறுமை release_fpu(கட்டமைப்பு proc * p);
வெறுமை arch_pause(வெறுமை);
short cpu_load(வெறுமை);
வெறுமை busy_delay_ms(முழு ms);

/* utility.c */
வெறுமை cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
