/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __kernel__

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROTO_H
#ವ್ಯಾಖ್ಯಾನಿಸು PROTO_H

#ಅನ್ತರ್ಭಾವ <minix/safecopies.h>
#ಅನ್ತರ್ಭಾವ <machine/archtypes.h>
#ಅನ್ತರ್ಭಾವ <machine/signal.h>
#ಅನ್ತರ್ಭಾವ <machine/frame.h>

/* Struct declarations. */
ರಚನೆ proc;
ರಚನೆ ipc_filter_s;

/* clock.c */
ಶೂನ್ಯ init_clock(ಶೂನ್ಯ);
clock_t get_realtime(ಶೂನ್ಯ);
ಶೂನ್ಯ ಗಣ_realtime(clock_t);
ಶೂನ್ಯ ಗಣ_adjtime_delta(int32_t);
clock_t get_monotonic(ಶೂನ್ಯ);
ಶೂನ್ಯ ಗಣ_boottime(time_t);
time_t get_boottime(ಶೂನ್ಯ);
ಶೂನ್ಯ ಗಣ_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, ಪೂರ್ಣಾಂಕ arg);
ಶೂನ್ಯ reset_kernel_timer(minix_timer_t *tp);
ಶೂನ್ಯ ser_dump_proc(ಶೂನ್ಯ);

ಶೂನ್ಯ ಚಕ್ರಗಳು_accounting_init(ಶೂನ್ಯ);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
ಶೂನ್ಯ context_stop(ರಚನೆ proc * p);
/* this is a wrapper to make calling it from assembly easier */
ಶೂನ್ಯ context_stop_idle(ಶೂನ್ಯ);
ಶೂನ್ಯ get_cpu_ticks(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpu, uint64_t ticks[MINIX_CPUSTATES]);
ಪೂರ್ಣಾಂಕ restore_fpu(ರಚನೆ proc *);
ಶೂನ್ಯ save_fpu(ರಚನೆ proc *);
ಶೂನ್ಯ save_local_fpu(ರಚನೆ proc *, ಪೂರ್ಣಾಂಕ retain);
ಶೂನ್ಯ fpu_sigcontext(ರಚನೆ proc *, ರಚನೆ sigframe_sigcontext *fr, ರಚನೆ
	sigcontext *sc);

/* main.c */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNPAGED
#ವ್ಯಾಖ್ಯಾನಿಸು kmain __k_unpaged_kmain
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ kmain(kinfo_t *cbi);
ಶೂನ್ಯ prepare_shutdown(ಪೂರ್ಣಾಂಕ how);
__dead ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಂಕ how);
ಶೂನ್ಯ bsp_ಮುಕ್ತಾಯ_booting(ಶೂನ್ಯ);

/* proc.c */

ಪೂರ್ಣಾಂಕ do_ipc(reg_t r1, reg_t r2, reg_t r3);
ಶೂನ್ಯ proc_init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ cancel_async(ರಚನೆ proc *src, ರಚನೆ proc *dst);
ಪೂರ್ಣಾಂಕ has_pending_notify(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p);
ಪೂರ್ಣಾಂಕ has_pending_asend(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p);
ಶೂನ್ಯ unset_notify_pending(ರಚನೆ proc * caller, ಪೂರ್ಣಾಂಕ src_p);
ಪೂರ್ಣಾಂಕ mini_notify(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *src, endpoint_t dst);
ಶೂನ್ಯ vm_suspend(ರಚನೆ proc *caller, ಸ್ಥಿರಾಂಕ ರಚನೆ proc *ಗುರಿ,
        ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು linaddr, ಸ್ಥಿರಾಂಕ vir_ಬೈಟುಗಳು len, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ type,
        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ writeflag);
ಶೂನ್ಯ ಸರತಿಗೆ_ಸೇರಿಸು(ರಚನೆ proc *rp);
ಶೂನ್ಯ ಸರತಿಯಿಂದ_ತೆಗೆ(ರಚನೆ proc *rp);
ಶೂನ್ಯ switch_ಗಮ್ಯ_user(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_proc_reset(ರಚನೆ proc *rp);
ಶೂನ್ಯ arch_proc_setcontext(ರಚನೆ proc *rp, ರಚನೆ stackframe_s *state,
	ಪೂರ್ಣಾಂಕ user, ಪೂರ್ಣಾಂಕ restorestyle);
ರಚನೆ proc * arch_ಮುಕ್ತಾಯ_switch_ಗಮ್ಯ_user(ಶೂನ್ಯ);
ರಚನೆ proc *endpoint_lookup(endpoint_t ep);
#ಆದರೆ DEBUG_ENABLE_IPC_WARNINGS
ಪೂರ್ಣಾಂಕ isokendpt_f(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, endpoint_t e, ಪೂರ್ಣಾಂಕ *p, ಪೂರ್ಣಾಂಕ
	f);
#ವ್ಯಾಖ್ಯಾನಿಸು isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ isokendpt_f(endpoint_t e, ಪೂರ್ಣಾಂಕ *p, ಪೂರ್ಣಾಂಕ f);
#ವ್ಯಾಖ್ಯಾನಿಸು isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ proc_no_ಸಮಯ(ರಚನೆ proc *p);
ಶೂನ್ಯ reset_proc_accounting(ರಚನೆ proc *p);
ಶೂನ್ಯ flag_account(ರಚನೆ proc *p, ಪೂರ್ಣಾಂಕ flag);
ಪೂರ್ಣಾಂಕ try_deliver_senda(ರಚನೆ proc *caller_ptr, asynmsg_t *ಕೋಷ್ಟಕ, size_t
	ಗಾತ್ರ_2);

/* start.c */
ಶೂನ್ಯ cstart(ಶೂನ್ಯ);
ಅಕ್ಷರ *env_get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);

