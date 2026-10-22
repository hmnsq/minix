/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#اگر_تعریف_شدہ __kernel__

#اگر_غیر_تعریف_شدہ PROTO_H
#تعریف_کرو PROTO_H

#شامل <minix/safecopies.h>
#شامل <machine/archtypes.h>
#شامل <machine/signal.h>
#شامل <machine/frame.h>

/* Struct declarations. */
ساخت proc;
ساخت ipc_filter_s;

/* clock.c */
خالی init_clock(خالی);
clock_t get_realtime(خالی);
خالی سیٹ_realtime(clock_t);
خالی سیٹ_adjtime_delta(int32_t);
clock_t get_monotonic(خالی);
خالی سیٹ_boottime(time_t);
time_t get_boottime(خالی);
خالی سیٹ_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, صحیح arg);
خالی reset_kernel_timer(minix_timer_t *tp);
خالی ser_dump_proc(خالی);

خالی ادوار_accounting_init(خالی);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
خالی context_stop(ساخت proc * p);
/* this is a wrapper to make calling it from assembly easier */
خالی context_stop_idle(خالی);
خالی get_cpu_ticks(غیرعلامتی صحیح cpu, uint64_t ticks[MINIX_CPUSTATES]);
صحیح restore_fpu(ساخت proc *);
خالی save_fpu(ساخت proc *);
خالی save_local_fpu(ساخت proc *, صحیح retain);
خالی fpu_sigcontext(ساخت proc *, ساخت sigframe_sigcontext *fr, ساخت
	sigcontext *sc);

/* main.c */
#اگر_غیر_تعریف_شدہ UNPAGED
#تعریف_کرو kmain __k_unpaged_kmain
#اگر_ختم
خالی kmain(kinfo_t *cbi);
خالی prepare_shutdown(صحیح how);
__dead خالی minix_shutdown(صحیح how);
خالی bsp_اختتام_booting(خالی);

/* proc.c */

صحیح do_ipc(reg_t r1, reg_t r2, reg_t r3);
خالی proc_init(خالی);
صحیح cancel_async(ساخت proc *src, ساخت proc *dst);
صحیح has_pending_notify(ساخت proc * caller, صحیح src_p);
صحیح has_pending_asend(ساخت proc * caller, صحیح src_p);
خالی unset_notify_pending(ساخت proc * caller, صحیح src_p);
صحیح mini_notify(ثابت ساخت proc *src, endpoint_t dst);
خالی vm_suspend(ساخت proc *caller, ثابت ساخت proc *ہدف,
        ثابت vir_بائٹیں linaddr, ثابت vir_بائٹیں len, ثابت صحیح type,
        ثابت صحیح writeflag);
خالی قطار_میں_ڈالو(ساخت proc *rp);
خالی قطار_سے_نکالو(ساخت proc *rp);
خالی switch_مقصد_user(خالی);
خالی arch_proc_reset(ساخت proc *rp);
خالی arch_proc_setcontext(ساخت proc *rp, ساخت stackframe_s *state,
	صحیح user, صحیح restorestyle);
ساخت proc * arch_اختتام_switch_مقصد_user(خالی);
ساخت proc *endpoint_lookup(endpoint_t ep);
#اگر DEBUG_ENABLE_IPC_WARNINGS
صحیح isokendpt_f(ثابت حرف *file, صحیح سطر, endpoint_t e, صحیح *p, صحیح
	f);
#تعریف_کرو isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ورنہ
صحیح isokendpt_f(endpoint_t e, صحیح *p, صحیح f);
#تعریف_کرو isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#اگر_ختم
خالی proc_no_وقت(ساخت proc *p);
خالی reset_proc_accounting(ساخت proc *p);
خالی flag_account(ساخت proc *p, صحیح flag);
صحیح try_deliver_senda(ساخت proc *caller_ptr, asynmsg_t *جدول, size_t
	سائز);

/* start.c */
خالی cstart(خالی);
حرف *env_get(ثابت حرف *کلید);

