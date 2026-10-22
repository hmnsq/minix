/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#ଅନ୍ତର୍ଭାଵ <minix/safecopies.h>
#ଅନ୍ତର୍ଭାଵ <machine/archtypes.h>
#ଅନ୍ତର୍ଭାଵ <machine/signal.h>
#ଅନ୍ତର୍ଭାଵ <machine/frame.h>

/* Struct declarations. */
ସଂରଚନା proc;
ସଂରଚନା ipc_filter_s;

/* clock.c */
ଶୂନ୍ୟ init_clock(ଶୂନ୍ୟ);
clock_t get_realtime(ଶୂନ୍ୟ);
ଶୂନ୍ୟ set_realtime(clock_t);
ଶୂନ୍ୟ set_adjtime_delta(int32_t);
clock_t get_monotonic(ଶୂନ୍ୟ);
ଶୂନ୍ୟ set_boottime(time_t);
time_t get_boottime(ଶୂନ୍ୟ);
ଶୂନ୍ୟ set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg);
ଶୂନ୍ୟ reset_kernel_timer(minix_timer_t *tp);
ଶୂନ୍ୟ ser_dump_proc(ଶୂନ୍ୟ);

ଶୂନ୍ୟ cycles_accounting_init(ଶୂନ୍ୟ);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
ଶୂନ୍ୟ context_stop(ସଂରଚନା proc * p);
/* this is a wrapper to make calling it from assembly easier */
ଶୂନ୍ୟ context_stop_idle(ଶୂନ୍ୟ);
ଶୂନ୍ୟ get_cpu_ticks(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu, uint64_t ticks[MINIX_CPUSTATES]);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_fpu(ସଂରଚନା proc *);
ଶୂନ୍ୟ save_fpu(ସଂରଚନା proc *);
ଶୂନ୍ୟ save_local_fpu(ସଂରଚନା proc *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain);
ଶୂନ୍ୟ fpu_sigcontext(ସଂରଚନା proc *, ସଂରଚନା sigframe_sigcontext *fr, ସଂରଚନା
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
ଶୂନ୍ୟ kmain(kinfo_t *cbi);
ଶୂନ୍ୟ prepare_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how);
__dead ଶୂନ୍ୟ minix_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how);
ଶୂନ୍ୟ bsp_finish_booting(ଶୂନ୍ୟ);

