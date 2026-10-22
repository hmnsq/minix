/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#اگر_تعریف_شده __kernel__

#اگر_تعریف_نشده PROTO_H
#تعریف_کن PROTO_H

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
فراغ مجموعه_realtime(clock_t);
فراغ مجموعه_adjtime_delta(int32_t);
clock_t get_monotonic(فراغ);
فراغ مجموعه_boottime(time_t);
time_t get_boottime(فراغ);
فراغ مجموعه_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, صحيح arg);
فراغ reset_kernel_timer(minix_timer_t *tp);
فراغ ser_dump_proc(فراغ);

فراغ چرخهها_accounting_init(فراغ);
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
فراغ get_cpu_ticks(بیعلامت صحيح cpu, uint64_t ticks[MINIX_CPUSTATES]);
صحيح restore_fpu(بنية proc *);
فراغ save_fpu(بنية proc *);
فراغ save_local_fpu(بنية proc *, صحيح retain);
فراغ fpu_sigcontext(بنية proc *, بنية sigframe_sigcontext *fr, بنية
	sigcontext *sc);

/* main.c */
#اگر_تعریف_نشده UNPAGED
#تعریف_کن kmain __k_unpaged_kmain
#پایان_اگر
فراغ kmain(kinfo_t *cbi);
فراغ prepare_shutdown(صحيح how);
__dead فراغ minix_shutdown(صحيح how);
فراغ bsp_پایان_booting(فراغ);

/* proc.c */

صحيح do_ipc(reg_t r1, reg_t r2, reg_t r3);
فراغ proc_init(فراغ);
صحيح cancel_async(بنية proc *src, بنية proc *dst);
صحيح has_pending_notify(بنية proc * caller, صحيح src_p);
صحيح has_pending_asend(بنية proc * caller, صحيح src_p);
فراغ unset_notify_pending(بنية proc * caller, صحيح src_p);
صحيح mini_notify(ثابت بنية proc *src, endpoint_t dst);
فراغ vm_suspend(بنية proc *caller, ثابت بنية proc *هدف,
        ثابت vir_بایتها linaddr, ثابت vir_بایتها len, ثابت صحيح type,
        ثابت صحيح writeflag);
فراغ افزودن_به_صف(بنية proc *rp);
فراغ خارج_از_صف(بنية proc *rp);
فراغ switch_مقصد_user(فراغ);
فراغ arch_proc_reset(بنية proc *rp);
فراغ arch_proc_setcontext(بنية proc *rp, بنية stackframe_s *state,
	صحيح user, صحيح restorestyle);
بنية proc * arch_پایان_switch_مقصد_user(فراغ);
بنية proc *endpoint_lookup(endpoint_t ep);
#إذا DEBUG_ENABLE_IPC_WARNINGS
صحيح isokendpt_f(ثابت حرف *file, صحيح سطر, endpoint_t e, صحيح *p, صحيح
	f);
#تعریف_کن isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#وإلا
صحيح isokendpt_f(endpoint_t e, صحيح *p, صحيح f);
#تعریف_کن isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#پایان_اگر
فراغ proc_no_زمان(بنية proc *p);
فراغ reset_proc_accounting(بنية proc *p);
فراغ flag_account(بنية proc *p, صحيح flag);
صحيح try_deliver_senda(بنية proc *caller_ptr, asynmsg_t *جدول, size_t
	اندازه);

/* start.c */
فراغ cstart(فراغ);
حرف *env_get(ثابت حرف *کلید);

/* system.c */
صحيح get_priv(سجل بنية proc *rc, صحيح proc_type);
فراغ مجموعه_sendto_بیت(ثابت بنية proc *rc, صحيح id);
فراغ unset_sendto_بیت(ثابت بنية proc *rc, صحيح id);
فراغ fill_sendto_ماسک(ثابت بنية proc *rc, sys_map_t *map);
صحيح send_sig(endpoint_t proc_nr, صحيح sig_nr);
فراغ cause_sig(proc_nr_t proc_nr, صحيح sig_nr);
فراغ sig_delay_انجامشده(بنية proc *rp);
فراغ send_diag_sig(فراغ);
فراغ kernel_call(پیام *m_user, بنية proc * caller);
فراغ system_init(فراغ);
فراغ clear_endpoint(بنية proc *rc);
فراغ clear_ipc_refs(بنية proc *rc, صحيح caller_ret);
فراغ kernel_call_resume(بنية proc *p);
صحيح sched_proc(بنية proc *rp, صحيح اولویت, صحيح برش_زمانی, صحيح cpu, صحيح niced);
صحيح جمع_کن_ipc_filter(بنية proc *rp, صحيح type,
    vir_بایتها آدرس, size_t درازی);
فراغ clear_ipc_filters(بنية proc *rp);
صحيح check_ipc_filter(بنية ipc_filter_s *ipcf, صحيح fill_flags);
صحيح allow_ipc_filtered_پیام(بنية proc *rp, endpoint_t src_e,
    vir_بایتها m_src_v, پیام *m_src_p);
صحيح allow_ipc_filtered_memreq(بنية proc *src_rp, بنية proc *dst_rp);
صحيح priv_جمع_کن_irq(بنية proc *rp, صحيح irq);
صحيح priv_جمع_کن_io(بنية proc *rp, بنية io_range *ior);
صحيح priv_جمع_کن_mem(بنية proc *rp, بنية minix_mem_range *memr);

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
#اگر_تعریف_نشده CONFIG_SMP
#تعریف_کن runqueues_ok_local runqueues_ok
#وإلا
#تعریف_کن runqueues_ok_local() runqueues_ok_cpu(cpuid)
صحيح runqueues_ok_cpu(بیعلامت cpu);
#پایان_اگر
حرف *rtsflagstr(u32_t flags);
حرف *miscflagstr(u32_t flags);
حرف *schedulerstr(بنية proc *scheduler);
/* prints process information */
فراغ print_proc(بنية proc *pp);
/* prints the given process and recursively all processes it depends on */
فراغ print_proc_recursive(بنية proc *pp);
فراغ printmsg(پیام *پیام_2, بنية proc *src, بنية proc *dst,
    حرف operation, صحيح printparams);
