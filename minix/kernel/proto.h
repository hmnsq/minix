/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#جيڪڏهن_وصف_ٿيل __kernel__

#جيڪڏهن_وصف_نه_ٿيل PROTO_H
#وصف_ڪريو PROTO_H

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
فراغ سيٽ_realtime(clock_t);
فراغ سيٽ_adjtime_delta(int32_t);
clock_t get_monotonic(فراغ);
فراغ سيٽ_boottime(time_t);
time_t get_boottime(فراغ);
فراغ سيٽ_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, صحيح arg);
فراغ reset_kernel_timer(minix_timer_t *tp);
فراغ ser_dump_proc(فراغ);

فراغ چڪرن_accounting_init(فراغ);
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
فراغ get_cpu_ticks(بنانشان صحيح cpu, uint64_t ticks[MINIX_CPUSTATES]);
صحيح restore_fpu(بنية proc *);
فراغ save_fpu(بنية proc *);
فراغ save_local_fpu(بنية proc *, صحيح retain);
فراغ fpu_sigcontext(بنية proc *, بنية sigframe_sigcontext *fr, بنية
	sigcontext *sc);

/* main.c */
#جيڪڏهن_وصف_نه_ٿيل UNPAGED
#وصف_ڪريو kmain __k_unpaged_kmain
#جيڪڏهن_پورو
فراغ kmain(kinfo_t *cbi);
فراغ prepare_shutdown(صحيح how);
__dead فراغ minix_shutdown(صحيح how);
فراغ bsp_پڄاڻي_booting(فراغ);

/* proc.c */

صحيح do_ipc(reg_t r1, reg_t r2, reg_t r3);
فراغ proc_init(فراغ);
صحيح cancel_async(بنية proc *src, بنية proc *dst);
صحيح has_pending_notify(بنية proc * caller, صحيح src_p);
صحيح has_pending_asend(بنية proc * caller, صحيح src_p);
فراغ unset_notify_pending(بنية proc * caller, صحيح src_p);
صحيح mini_notify(ثابت بنية proc *src, endpoint_t dst);
فراغ vm_suspend(بنية proc *caller, ثابت بنية proc *هدف,
        ثابت vir_بائيٽون linaddr, ثابت vir_بائيٽون len, ثابت صحيح type,
        ثابت صحيح writeflag);
فراغ قطار_اندر_وجهو(بنية proc *rp);
فراغ قطار_مان_ڪڍو(بنية proc *rp);
فراغ switch_منزل_user(فراغ);
فراغ arch_proc_reset(بنية proc *rp);
فراغ arch_proc_setcontext(بنية proc *rp, بنية stackframe_s *state,
	صحيح user, صحيح restorestyle);
بنية proc * arch_پڄاڻي_switch_منزل_user(فراغ);
بنية proc *endpoint_lookup(endpoint_t ep);
#إذا DEBUG_ENABLE_IPC_WARNINGS
صحيح isokendpt_f(ثابت حرف *file, صحيح سٽ, endpoint_t e, صحيح *p, صحيح
	f);
#وصف_ڪريو isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#وإلا
صحيح isokendpt_f(endpoint_t e, صحيح *p, صحيح f);
#وصف_ڪريو isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#جيڪڏهن_پورو
فراغ proc_no_وقت(بنية proc *p);
فراغ reset_proc_accounting(بنية proc *p);
فراغ flag_account(بنية proc *p, صحيح flag);
صحيح try_deliver_senda(بنية proc *caller_ptr, asynmsg_t *جدول, size_t
	سائيز);

/* start.c */
فراغ cstart(فراغ);
حرف *env_get(ثابت حرف *ڪنجي);