/* system.c */
ಪೂರ್ಣಾಂಕ get_priv(ನೋಂದಣಿ ರಚನೆ proc *rc, ಪೂರ್ಣಾಂಕ proc_type);
ಶೂನ್ಯ ಗಣ_sendto_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rc, ಪೂರ್ಣಾಂಕ id);
ಶೂನ್ಯ unset_sendto_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rc, ಪೂರ್ಣಾಂಕ id);
ಶೂನ್ಯ fill_sendto_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *rc, sys_map_t *map);
ಪೂರ್ಣಾಂಕ send_sig(endpoint_t proc_nr, ಪೂರ್ಣಾಂಕ sig_nr);
ಶೂನ್ಯ cause_sig(proc_nr_t proc_nr, ಪೂರ್ಣಾಂಕ sig_nr);
ಶೂನ್ಯ sig_delay_ಮುಗಿದ(ರಚನೆ proc *rp);
ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call(ಸಂದೇಶ *m_user, ರಚನೆ proc * caller);
ಶೂನ್ಯ system_init(ಶೂನ್ಯ);
ಶೂನ್ಯ clear_endpoint(ರಚನೆ proc *rc);
ಶೂನ್ಯ clear_ipc_refs(ರಚನೆ proc *rc, ಪೂರ್ಣಾಂಕ caller_ret);
ಶೂನ್ಯ kernel_call_resume(ರಚನೆ proc *p);
ಪೂರ್ಣಾಂಕ sched_proc(ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಪೂರ್ಣಾಂಕ ಕಾಲಾಂಶ, ಪೂರ್ಣಾಂಕ cpu, ಪೂರ್ಣಾಂಕ niced);
ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_ipc_filter(ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ type,
    vir_ಬೈಟುಗಳು ವಿಳಾಸ, size_t ಉದ್ದಳತೆ);
ಶೂನ್ಯ clear_ipc_filters(ರಚನೆ proc *rp);
ಪೂರ್ಣಾಂಕ check_ipc_filter(ರಚನೆ ipc_filter_s *ipcf, ಪೂರ್ಣಾಂಕ fill_flags);
ಪೂರ್ಣಾಂಕ allow_ipc_filtered_ಸಂದೇಶ(ರಚನೆ proc *rp, endpoint_t src_e,
    vir_ಬೈಟುಗಳು m_src_v, ಸಂದೇಶ *m_src_p);
ಪೂರ್ಣಾಂಕ allow_ipc_filtered_memreq(ರಚನೆ proc *src_rp, ರಚನೆ proc *dst_rp);
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_irq(ರಚನೆ proc *rp, ಪೂರ್ಣಾಂಕ irq);
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_io(ರಚನೆ proc *rp, ರಚನೆ io_range *ior);
ಪೂರ್ಣಾಂಕ priv_ಕೂಡಿಸು_mem(ರಚನೆ proc *rp, ರಚನೆ minix_mem_range *memr);

/* system/do_vtimer.c */
ಶೂನ್ಯ vtimer_check(ರಚನೆ proc *rp);

/* interrupt.c */
ಶೂನ್ಯ put_irq_handler(irq_hook_t *hook, ಪೂರ್ಣಾಂಕ irq, irq_handler_t handler);
ಶೂನ್ಯ rm_irq_handler(ಸ್ಥಿರಾಂಕ irq_hook_t *hook);
ಶೂನ್ಯ enable_irq(ಸ್ಥಿರಾಂಕ irq_hook_t *hook);
ಪೂರ್ಣಾಂಕ disable_irq(ಸ್ಥಿರಾಂಕ irq_hook_t *hook);

ಶೂನ್ಯ interrupts_enable(ಶೂನ್ಯ);
ಶೂನ್ಯ interrupts_disable(ಶೂನ್ಯ);

