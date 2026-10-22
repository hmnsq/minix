/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#جے_تعریف_شدہ __kernel__

#جے_بنا_تعریف PROTO_H
#تعریف_کرو PROTO_H

#ضمن <minix/safecopies.h>
#ضمن <machine/archtypes.h>
#ضمن <machine/signal.h>
#ضمن <machine/frame.h>

/* Struct declarations. */
بنية proc;
بنية ipc_filter_s;

/* clock.c */
فراغ init_clock(فراغ);
clock_t get_realtime(فراغ);
فراغ سیٹ_realtime(clock_t);
فراغ سیٹ_adjtime_delta(int32_t);
clock_t get_monotonic(فراغ);
فراغ سیٹ_boottime(time_t);
time_t get_boottime(فراغ);
فراغ سیٹ_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, صحيح arg);
فراغ reset_kernel_timer(minix_timer_t *tp);
فراغ ser_dump_proc(فراغ);

فراغ چکراں_accounting_init(فراغ);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
فراغ context_stop(بنية proc * p);
/* this is a wrapper to make calling it from assembly easier */
فراغ context_stop_idle(فراغ);
فراغ get_cpu_ticks(بنانشانی صحيح cpu, uint64_t ticks[MINIX_CPUSTATES]);
صحيح restore_fpu(بنية proc *);
فراغ save_fpu(بنية proc *);
فراغ save_local_fpu(بنية proc *, صحيح retain);
فراغ fpu_sigcontext(بنية proc *, بنية sigframe_sigcontext *fr, بنية
	sigcontext *sc);

/* main.c */
#جے_بنا_تعریف UNPAGED
#تعریف_کرو kmain __k_unpaged_kmain
#جے_انت
فراغ kmain(kinfo_t *cbi);
فراغ prepare_shutdown(صحيح how);
__dead فراغ minix_shutdown(صحيح how);
فراغ bsp_انت_booting(فراغ);

/* proc.c */

صحيح do_ipc(reg_t r1, reg_t r2, reg_t r3);
فراغ proc_init(فراغ);
صحيح cancel_async(بنية proc *src, بنية proc *dst);
صحيح has_pending_notify(بنية proc * caller, صحيح src_p);
صحيح has_pending_asend(بنية proc * caller, صحيح src_p);
فراغ unset_notify_pending(بنية proc * caller, صحيح src_p);
صحيح mini_notify(ثابت بنية proc *src, endpoint_t dst);
فراغ vm_suspend(بنية proc *caller, ثابت بنية proc *نشانہ,
        ثابت vir_بائٹاں linaddr, ثابت vir_بائٹاں len, ثابت صحيح type,
        ثابت صحيح writeflag);
فراغ قطار_وچ_پاؤ(بنية proc *rp);
فراغ قطار_توں_کڈھو(بنية proc *rp);
فراغ switch_منزل_user(فراغ);
فراغ arch_proc_reset(بنية proc *rp);
فراغ arch_proc_setcontext(بنية proc *rp, بنية stackframe_s *state,
	صحيح user, صحيح restorestyle);
بنية proc * arch_انت_switch_منزل_user(فراغ);
بنية proc *endpoint_lookup(endpoint_t ep);
#إذا DEBUG_ENABLE_IPC_WARNINGS
صحيح isokendpt_f(ثابت حرف *file, صحيح سطر, endpoint_t e, صحيح *p, صحيح
	f);
#تعریف_کرو isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#وإلا
صحيح isokendpt_f(endpoint_t e, صحيح *p, صحيح f);
#تعریف_کرو isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#جے_انت
فراغ proc_no_ویلا(بنية proc *p);
فراغ reset_proc_accounting(بنية proc *p);
فراغ flag_account(بنية proc *p, صحيح flag);
صحيح try_deliver_senda(بنية proc *caller_ptr, asynmsg_t *جدول, size_t
	ناپ);

/* start.c */
فراغ cstart(فراغ);
حرف *env_get(ثابت حرف *کلید);