/* system.c */
صحيح get_priv(سجل بنية proc *rc, صحيح proc_type);
فراغ سيٽ_sendto_ٻٽو_انگ(ثابت بنية proc *rc, صحيح id);
فراغ unset_sendto_ٻٽو_انگ(ثابت بنية proc *rc, صحيح id);
فراغ fill_sendto_نقاب(ثابت بنية proc *rc, sys_map_t *map);
صحيح send_sig(endpoint_t proc_nr, صحيح sig_nr);
فراغ cause_sig(proc_nr_t proc_nr, صحيح sig_nr);
فراغ sig_delay_پورو(بنية proc *rp);
فراغ send_diag_sig(فراغ);
فراغ kernel_call(پيغام *m_user, بنية proc * caller);
فراغ system_init(فراغ);
فراغ clear_endpoint(بنية proc *rc);
فراغ clear_ipc_refs(بنية proc *rc, صحيح caller_ret);
فراغ kernel_call_resume(بنية proc *p);
صحيح sched_proc(بنية proc *rp, صحيح ترجيح, صحيح وقتي_حصو, صحيح cpu, صحيح niced);
صحيح جوڙيو_ipc_filter(بنية proc *rp, صحيح type,
    vir_بائيٽون پتو, size_t ڊيگهائي);
فراغ clear_ipc_filters(بنية proc *rp);
صحيح check_ipc_filter(بنية ipc_filter_s *ipcf, صحيح fill_flags);
صحيح allow_ipc_filtered_نياپو(بنية proc *rp, endpoint_t src_e,
    vir_بائيٽون m_src_v, پيغام *m_src_p);
صحيح allow_ipc_filtered_memreq(بنية proc *src_rp, بنية proc *dst_rp);
صحيح priv_جوڙيو_irq(بنية proc *rp, صحيح irq);
صحيح priv_جوڙيو_io(بنية proc *rp, بنية io_range *ior);
صحيح priv_جوڙيو_mem(بنية proc *rp, بنية minix_mem_range *memr);

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
#جيڪڏهن_وصف_نه_ٿيل CONFIG_SMP
#وصف_ڪريو runqueues_ok_local runqueues_ok
#وإلا
#وصف_ڪريو runqueues_ok_local() runqueues_ok_cpu(cpuid)
صحيح runqueues_ok_cpu(بنانشان cpu);
#جيڪڏهن_پورو
حرف *rtsflagstr(u32_t flags);
حرف *miscflagstr(u32_t flags);
حرف *schedulerstr(بنية proc *scheduler);
/* prints process information */
فراغ print_proc(بنية proc *pp);
/* prints the given process and recursively all processes it depends on */
فراغ print_proc_recursive(بنية proc *pp);
فراغ printmsg(پيغام *نياپو, بنية proc *src, بنية proc *dst,
    حرف operation, صحيح printparams);
#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgrecv(پيغام *نياپو, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgsend(پيغام *نياپو, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgkcall(پيغام *نياپو, بنية proc *proc);
فراغ hook_ipc_msgkresult(پيغام *نياپو, بنية proc *proc);
فراغ hook_ipc_clear(بنية proc *proc);
#جيڪڏهن_پورو

/* system/do_safecopy.c */
بنية cp_sfinfo; /* external callers may only provide NULL */
صحيح verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_بائيٽون, صحيح,
	vir_بائيٽون, vir_بائيٽون *, endpoint_t *, بنية cp_sfinfo *);

/* system/do_diagctl.c */
صحيح do_diagctl(بنية proc * caller, پيغام *m);

#إذا SPROFILE
/* profile.c */
فراغ init_profile_clock(u32_t);
فراغ stop_profile_clock(فراغ);
#جيڪڏهن_پورو

/* functions defined in architecture-dependent files. */
فراغ prot_init(فراغ);
فراغ arch_post_init(فراغ);
فراغ arch_سيٽ_secondary_ipc_return(بنية proc *, u32_t val);
phys_بائيٽون phys_copy(phys_بائيٽون ذريعو, phys_بائيٽون dest, phys_بائيٽون
	ڳڻپ);