/* proc.c */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_ipc(reg_t r1, reg_t r2, reg_t r3);
ଶୂନ୍ୟ proc_init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cancel_async(ସଂରଚନା proc *src, ସଂରଚନା proc *dst);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_pending_notify(ସଂରଚନା proc * caller, ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_pending_asend(ସଂରଚନା proc * caller, ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_p);
ଶୂନ୍ୟ unset_notify_pending(ସଂରଚନା proc * caller, ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mini_notify(ଅଚଳ ସଂରଚନା proc *src, endpoint_t dst);
ଶୂନ୍ୟ vm_suspend(ସଂରଚନା proc *caller, ଅଚଳ ସଂରଚନା proc *target,
        ଅଚଳ vir_bytes linaddr, ଅଚଳ vir_bytes len, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
        ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା writeflag);
ଶୂନ୍ୟ enqueue(ସଂରଚନା proc *rp);
ଶୂନ୍ୟ dequeue(ସଂରଚନା proc *rp);
ଶୂନ୍ୟ switch_to_user(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_proc_reset(ସଂରଚନା proc *rp);
ଶୂନ୍ୟ arch_proc_setcontext(ସଂରଚନା proc *rp, ସଂରଚନା stackframe_s *state,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା user, ପୂର୍ଣ୍ଣସଂଖ୍ୟା restorestyle);
ସଂରଚନା proc * arch_finish_switch_to_user(ଶୂନ୍ୟ);
ସଂରଚନା proc *endpoint_lookup(endpoint_t ep);
#ଯଦି DEBUG_ENABLE_IPC_WARNINGS
ପୂର୍ଣ୍ଣସଂଖ୍ୟା isokendpt_f(ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, endpoint_t e, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ଅନ୍ୟଥା
ପୂର୍ଣ୍ଣସଂଖ୍ୟା isokendpt_f(endpoint_t e, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
ଶୂନ୍ୟ proc_no_time(ସଂରଚନା proc *p);
ଶୂନ୍ୟ reset_proc_accounting(ସଂରଚନା proc *p);
ଶୂନ୍ୟ flag_account(ସଂରଚନା proc *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା try_deliver_senda(ସଂରଚନା proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
ଶୂନ୍ୟ cstart(ଶୂନ୍ୟ);
ଅକ୍ଷର *env_get(ଅଚଳ ଅକ୍ଷର *ଚାବି);

/* system.c */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_priv(ପଞ୍ଜିକରଣ ସଂରଚନା proc *rc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_type);
ଶୂନ୍ୟ set_sendto_bit(ଅଚଳ ସଂରଚନା proc *rc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id);
ଶୂନ୍ୟ unset_sendto_bit(ଅଚଳ ସଂରଚନା proc *rc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id);
ଶୂନ୍ୟ fill_sendto_mask(ଅଚଳ ସଂରଚନା proc *rc, sys_map_t *map);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send_sig(endpoint_t proc_nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_nr);
ଶୂନ୍ୟ cause_sig(proc_nr_t proc_nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig_nr);
ଶୂନ୍ୟ sig_delay_done(ସଂରଚନା proc *rp);
ଶୂନ୍ୟ send_diag_sig(ଶୂନ୍ୟ);
ଶୂନ୍ୟ kernel_call(message *m_user, ସଂରଚନା proc * caller);
ଶୂନ୍ୟ system_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ clear_endpoint(ସଂରଚନା proc *rc);
ଶୂନ୍ୟ clear_ipc_refs(ସଂରଚନା proc *rc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା caller_ret);
ଶୂନ୍ୟ kernel_call_resume(ସଂରଚନା proc *p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sched_proc(ସଂରଚନା proc *rp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quantum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା niced);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_ipc_filter(ସଂରଚନା proc *rp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type,
    vir_bytes address, size_t length);
ଶୂନ୍ୟ clear_ipc_filters(ସଂରଚନା proc *rp);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_ipc_filter(ସଂରଚନା ipc_filter_s *ipcf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fill_flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_ipc_filtered_ସନ୍ଦେଶ(ସଂରଚନା proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_ipc_filtered_memreq(ସଂରଚନା proc *src_rp, ସଂରଚନା proc *dst_rp);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା priv_add_irq(ସଂରଚନା proc *rp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା priv_add_io(ସଂରଚନା proc *rp, ସଂରଚନା io_range *ior);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା priv_add_mem(ସଂରଚନା proc *rp, ସଂରଚନା minix_mem_range *memr);

/* system/do_vtimer.c */
ଶୂନ୍ୟ vtimer_check(ସଂରଚନା proc *rp);

/* interrupt.c */
ଶୂନ୍ୟ put_irq_handler(irq_hook_t *hook, ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq, irq_handler_t handler);
ଶୂନ୍ୟ rm_irq_handler(ଅଚଳ irq_hook_t *hook);
ଶୂନ୍ୟ enable_irq(ଅଚଳ irq_hook_t *hook);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା disable_irq(ଅଚଳ irq_hook_t *hook);

ଶୂନ୍ୟ interrupts_enable(ଶୂନ୍ୟ);
ଶୂନ୍ୟ interrupts_disable(ଶୂନ୍ୟ);

/* debug.c */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok(ଶୂନ୍ୟ);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#ଅନ୍ୟଥା
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା runqueues_ok_cpu(unsigned cpu);
#endif
ଅକ୍ଷର *rtsflagstr(u32_t flags);
ଅକ୍ଷର *miscflagstr(u32_t flags);
ଅକ୍ଷର *schedulerstr(ସଂରଚନା proc *scheduler);
/* prints process information */
ଶୂନ୍ୟ print_proc(ସଂରଚନା proc *pp);
/* prints the given process and recursively all processes it depends on */
ଶୂନ୍ୟ print_proc_recursive(ସଂରଚନା proc *pp);
ଶୂନ୍ୟ printmsg(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst,
    ଅକ୍ଷର operation, ପୂର୍ଣ୍ଣସଂଖ୍ୟା printparams);
#ଯଦି DEBUG_IPC_HOOK
ଶୂନ୍ୟ hook_ipc_msgrecv(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst);
ଶୂନ୍ୟ hook_ipc_msgsend(message *ସନ୍ଦେଶ, ସଂରଚନା proc *src, ସଂରଚନା proc *dst);
ଶୂନ୍ୟ hook_ipc_msgkcall(message *ସନ୍ଦେଶ, ସଂରଚନା proc *proc);
ଶୂନ୍ୟ hook_ipc_msgkresult(message *ସନ୍ଦେଶ, ସଂରଚନା proc *proc);
ଶୂନ୍ୟ hook_ipc_clear(ସଂରଚନା proc *proc);
#endif

/* system/do_safecopy.c */
ସଂରଚନା cp_sfinfo; /* external callers may only provide NULL */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା,
	vir_bytes, vir_bytes *, endpoint_t *, ସଂରଚନା cp_sfinfo *);

/* system/do_diagctl.c */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_diagctl(ସଂରଚନା proc * caller, message *m);

#ଯଦି SPROFILE
/* profile.c */
ଶୂନ୍ୟ init_profile_clock(u32_t);
ଶୂନ୍ୟ stop_profile_clock(ଶୂନ୍ୟ);
#endif

/* functions defined in architecture-dependent files. */
ଶୂନ୍ୟ prot_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_post_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_set_secondary_ipc_return(ସଂରଚନା proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	ଗଣନା_2);
ଶୂନ୍ୟ phys_copy_fault(ଶୂନ୍ୟ);
ଶୂନ୍ୟ phys_copy_fault_in_kernel(ଶୂନ୍ୟ);
ଶୂନ୍ୟ memset_fault(ଶୂନ୍ୟ);
ଶୂନ୍ୟ memset_fault_in_kernel(ଶୂନ୍ୟ);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(ଖାଲି, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା virtual_copy_f(ସଂରଚନା proc * caller, ସଂରଚନା vir_addr *src, ସଂରଚନା
	vir_addr *dst, vir_bytes bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vmcheck);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_copy_vmcheck(ସଂରଚନା proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(ସଂରଚନା proc* rp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_memset(ସଂରଚନା proc *caller, endpoint_t who, phys_bytes dst,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern, phys_bytes ଗଣନା_2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା intr_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ halt_cpu(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_boot_proc(ସଂରଚନା boot_image *b, ସଂରଚନା proc *p);
ଶୂନ୍ୟ cpu_identify(ଶୂନ୍ୟ);
/* arch dependent FPU initialization per CPU */
ଶୂନ୍ୟ fpu_init(ଶୂନ୍ୟ);
/* returns true if pfu is present and initialized */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_fpu(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ser_putc(ଅକ୍ଷର);
__dead ଶୂନ୍ୟ arch_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ restore_user_context(ସଂରଚନା proc * p);
ଶୂନ୍ୟ read_tsc(u32_t *ଉପର, u32_t *ତଳ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_init_profile_clock(u32_t freq);
ଶୂନ୍ୟ arch_stop_profile_clock(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_ack_profile_clock(ଶୂନ୍ୟ);
ଶୂନ୍ୟ do_ser_debug(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_get_params(ଅକ୍ଷର *parm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max);
ଶୂନ୍ୟ memory_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ mem_clear_mapcache(ଶୂନ୍ୟ);
ଶୂନ୍ୟ arch_proc_init(ସଂରଚନା proc *pr, u32_t, u32_t, u32_t, ଅକ୍ଷର *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_do_vmctl(message *m_ptr, ସଂରଚନା proc *p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_contiguous(ଅଚଳ ସଂରଚନା proc *targetproc, vir_bytes vir_buf,
	size_t ଗଣନା_2);
ଶୂନ୍ୟ proc_stacktrace(ସଂରଚନା proc *proc);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_lookup(ଅଚଳ ସଂରଚନା proc *proc, vir_bytes ଆଭାସୀ, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(ଅଚଳ ସଂରଚନା proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
ଶୂନ୍ୟ arch_do_syscall(ସଂରଚନା proc *proc);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, phys_bytes *addr, phys_bytes *len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା
	*flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_phys_map_reply(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, vir_bytes addr);
reg_t arch_get_sp(ସଂରଚନା proc *p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା arch_enable_paging(ସଂରଚନା proc * caller);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_check_range(ସଂରଚନା proc *caller,
       ସଂରଚନା proc *target, vir_bytes vir_addr, size_t bytes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writable);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_ସନ୍ଦେଶ_from_user(message * user_mbuf, message * dst);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_ସନ୍ଦେଶ_to_user(message * src, message * user_mbuf);
ଶୂନ୍ୟ switch_address_space(ସଂରଚନା proc * p);
ଶୂନ୍ୟ release_address_space(ସଂରଚନା proc *pr);

ଶୂନ୍ୟ enable_fpu_exception(ଶୂନ୍ୟ);
ଶୂନ୍ୟ disable_fpu_exception(ଶୂନ୍ୟ);
ଶୂନ୍ୟ release_fpu(ସଂରଚନା proc * p);
ଶୂନ୍ୟ arch_pause(ଶୂନ୍ୟ);
short cpu_load(ଶୂନ୍ୟ);
ଶୂନ୍ୟ busy_delay_ms(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ms);

/* utility.c */
ଶୂନ୍ୟ cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