#إذا DEBUG_IPC_HOOK
فراغ hook_ipc_msgrecv(پیام *پیام_2, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgsend(پیام *پیام_2, بنية proc *src, بنية proc *dst);
فراغ hook_ipc_msgkcall(پیام *پیام_2, بنية proc *proc);
فراغ hook_ipc_msgkresult(پیام *پیام_2, بنية proc *proc);
فراغ hook_ipc_clear(بنية proc *proc);
#پایان_اگر

/* system/do_safecopy.c */
بنية cp_sfinfo; /* external callers may only provide NULL */
صحيح verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_بایتها, صحيح,
	vir_بایتها, vir_بایتها *, endpoint_t *, بنية cp_sfinfo *);

/* system/do_diagctl.c */
صحيح do_diagctl(بنية proc * caller, پیام *m);

#إذا SPROFILE
/* profile.c */
فراغ init_profile_clock(u32_t);
فراغ stop_profile_clock(فراغ);
#پایان_اگر

/* functions defined in architecture-dependent files. */
فراغ prot_init(فراغ);
فراغ arch_post_init(فراغ);
فراغ arch_مجموعه_secondary_ipc_return(بنية proc *, u32_t val);
phys_بایتها phys_copy(phys_بایتها منبع, phys_بایتها dest, phys_بایتها
	شمار);
فراغ phys_copy_fault(فراغ);
فراغ phys_copy_fault_درون_kernel(فراغ);
فراغ memset_fault(فراغ);
فراغ memset_fault_درون_kernel(فراغ);
#تعریف_کن virtual_copy(src, dst, بایتها) \
				virtual_copy_f(عدم, src, dst, بایتها, 0)
#تعریف_کن virtual_copy_vmcheck(caller, src, dst, بایتها) \
				virtual_copy_f(caller, src, dst, بایتها, 1)
صحيح virtual_copy_f(بنية proc * caller, بنية vir_addr *src, بنية
	vir_addr *dst, vir_بایتها بایتها, صحيح vmcheck);
صحيح داده_copy(endpoint_t from, vir_بایتها from_addr, endpoint_t مقصد,
	vir_بایتها مقصد_addr, size_t بایتها);
صحيح داده_copy_vmcheck(بنية proc *, endpoint_t from, vir_بایتها
	from_addr, endpoint_t مقصد, vir_بایتها مقصد_addr, size_t بایتها);
phys_بایتها umap_virtual(بنية proc* rp, صحيح seg, vir_بایتها vir_addr,
	vir_بایتها بایتها);
phys_بایتها seg2phys(u16_t);
صحيح vm_memset(بنية proc *caller, endpoint_t who, phys_بایتها dst,
	صحيح الگو, phys_بایتها شمار);
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
فراغ read_tsc(u32_t *بالا, u32_t *کم);
صحيح arch_init_profile_clock(u32_t freq);
فراغ arch_stop_profile_clock(فراغ);
فراغ arch_ack_profile_clock(فراغ);
فراغ do_ser_debug(فراغ);
صحيح arch_get_params(حرف *parm, صحيح max);
فراغ حافظه_init(فراغ);
فراغ mem_clear_mapcache(فراغ);
فراغ arch_proc_init(بنية proc *pr, u32_t, u32_t, u32_t, حرف *);
صحيح arch_do_vmctl(پیام *m_ptr, بنية proc *p);
صحيح vm_contiguous(ثابت بنية proc *targetproc, vir_بایتها vir_buf,
	size_t شمار);
فراغ proc_stacktrace(بنية proc *proc);
صحيح vm_lookup(ثابت بنية proc *proc, vir_بایتها مجازی, phys_بایتها
	*نتیجه, u32_t *ptent);
size_t vm_lookup_range(ثابت بنية proc *proc,
       vir_بایتها vir_addr, phys_بایتها *phys_addr, size_t بایتها);
فراغ arch_do_syscall(بنية proc *proc);
صحيح arch_phys_map(صحيح شاخص, phys_بایتها *addr, phys_بایتها *len, صحيح
	*flags);
صحيح arch_phys_map_پاسخ(صحيح شاخص, vir_بایتها addr);
reg_t arch_get_sp(بنية proc *p);
صحيح arch_enable_paging(بنية proc * caller);
صحيح vm_check_range(بنية proc *caller,
       بنية proc *هدف, vir_بایتها vir_addr, size_t بایتها, صحيح writable);

صحيح copy_پیام_from_user(پیام * user_mbuf, پیام * dst);
صحيح copy_پیام_مقصد_user(پیام * src, پیام * user_mbuf);
فراغ switch_آدرس_space(بنية proc * p);
فراغ release_آدرس_space(بنية proc *pr);

فراغ enable_fpu_exception(فراغ);
فراغ disable_fpu_exception(فراغ);
فراغ release_fpu(بنية proc * p);
فراغ arch_pause(فراغ);
کوتاه cpu_بارگیری(فراغ);
فراغ busy_delay_ms(صحيح ms);

/* utility.c */
فراغ cpu_print_freq(بیعلامت cpu);
#پایان_اگر /* __kernel__ */

#پایان_اگر /* PROTO_H */