/* system.c */
صحيح get_priv(سجل بنية proc *rc, صحيح proc_type);
فراغ سیٹ_sendto_ثنائی_ہندسہ(ثابت بنية proc *rc, صحيح id);
فراغ unset_sendto_ثنائی_ہندسہ(ثابت بنية proc *rc, صحيح id);
فراغ fill_sendto_نقاب(ثابت بنية proc *rc, sys_map_t *map);
صحيح send_sig(endpoint_t proc_nr, صحيح sig_nr);
فراغ cause_sig(proc_nr_t proc_nr, صحيح sig_nr);
فراغ sig_delay_پورا(بنية proc *rp);
فراغ send_diag_sig(فراغ);
فراغ kernel_call(سنیہا *m_user, بنية proc * caller);
فراغ system_init(فراغ);
فراغ clear_endpoint(بنية proc *rc);
فراغ clear_ipc_refs(بنية proc *rc, صحيح caller_ret);
فراغ kernel_call_resume(بنية proc *p);
صحيح sched_proc(بنية proc *rp, صحيح ترجیح, صحيح ویلا_حصہ, صحيح cpu, صحيح niced);
صحيح جوڑو_ipc_filter(بنية proc *rp, صحيح type,
    vir_بائٹاں پتہ, size_t لمائی);
فراغ clear_ipc_filters(بنية proc *rp);
صحيح check_ipc_filter(بنية ipc_filter_s *ipcf, صحيح fill_flags);
صحيح allow_ipc_filtered_پیغام(بنية proc *rp, endpoint_t src_e,
    vir_بائٹاں m_src_v, سنیہا *m_src_p);
صحيح allow_ipc_filtered_memreq(بنية proc *src_rp, بنية proc *dst_rp);
صحيح priv_جوڑو_irq(بنية proc *rp, صحيح irq);
صحيح priv_جوڑو_io(بنية proc *rp, بنية io_range *ior);
صحيح priv_جوڑو_mem(بنية proc *rp, بنية minix_mem_range *memr);

/* system/do_vtimer.c */
فراغ vtimer_check(بنية proc *rp);

/* interrupt.c */
فراغ put_irq_handler(irq_hook_t *hook, صحيح irq, irq_handler_t handler);
فراغ rm_irq_handler(ثابت irq_hook_t *hook);
فراغ enable_irq(ثابت irq_hook_t *hook);
صحيح disable_irq(ثابت irq_hook_t *hook);

فراغ interrupts_enable(فراغ);
فراغ interrupts_disable(فراغ);

/* debug.c */
صحيح runqueues_ok(فراغ);
#جے_بنا_تعریف CONFIG_SMP
#تعریف_کرو runqueues_ok_local runqueues_ok
#وإلا
#تعریف_کرو runqueues_ok_local() runqueues_ok_cpu(cpuid)
صحيح runqueues_ok_cpu(بنانشانی cpu);
#جے_انت
حرف *rtsflagstr(u32_t flags);
حرف *miscflagstr(u32_t flags);
حرف *schedulerstr(بنية proc *scheduler);
/* prints process information */
فراغ print_proc(بنية proc *pp);
/* prints the given process and recursively all processes it depends on */
فراغ print_proc_recursive(بنية proc *pp);
فراغ printmsg(سنیہا *پیغام, بنية proc *src, بنية proc *dst,
    حرف operation, صحيح printparams);
#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgrecv(سنیہا *پیغام, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgsend(سنیہا *پیغام, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgkcall(سنیہا *پیغام, بنية proc *proc);
فراغ hook_ipc_msgkresult(سنیہا *پیغام, بنية proc *proc);
فراغ hook_ipc_clear(بنية proc *proc);
#جے_انت

/* system/do_safecopy.c */
بنية cp_sfinfo; /* external callers may only provide NULL */
صحيح verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_بائٹاں, صحيح,
	vir_بائٹاں, vir_بائٹاں *, endpoint_t *, بنية cp_sfinfo *);

/* system/do_diagctl.c */
صحيح do_diagctl(بنية proc * caller, سنیہا *m);

#إذا SPROFILE
/* profile.c */
فراغ init_profile_clock(u32_t);
فراغ stop_profile_clock(فراغ);
#جے_انت

/* functions defined in architecture-dependent files. */
فراغ prot_init(فراغ);
فراغ arch_post_init(فراغ);
فراغ arch_سیٹ_secondary_ipc_return(بنية proc *, u32_t val);
phys_بائٹاں phys_copy(phys_بائٹاں سروت, phys_بائٹاں dest, phys_بائٹاں
	گنتی);
فراغ phys_copy_fault(فراغ);
فراغ phys_copy_fault_وچ_kernel(فراغ);
فراغ memset_fault(فراغ);
فراغ memset_fault_وچ_kernel(فراغ);
#تعریف_کرو virtual_copy(src, dst, بائٹاں) \
				virtual_copy_f(عدم, src, dst, بائٹاں, 0)