/* debug.c */
ಪೂರ್ಣಾಂಕ runqueues_ok(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_SMP
#ವ್ಯಾಖ್ಯಾನಿಸು runqueues_ok_local runqueues_ok
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು runqueues_ok_local() runqueues_ok_cpu(cpuid)
ಪೂರ್ಣಾಂಕ runqueues_ok_cpu(ಚಿಹ್ನೆರಹಿತ cpu);
#ಆದರೆ_ಅಂತ್ಯ
ಅಕ್ಷರ *rtsflagstr(u32_t flags);
ಅಕ್ಷರ *miscflagstr(u32_t flags);
ಅಕ್ಷರ *schedulerstr(ರಚನೆ proc *scheduler);
/* prints process information */
ಶೂನ್ಯ print_proc(ರಚನೆ proc *pp);
/* prints the given process and recursively all processes it depends on */
ಶೂನ್ಯ print_proc_recursive(ರಚನೆ proc *pp);
ಶೂನ್ಯ printmsg(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst,
    ಅಕ್ಷರ operation, ಪೂರ್ಣಾಂಕ printparams);
#ಆದರೆ DEBUG_IPC_HOOK
ಶೂನ್ಯ hook_ipc_msgrecv(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst);
ಶೂನ್ಯ hook_ipc_msgsend(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *src, ರಚನೆ proc *dst);
ಶೂನ್ಯ hook_ipc_msgkcall(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *proc);
ಶೂನ್ಯ hook_ipc_msgkresult(ಸಂದೇಶ *ಸಂದೇಶ_2, ರಚನೆ proc *proc);
ಶೂನ್ಯ hook_ipc_clear(ರಚನೆ proc *proc);
#ಆದರೆ_ಅಂತ್ಯ

/* system/do_safecopy.c */
ರಚನೆ cp_sfinfo; /* external callers may only provide NULL */
ಪೂರ್ಣಾಂಕ verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ,
	vir_ಬೈಟುಗಳು, vir_ಬೈಟುಗಳು *, endpoint_t *, ರಚನೆ cp_sfinfo *);

/* system/do_diagctl.c */
ಪೂರ್ಣಾಂಕ do_diagctl(ರಚನೆ proc * caller, ಸಂದೇಶ *m);