فراغ phys_copy_fault(فراغ);
فراغ phys_copy_fault_اندريون_kernel(فراغ);
فراغ memset_fault(فراغ);
فراغ memset_fault_اندريون_kernel(فراغ);
#وصف_ڪريو virtual_copy(src, dst, بائيٽون) \
				virtual_copy_f(عدم, src, dst, بائيٽون, 0)
#وصف_ڪريو virtual_copy_vmcheck(caller, src, dst, بائيٽون) \
				virtual_copy_f(caller, src, dst, بائيٽون, 1)
صحيح virtual_copy_f(بنية proc * caller, بنية vir_addr *src, بنية
	vir_addr *dst, vir_بائيٽون بائيٽون, صحيح vmcheck);
صحيح معلومات_copy(endpoint_t from, vir_بائيٽون from_addr, endpoint_t منزل,
	vir_بائيٽون منزل_addr, size_t بائيٽون);
صحيح معلومات_copy_vmcheck(بنية proc *, endpoint_t from, vir_بائيٽون
	from_addr, endpoint_t منزل, vir_بائيٽون منزل_addr, size_t بائيٽون);
phys_بائيٽون umap_virtual(بنية proc* rp, صحيح seg, vir_بائيٽون vir_addr,
	vir_بائيٽون بائيٽون);
phys_بائيٽون seg2phys(u16_t);
صحيح vm_memset(بنية proc *caller, endpoint_t who, phys_بائيٽون dst,
	صحيح نمونو, phys_بائيٽون ڳڻپ);
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
فراغ read_tsc(u32_t *مٿاهون, u32_t *هيٺيون);
صحيح arch_init_profile_clock(u32_t freq);
فراغ arch_stop_profile_clock(فراغ);
فراغ arch_ack_profile_clock(فراغ);
فراغ do_ser_debug(فراغ);
صحيح arch_get_params(حرف *parm, صحيح max);
فراغ يادگيري_init(فراغ);
فراغ mem_clear_mapcache(فراغ);
فراغ arch_proc_init(بنية proc *pr, u32_t, u32_t, u32_t, حرف *);
صحيح arch_do_vmctl(پيغام *m_ptr, بنية proc *p);
صحيح vm_contiguous(ثابت بنية proc *targetproc, vir_بائيٽون vir_buf,
	size_t ڳڻپ);
فراغ proc_stacktrace(بنية proc *proc);
صحيح vm_lookup(ثابت بنية proc *proc, vir_بائيٽون مجازي, phys_بائيٽون
	*نتيجو, u32_t *ptent);
size_t vm_lookup_range(ثابت بنية proc *proc,
       vir_بائيٽون vir_addr, phys_بائيٽون *phys_addr, size_t بائيٽون);
فراغ arch_do_syscall(بنية proc *proc);
صحيح arch_phys_map(صحيح اشاريو, phys_بائيٽون *addr, phys_بائيٽون *len, صحيح
	*flags);
صحيح arch_phys_map_جواب(صحيح اشاريو, vir_بائيٽون addr);
reg_t arch_get_sp(بنية proc *p);
صحيح arch_enable_paging(بنية proc * caller);
صحيح vm_check_range(بنية proc *caller,
       بنية proc *هدف, vir_بائيٽون vir_addr, size_t بائيٽون, صحيح writable);

صحيح copy_نياپو_from_user(پيغام * user_mbuf, پيغام * dst);
صحيح copy_نياپو_منزل_user(پيغام * src, پيغام * user_mbuf);
فراغ switch_پتو_space(بنية proc * p);
فراغ release_پتو_space(بنية proc *pr);

فراغ enable_fpu_exception(فراغ);
فراغ disable_fpu_exception(فراغ);
فراغ release_fpu(بنية proc * p);
فراغ arch_pause(فراغ);
ننڍو cpu_ڀريو(فراغ);
فراغ busy_delay_ms(صحيح ms);

/* utility.c */
فراغ cpu_print_freq(بنانشان cpu);
#جيڪڏهن_پورو /* __kernel__ */

#جيڪڏهن_پورو /* PROTO_H */