/* system.c */
صحیح get_priv(اندراج ساخت proc *rc, صحیح proc_type);
خالی سیٹ_sendto_ثنائی_ہندسہ(ثابت ساخت proc *rc, صحیح id);
خالی unset_sendto_ثنائی_ہندسہ(ثابت ساخت proc *rc, صحیح id);
خالی fill_sendto_نقاب(ثابت ساخت proc *rc, sys_map_t *map);
صحیح send_sig(endpoint_t proc_nr, صحیح sig_nr);
خالی cause_sig(proc_nr_t proc_nr, صحیح sig_nr);
خالی sig_delay_مکمل(ساخت proc *rp);
خالی send_diag_sig(خالی);
خالی kernel_call(پیغام *m_user, ساخت proc * caller);
خالی system_init(خالی);
خالی clear_endpoint(ساخت proc *rc);
خالی clear_ipc_refs(ساخت proc *rc, صحیح caller_ret);
خالی kernel_call_resume(ساخت proc *p);
صحیح sched_proc(ساخت proc *rp, صحیح ترجیح, صحیح وقتی_حصہ, صحیح cpu, صحیح niced);
صحیح جوڑو_ipc_filter(ساخت proc *rp, صحیح type,
    vir_بائٹیں پتہ, size_t لمبائی);
خالی clear_ipc_filters(ساخت proc *rp);
صحیح check_ipc_filter(ساخت ipc_filter_s *ipcf, صحیح fill_flags);
صحیح allow_ipc_filtered_پیغام(ساخت proc *rp, endpoint_t src_e,
    vir_بائٹیں m_src_v, پیغام *m_src_p);
صحیح allow_ipc_filtered_memreq(ساخت proc *src_rp, ساخت proc *dst_rp);
صحیح priv_جوڑو_irq(ساخت proc *rp, صحیح irq);
صحیح priv_جوڑو_io(ساخت proc *rp, ساخت io_range *ior);
صحیح priv_جوڑو_mem(ساخت proc *rp, ساخت minix_mem_range *memr);

/* system/do_vtimer.c */
خالی vtimer_check(ساخت proc *rp);

/* interrupt.c */
خالی put_irq_handler(irq_hook_t *hook, صحیح irq, irq_handler_t handler);
خالی rm_irq_handler(ثابت irq_hook_t *hook);
خالی enable_irq(ثابت irq_hook_t *hook);
صحیح disable_irq(ثابت irq_hook_t *hook);

خالی interrupts_enable(خالی);
خالی interrupts_disable(خالی);

/* debug.c */
صحیح runqueues_ok(خالی);
#اگر_غیر_تعریف_شدہ CONFIG_SMP
#تعریف_کرو runqueues_ok_local runqueues_ok
#ورنہ
#تعریف_کرو runqueues_ok_local() runqueues_ok_cpu(cpuid)
صحیح runqueues_ok_cpu(غیرعلامتی cpu);
#اگر_ختم
حرف *rtsflagstr(u32_t flags);
حرف *miscflagstr(u32_t flags);
حرف *schedulerstr(ساخت proc *scheduler);
/* prints process information */
خالی print_proc(ساخت proc *pp);
/* prints the given process and recursively all processes it depends on */
خالی print_proc_recursive(ساخت proc *pp);
خالی printmsg(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst,
    حرف operation, صحیح printparams);
#اگر DEBUG_IPC_HOOK
خالی hook_ipc_msgrecv(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst);
خالی hook_ipc_msgsend(پیغام *پیغام_2, ساخت proc *src, ساخت proc *dst);
خالی hook_ipc_msgkcall(پیغام *پیغام_2, ساخت proc *proc);
خالی hook_ipc_msgkresult(پیغام *پیغام_2, ساخت proc *proc);
خالی hook_ipc_clear(ساخت proc *proc);
#اگر_ختم

/* system/do_safecopy.c */
ساخت cp_sfinfo; /* external callers may only provide NULL */
صحیح verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_بائٹیں, صحیح,
	vir_بائٹیں, vir_بائٹیں *, endpoint_t *, ساخت cp_sfinfo *);

/* system/do_diagctl.c */
صحیح do_diagctl(ساخت proc * caller, پیغام *m);

#اگر SPROFILE
/* profile.c */
خالی init_profile_clock(u32_t);
خالی stop_profile_clock(خالی);
#اگر_ختم

/* functions defined in architecture-dependent files. */
خالی prot_init(خالی);
خالی arch_post_init(خالی);
خالی arch_سیٹ_secondary_ipc_return(ساخت proc *, u32_t val);
phys_بائٹیں phys_copy(phys_بائٹیں ماخذ, phys_بائٹیں dest, phys_بائٹیں
	گنتی);