#ಆದರೆ SPROFILE
/* profile.c */
ಶೂನ್ಯ init_profile_clock(u32_t);
ಶೂನ್ಯ stop_profile_clock(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

/* functions defined in architecture-dependent files. */
ಶೂನ್ಯ prot_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_post_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_ಗಣ_secondary_ipc_return(ರಚನೆ proc *, u32_t val);
phys_ಬೈಟುಗಳು phys_copy(phys_ಬೈಟುಗಳು ಮೂಲ, phys_ಬೈಟುಗಳು dest, phys_ಬೈಟುಗಳು
	ಎಣಿಕೆ_2);
ಶೂನ್ಯ phys_copy_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ phys_copy_fault_ಒಳ_kernel(ಶೂನ್ಯ);
ಶೂನ್ಯ memset_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ memset_fault_ಒಳ_kernel(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿಸು virtual_copy(src, dst, ಬೈಟುಗಳು) \
				virtual_copy_f(ಖಾಲಿ, src, dst, ಬೈಟುಗಳು, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು virtual_copy_vmcheck(caller, src, dst, ಬೈಟುಗಳು) \
				virtual_copy_f(caller, src, dst, ಬೈಟುಗಳು, 1)
ಪೂರ್ಣಾಂಕ virtual_copy_f(ರಚನೆ proc * caller, ರಚನೆ vir_addr *src, ರಚನೆ
	vir_addr *dst, vir_ಬೈಟುಗಳು ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ vmcheck);
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy(endpoint_t from, vir_ಬೈಟುಗಳು from_addr, endpoint_t ಗಮ್ಯ,
	vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr, size_t ಬೈಟುಗಳು);
ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_copy_vmcheck(ರಚನೆ proc *, endpoint_t from, vir_ಬೈಟುಗಳು
	from_addr, endpoint_t ಗಮ್ಯ, vir_ಬೈಟುಗಳು ಗಮ್ಯ_addr, size_t ಬೈಟುಗಳು);
phys_ಬೈಟುಗಳು umap_virtual(ರಚನೆ proc* rp, ಪೂರ್ಣಾಂಕ seg, vir_ಬೈಟುಗಳು vir_addr,
	vir_ಬೈಟುಗಳು ಬೈಟುಗಳು);
phys_ಬೈಟುಗಳು seg2phys(u16_t);
ಪೂರ್ಣಾಂಕ vm_memset(ರಚನೆ proc *caller, endpoint_t who, phys_ಬೈಟುಗಳು dst,
	ಪೂರ್ಣಾಂಕ ನಮೂನೆ, phys_ಬೈಟುಗಳು ಎಣಿಕೆ_2);
ಪೂರ್ಣಾಂಕ intr_init(ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ halt_cpu(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_init(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_boot_proc(ರಚನೆ boot_image *b, ರಚನೆ proc *p);
ಶೂನ್ಯ cpu_identify(ಶೂನ್ಯ);
/* arch dependent FPU initialization per CPU */
ಶೂನ್ಯ fpu_init(ಶೂನ್ಯ);
/* returns true if pfu is present and initialized */
ಪೂರ್ಣಾಂಕ is_fpu(ಶೂನ್ಯ);
ಶೂನ್ಯ ser_putc(ಅಕ್ಷರ);
__dead ಶೂನ್ಯ arch_shutdown(ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ restore_user_context(ರಚನೆ proc * p);
ಶೂನ್ಯ read_tsc(u32_t *ಮೇಲಿನ, u32_t *ಕೆಳಗಿನ);
ಪೂರ್ಣಾಂಕ arch_init_profile_clock(u32_t freq);
ಶೂನ್ಯ arch_stop_profile_clock(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_ack_profile_clock(ಶೂನ್ಯ);
ಶೂನ್ಯ do_ser_debug(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ arch_get_params(ಅಕ್ಷರ *parm, ಪೂರ್ಣಾಂಕ max);
ಶೂನ್ಯ ಸ್ಮರಣೆ_init(ಶೂನ್ಯ);
ಶೂನ್ಯ mem_clear_mapcache(ಶೂನ್ಯ);
ಶೂನ್ಯ arch_proc_init(ರಚನೆ proc *pr, u32_t, u32_t, u32_t, ಅಕ್ಷರ *);
ಪೂರ್ಣಾಂಕ arch_do_vmctl(ಸಂದೇಶ *m_ptr, ರಚನೆ proc *p);
ಪೂರ್ಣಾಂಕ vm_contiguous(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *targetproc, vir_ಬೈಟುಗಳು vir_buf,
	size_t ಎಣಿಕೆ_2);
ಶೂನ್ಯ proc_stacktrace(ರಚನೆ proc *proc);
ಪೂರ್ಣಾಂಕ vm_lookup(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc, vir_ಬೈಟುಗಳು ವಾಸ್ತವಿಕ, phys_ಬೈಟುಗಳು
	*ಫಲಿತಾಂಶ, u32_t *ptent);
size_t vm_lookup_range(ಸ್ಥಿರಾಂಕ ರಚನೆ proc *proc,
       vir_ಬೈಟುಗಳು vir_addr, phys_ಬೈಟುಗಳು *phys_addr, size_t ಬೈಟುಗಳು);
ಶೂನ್ಯ arch_do_syscall(ರಚನೆ proc *proc);
ಪೂರ್ಣಾಂಕ arch_phys_map(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, phys_ಬೈಟುಗಳು *addr, phys_ಬೈಟುಗಳು *len, ಪೂರ್ಣಾಂಕ
	*flags);
ಪೂರ್ಣಾಂಕ arch_phys_map_ಉತ್ತರ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, vir_ಬೈಟುಗಳು addr);
reg_t arch_get_sp(ರಚನೆ proc *p);
ಪೂರ್ಣಾಂಕ arch_enable_paging(ರಚನೆ proc * caller);
ಪೂರ್ಣಾಂಕ vm_check_range(ರಚನೆ proc *caller,
       ರಚನೆ proc *ಗುರಿ, vir_ಬೈಟುಗಳು vir_addr, size_t ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ writable);

ಪೂರ್ಣಾಂಕ copy_ಸಂದೇಶ_from_user(ಸಂದೇಶ * user_mbuf, ಸಂದೇಶ * dst);
ಪೂರ್ಣಾಂಕ copy_ಸಂದೇಶ_ಗಮ್ಯ_user(ಸಂದೇಶ * src, ಸಂದೇಶ * user_mbuf);
ಶೂನ್ಯ switch_ವಿಳಾಸ_space(ರಚನೆ proc * p);
ಶೂನ್ಯ release_ವಿಳಾಸ_space(ರಚನೆ proc *pr);

ಶೂನ್ಯ enable_fpu_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ disable_fpu_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ release_fpu(ರಚನೆ proc * p);
ಶೂನ್ಯ arch_pause(ಶೂನ್ಯ);
ಗಿಡ್ಡ cpu_ತುಂಬು(ಶೂನ್ಯ);
ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಂಕ ms);

/* utility.c */
ಶೂನ್ಯ cpu_print_freq(ಚಿಹ್ನೆರಹಿತ cpu);
#ಆದರೆ_ಅಂತ್ಯ /* __kernel__ */

#ಆದರೆ_ಅಂತ್ಯ /* PROTO_H */