#تعریف_کرو virtual_copy_vmcheck(caller, src, dst, بائٹاں) \
				virtual_copy_f(caller, src, dst, بائٹاں, 1)
صحيح virtual_copy_f(بنية proc * caller, بنية vir_addr *src, بنية
	vir_addr *dst, vir_بائٹاں بائٹاں, صحيح vmcheck);
صحيح انکڑے_copy(endpoint_t from, vir_بائٹاں from_addr, endpoint_t منزل,
	vir_بائٹاں منزل_addr, size_t بائٹاں);
صحيح انکڑے_copy_vmcheck(بنية proc *, endpoint_t from, vir_بائٹاں
	from_addr, endpoint_t منزل, vir_بائٹاں منزل_addr, size_t بائٹاں);
phys_بائٹاں umap_virtual(بنية proc* rp, صحيح seg, vir_بائٹاں vir_addr,
	vir_بائٹاں بائٹاں);
phys_بائٹاں seg2phys(u16_t);
صحيح vm_memset(بنية proc *caller, endpoint_t who, phys_بائٹاں dst,
	صحيح نمونہ, phys_بائٹاں گنتی);
صحيح intr_init(صحيح);
فراغ halt_cpu(فراغ);
فراغ arch_init(فراغ);
فراغ arch_boot_proc(بنية boot_image *b, بنية proc *p);
فراغ cpu_identify(فراغ);
/* arch dependent FPU initialization per CPU */
فراغ fpu_init(فراغ);
/* returns true if pfu is present and initialized */
صحيح is_fpu(فراغ);
فراغ ser_putc(حرف);
__dead فراغ arch_shutdown(صحيح);
فراغ restore_user_context(بنية proc * p);
فراغ read_tsc(u32_t *بلند, u32_t *کم);
صحيح arch_init_profile_clock(u32_t freq);
فراغ arch_stop_profile_clock(فراغ);
فراغ arch_ack_profile_clock(فراغ);
فراغ do_ser_debug(فراغ);
صحيح arch_get_params(حرف *parm, صحيح max);
فراغ حافظہ_init(فراغ);
فراغ mem_clear_mapcache(فراغ);
فراغ arch_proc_init(بنية proc *pr, u32_t, u32_t, u32_t, حرف *);
صحيح arch_do_vmctl(سنیہا *m_ptr, بنية proc *p);
صحيح vm_contiguous(ثابت بنية proc *targetproc, vir_بائٹاں vir_buf,
	size_t گنتی);
فراغ proc_stacktrace(بنية proc *proc);
صحيح vm_lookup(ثابت بنية proc *proc, vir_بائٹاں مجازی, phys_بائٹاں
	*نتیجہ, u32_t *ptent);
size_t vm_lookup_range(ثابت بنية proc *proc,
       vir_بائٹاں vir_addr, phys_بائٹاں *phys_addr, size_t بائٹاں);
فراغ arch_do_syscall(بنية proc *proc);
صحيح arch_phys_map(صحيح اشاریہ, phys_بائٹاں *addr, phys_بائٹاں *len, صحيح
	*flags);
صحيح arch_phys_map_جواب(صحيح اشاریہ, vir_بائٹاں addr);
reg_t arch_get_sp(بنية proc *p);
صحيح arch_enable_paging(بنية proc * caller);
صحيح vm_check_range(بنية proc *caller,
       بنية proc *نشانہ, vir_بائٹاں vir_addr, size_t بائٹاں, صحيح writable);

صحيح copy_پیغام_from_user(سنیہا * user_mbuf, سنیہا * dst);
صحيح copy_پیغام_منزل_user(سنیہا * src, سنیہا * user_mbuf);
فراغ switch_پتہ_space(بنية proc * p);
فراغ release_پتہ_space(بنية proc *pr);

فراغ enable_fpu_exception(فراغ);
فراغ disable_fpu_exception(فراغ);
فراغ release_fpu(بنية proc * p);
فراغ arch_pause(فراغ);
چھوٹا cpu_لدو(فراغ);
فراغ busy_delay_ms(صحيح ms);

/* utility.c */
فراغ cpu_print_freq(بنانشانی cpu);
#جے_انت /* __kernel__ */

#جے_انت /* PROTO_H */
