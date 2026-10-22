/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#شامل <minix/safecopies.h>
#شامل <machine/archtypes.h>
#شامل <machine/signal.h>
#شامل <machine/frame.h>

/* Struct declarations. */
ساختار proc;
ساختار ipc_filter_s;

/* clock.c */
تهی init_clock(تهی);
clock_t get_realtime(تهی);
تهی set_realtime(clock_t);
تهی set_adjtime_delta(int32_t);
clock_t get_monotonic(تهی);
تهی set_boottime(time_t);
time_t get_boottime(تهی);
تهی set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, صحیح arg);
تهی reset_kernel_timer(minix_timer_t *tp);
تهی ser_dump_proc(تهی);

تهی cycles_accounting_init(تهی);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
تهی context_stop(ساختار proc * p);
/* this is a wrapper to make calling it from assembly easier */
تهی context_stop_idle(تهی);
تهی get_cpu_ticks(unsigned صحیح cpu, uint64_t ticks[MINIX_CPUSTATES]);
صحیح restore_fpu(ساختار proc *);
تهی save_fpu(ساختار proc *);
تهی save_local_fpu(ساختار proc *, صحیح retain);
تهی fpu_sigcontext(ساختار proc *, ساختار sigframe_sigcontext *fr, ساختار
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
تهی kmain(kinfo_t *cbi);
تهی prepare_shutdown(صحیح how);
__dead تهی minix_shutdown(صحیح how);
تهی bsp_finish_booting(تهی);

/* proc.c */

صحیح do_ipc(reg_t r1, reg_t r2, reg_t r3);
تهی proc_init(تهی);
صحیح cancel_async(ساختار proc *src, ساختار proc *dst);
صحیح has_pending_notify(ساختار proc * caller, صحیح src_p);
صحیح has_pending_asend(ساختار proc * caller, صحیح src_p);
تهی unset_notify_pending(ساختار proc * caller, صحیح src_p);
صحیح mini_notify(ثابت ساختار proc *src, endpoint_t dst);
تهی vm_suspend(ساختار proc *caller, ثابت ساختار proc *target,
        ثابت vir_bytes linaddr, ثابت vir_bytes len, ثابت صحیح type,
        ثابت صحیح writeflag);
تهی enqueue(ساختار proc *rp);
تهی dequeue(ساختار proc *rp);
تهی switch_to_user(تهی);
تهی arch_proc_reset(ساختار proc *rp);
تهی arch_proc_setcontext(ساختار proc *rp, ساختار stackframe_s *state,
	صحیح user, صحیح restorestyle);
ساختار proc * arch_finish_switch_to_user(تهی);
ساختار proc *endpoint_lookup(endpoint_t ep);
#اگر DEBUG_ENABLE_IPC_WARNINGS
صحیح isokendpt_f(ثابت نویسه *file, صحیح سطر, endpoint_t e, صحیح *p, صحیح
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#وگرنه
صحیح isokendpt_f(endpoint_t e, صحیح *p, صحیح f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
تهی proc_no_time(ساختار proc *p);
تهی reset_proc_accounting(ساختار proc *p);
تهی flag_account(ساختار proc *p, صحیح flag);
صحیح try_deliver_senda(ساختار proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
تهی cstart(تهی);
نویسه *env_get(ثابت نویسه *کلید);

/* system.c */
صحیح get_priv(ثبات ساختار proc *rc, صحیح proc_type);
تهی set_sendto_bit(ثابت ساختار proc *rc, صحیح id);
تهی unset_sendto_bit(ثابت ساختار proc *rc, صحیح id);
تهی fill_sendto_mask(ثابت ساختار proc *rc, sys_map_t *map);
صحیح send_sig(endpoint_t proc_nr, صحیح sig_nr);
تهی cause_sig(proc_nr_t proc_nr, صحیح sig_nr);
تهی sig_delay_done(ساختار proc *rp);
تهی send_diag_sig(تهی);
تهی kernel_call(message *m_user, ساختار proc * caller);
تهی system_init(تهی);
تهی clear_endpoint(ساختار proc *rc);
تهی clear_ipc_refs(ساختار proc *rc, صحیح caller_ret);
تهی kernel_call_resume(ساختار proc *p);
صحیح sched_proc(ساختار proc *rp, صحیح priority, صحیح quantum, صحیح cpu, صحیح niced);
صحیح add_ipc_filter(ساختار proc *rp, صحیح type,
    vir_bytes address, size_t length);
تهی clear_ipc_filters(ساختار proc *rp);
صحیح check_ipc_filter(ساختار ipc_filter_s *ipcf, صحیح fill_flags);
صحیح allow_ipc_filtered_پیام(ساختار proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
صحیح allow_ipc_filtered_memreq(ساختار proc *src_rp, ساختار proc *dst_rp);
صحیح priv_add_irq(ساختار proc *rp, صحیح irq);
صحیح priv_add_io(ساختار proc *rp, ساختار io_range *ior);
صحیح priv_add_mem(ساختار proc *rp, ساختار minix_mem_range *memr);

/* system/do_vtimer.c */
تهی vtimer_check(ساختار proc *rp);

/* interrupt.c */
تهی put_irq_handler(irq_hook_t *hook, صحیح irq, irq_handler_t handler);
تهی rm_irq_handler(ثابت irq_hook_t *hook);
تهی enable_irq(ثابت irq_hook_t *hook);
صحیح disable_irq(ثابت irq_hook_t *hook);

تهی interrupts_enable(تهی);
تهی interrupts_disable(تهی);

/* debug.c */
صحیح runqueues_ok(تهی);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#وگرنه
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
صحیح runqueues_ok_cpu(unsigned cpu);
#endif
نویسه *rtsflagstr(u32_t flags);
نویسه *miscflagstr(u32_t flags);
نویسه *schedulerstr(ساختار proc *scheduler);
/* prints process information */
تهی print_proc(ساختار proc *pp);
/* prints the given process and recursively all processes it depends on */
تهی print_proc_recursive(ساختار proc *pp);
تهی printmsg(message *پیام, ساختار proc *src, ساختار proc *dst,
    نویسه operation, صحیح printparams);
#اگر DEBUG_IPC_HOOK
تهی hook_ipc_msgrecv(message *پیام, ساختار proc *src, ساختار proc *dst);
تهی hook_ipc_msgsend(message *پیام, ساختار proc *src, ساختار proc *dst);
تهی hook_ipc_msgkcall(message *پیام, ساختار proc *proc);
تهی hook_ipc_msgkresult(message *پیام, ساختار proc *proc);
تهی hook_ipc_clear(ساختار proc *proc);
#endif

/* system/do_safecopy.c */
ساختار cp_sfinfo; /* external callers may only provide NULL */
صحیح verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, صحیح,
	vir_bytes, vir_bytes *, endpoint_t *, ساختار cp_sfinfo *);

/* system/do_diagctl.c */
صحیح do_diagctl(ساختار proc * caller, message *m);

#اگر SPROFILE
/* profile.c */
تهی init_profile_clock(u32_t);
تهی stop_profile_clock(تهی);
#endif

/* functions defined in architecture-dependent files. */
تهی prot_init(تهی);
تهی arch_post_init(تهی);
تهی arch_set_secondary_ipc_return(ساختار proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	شمار);
تهی phys_copy_fault(تهی);
تهی phys_copy_fault_in_kernel(تهی);
تهی memset_fault(تهی);
تهی memset_fault_in_kernel(تهی);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(پوچ, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
صحیح virtual_copy_f(ساختار proc * caller, ساختار vir_addr *src, ساختار
	vir_addr *dst, vir_bytes bytes, صحیح vmcheck);
صحیح data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
صحیح data_copy_vmcheck(ساختار proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(ساختار proc* rp, صحیح seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
صحیح vm_memset(ساختار proc *caller, endpoint_t who, phys_bytes dst,
	صحیح pattern, phys_bytes شمار);
صحیح intr_init(صحیح);
تهی halt_cpu(تهی);
تهی arch_init(تهی);
تهی arch_boot_proc(ساختار boot_image *b, ساختار proc *p);
تهی cpu_identify(تهی);
/* arch dependent FPU initialization per CPU */
تهی fpu_init(تهی);
/* returns true if pfu is present and initialized */
صحیح is_fpu(تهی);
تهی ser_putc(نویسه);
__dead تهی arch_shutdown(صحیح);
تهی restore_user_context(ساختار proc * p);
تهی read_tsc(u32_t *بالا, u32_t *کم);
صحیح arch_init_profile_clock(u32_t freq);
تهی arch_stop_profile_clock(تهی);
تهی arch_ack_profile_clock(تهی);
تهی do_ser_debug(تهی);
صحیح arch_get_params(نویسه *parm, صحیح max);
تهی memory_init(تهی);
تهی mem_clear_mapcache(تهی);
تهی arch_proc_init(ساختار proc *pr, u32_t, u32_t, u32_t, نویسه *);
صحیح arch_do_vmctl(message *m_ptr, ساختار proc *p);
صحیح vm_contiguous(ثابت ساختار proc *targetproc, vir_bytes vir_buf,
	size_t شمار);
تهی proc_stacktrace(ساختار proc *proc);
صحیح vm_lookup(ثابت ساختار proc *proc, vir_bytes مجازی, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(ثابت ساختار proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
تهی arch_do_syscall(ساختار proc *proc);
صحیح arch_phys_map(صحیح index, phys_bytes *addr, phys_bytes *len, صحیح
	*flags);
صحیح arch_phys_map_reply(صحیح index, vir_bytes addr);
reg_t arch_get_sp(ساختار proc *p);
صحیح arch_enable_paging(ساختار proc * caller);
صحیح vm_check_range(ساختار proc *caller,
       ساختار proc *target, vir_bytes vir_addr, size_t bytes, صحیح writable);

صحیح copy_پیام_from_user(message * user_mbuf, message * dst);
صحیح copy_پیام_to_user(message * src, message * user_mbuf);
تهی switch_address_space(ساختار proc * p);
تهی release_address_space(ساختار proc *pr);

تهی enable_fpu_exception(تهی);
تهی disable_fpu_exception(تهی);
تهی release_fpu(ساختار proc * p);
تهی arch_pause(تهی);
short cpu_load(تهی);
تهی busy_delay_ms(صحیح ms);

/* utility.c */
تهی cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