خالی phys_copy_fault(خالی);
خالی phys_copy_fault_اندر_kernel(خالی);
خالی memset_fault(خالی);
خالی memset_fault_اندر_kernel(خالی);
#تعریف_کرو virtual_copy(src, dst, بائٹیں) \
				virtual_copy_f(تہی, src, dst, بائٹیں, 0)
#تعریف_کرو virtual_copy_vmcheck(caller, src, dst, بائٹیں) \
				virtual_copy_f(caller, src, dst, بائٹیں, 1)
صحیح virtual_copy_f(ساخت proc * caller, ساخت vir_addr *src, ساخت
	vir_addr *dst, vir_بائٹیں بائٹیں, صحیح vmcheck);
صحیح ڈیٹا_copy(endpoint_t from, vir_بائٹیں from_addr, endpoint_t مقصد,
	vir_بائٹیں مقصد_addr, size_t بائٹیں);
صحیح ڈیٹا_copy_vmcheck(ساخت proc *, endpoint_t from, vir_بائٹیں
	from_addr, endpoint_t مقصد, vir_بائٹیں مقصد_addr, size_t بائٹیں);
phys_بائٹیں umap_virtual(ساخت proc* rp, صحیح seg, vir_بائٹیں vir_addr,
	vir_بائٹیں بائٹیں);
phys_بائٹیں seg2phys(u16_t);
صحیح vm_memset(ساخت proc *caller, endpoint_t who, phys_بائٹیں dst,
	صحیح نمونہ, phys_بائٹیں گنتی);
صحیح intr_init(صحیح);
خالی halt_cpu(خالی);
خالی arch_init(خالی);
خالی arch_boot_proc(ساخت boot_image *b, ساخت proc *p);
خالی cpu_identify(خالی);
/* arch dependent FPU initialization per CPU */
خالی fpu_init(خالی);
/* returns true if pfu is present and initialized */
صحیح is_fpu(خالی);
خالی ser_putc(حرف);
__dead خالی arch_shutdown(صحیح);
خالی restore_user_context(ساخت proc * p);
خالی read_tsc(u32_t *بلند, u32_t *کم);
صحیح arch_init_profile_clock(u32_t freq);
خالی arch_stop_profile_clock(خالی);
خالی arch_ack_profile_clock(خالی);
خالی do_ser_debug(خالی);
صحیح arch_get_params(حرف *parm, صحیح max);
خالی حافظہ_init(خالی);
خالی mem_clear_mapcache(خالی);
خالی arch_proc_init(ساخت proc *pr, u32_t, u32_t, u32_t, حرف *);
صحیح arch_do_vmctl(پیغام *m_ptr, ساخت proc *p);
صحیح vm_contiguous(ثابت ساخت proc *targetproc, vir_بائٹیں vir_buf,
	size_t گنتی);
خالی proc_stacktrace(ساخت proc *proc);
صحیح vm_lookup(ثابت ساخت proc *proc, vir_بائٹیں مجازی, phys_بائٹیں
	*نتیجہ, u32_t *ptent);
size_t vm_lookup_range(ثابت ساخت proc *proc,
       vir_بائٹیں vir_addr, phys_بائٹیں *phys_addr, size_t بائٹیں);
خالی arch_do_syscall(ساخت proc *proc);
صحیح arch_phys_map(صحیح اشاریہ, phys_بائٹیں *addr, phys_بائٹیں *len, صحیح
	*flags);
صحیح arch_phys_map_جواب(صحیح اشاریہ, vir_بائٹیں addr);
reg_t arch_get_sp(ساخت proc *p);
صحیح arch_enable_paging(ساخت proc * caller);
صحیح vm_check_range(ساخت proc *caller,
       ساخت proc *ہدف, vir_بائٹیں vir_addr, size_t بائٹیں, صحیح writable);

صحیح copy_پیغام_from_user(پیغام * user_mbuf, پیغام * dst);
صحیح copy_پیغام_مقصد_user(پیغام * src, پیغام * user_mbuf);
خالی switch_پتہ_space(ساخت proc * p);
خالی release_پتہ_space(ساخت proc *pr);

خالی enable_fpu_exception(خالی);
خالی disable_fpu_exception(خالی);
خالی release_fpu(ساخت proc * p);
خالی arch_pause(خالی);
مختصر cpu_بھرو(خالی);
خالی busy_delay_ms(صحیح ms);

/* utility.c */
خالی cpu_print_freq(غیرعلامتی cpu);
#اگر_ختم /* __kernel__ */

#اگر_ختم /* PROTO_H */
