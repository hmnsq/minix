/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#അന്തര്ഭാവ <minix/safecopies.h>
#അന്തര്ഭാവ <machine/archtypes.h>
#അന്തര്ഭാവ <machine/signal.h>
#അന്തര്ഭാവ <machine/frame.h>

/* Struct declarations. */
ഘടന proc;
ഘടന ipc_filter_s;

/* clock.c */
ശൂന്യം init_clock(ശൂന്യം);
clock_t get_realtime(ശൂന്യം);
ശൂന്യം set_realtime(clock_t);
ശൂന്യം set_adjtime_delta(int32_t);
clock_t get_monotonic(ശൂന്യം);
ശൂന്യം set_boottime(time_t);
time_t get_boottime(ശൂന്യം);
ശൂന്യം set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, പൂർണ്ണസംഖ്യ arg);
ശൂന്യം reset_kernel_timer(minix_timer_t *tp);
ശൂന്യം ser_dump_proc(ശൂന്യം);

ശൂന്യം cycles_accounting_init(ശൂന്യം);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
ശൂന്യം context_stop(ഘടന proc * p);
/* this is a wrapper to make calling it from assembly easier */
ശൂന്യം context_stop_idle(ശൂന്യം);
ശൂന്യം get_cpu_ticks(unsigned പൂർണ്ണസംഖ്യ cpu, uint64_t ticks[MINIX_CPUSTATES]);
പൂർണ്ണസംഖ്യ restore_fpu(ഘടന proc *);
ശൂന്യം save_fpu(ഘടന proc *);
ശൂന്യം save_local_fpu(ഘടന proc *, പൂർണ്ണസംഖ്യ retain);
ശൂന്യം fpu_sigcontext(ഘടന proc *, ഘടന sigframe_sigcontext *fr, ഘടന
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
ശൂന്യം kmain(kinfo_t *cbi);
ശൂന്യം prepare_shutdown(പൂർണ്ണസംഖ്യ how);
__dead ശൂന്യം minix_shutdown(പൂർണ്ണസംഖ്യ how);
ശൂന്യം bsp_finish_booting(ശൂന്യം);

/* proc.c */

പൂർണ്ണസംഖ്യ do_ipc(reg_t r1, reg_t r2, reg_t r3);
ശൂന്യം proc_init(ശൂന്യം);
പൂർണ്ണസംഖ്യ cancel_async(ഘടന proc *src, ഘടന proc *dst);
പൂർണ്ണസംഖ്യ has_pending_notify(ഘടന proc * caller, പൂർണ്ണസംഖ്യ src_p);
പൂർണ്ണസംഖ്യ has_pending_asend(ഘടന proc * caller, പൂർണ്ണസംഖ്യ src_p);
ശൂന്യം unset_notify_pending(ഘടന proc * caller, പൂർണ്ണസംഖ്യ src_p);
പൂർണ്ണസംഖ്യ mini_notify(സ്ഥിരം ഘടന proc *src, endpoint_t dst);
ശൂന്യം vm_suspend(ഘടന proc *caller, സ്ഥിരം ഘടന proc *target,
        സ്ഥിരം vir_bytes linaddr, സ്ഥിരം vir_bytes len, സ്ഥിരം പൂർണ്ണസംഖ്യ type,
        സ്ഥിരം പൂർണ്ണസംഖ്യ writeflag);
ശൂന്യം enqueue(ഘടന proc *rp);
ശൂന്യം dequeue(ഘടന proc *rp);
ശൂന്യം switch_to_user(ശൂന്യം);
ശൂന്യം arch_proc_reset(ഘടന proc *rp);
ശൂന്യം arch_proc_setcontext(ഘടന proc *rp, ഘടന stackframe_s *state,
	പൂർണ്ണസംഖ്യ user, പൂർണ്ണസംഖ്യ restorestyle);
ഘടന proc * arch_finish_switch_to_user(ശൂന്യം);
ഘടന proc *endpoint_lookup(endpoint_t ep);
#എങ്കിൽ DEBUG_ENABLE_IPC_WARNINGS
പൂർണ്ണസംഖ്യ isokendpt_f(സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, endpoint_t e, പൂർണ്ണസംഖ്യ *p, പൂർണ്ണസംഖ്യ
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#അല്ലെങ്കിൽ
പൂർണ്ണസംഖ്യ isokendpt_f(endpoint_t e, പൂർണ്ണസംഖ്യ *p, പൂർണ്ണസംഖ്യ f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
ശൂന്യം proc_no_time(ഘടന proc *p);
ശൂന്യം reset_proc_accounting(ഘടന proc *p);
ശൂന്യം flag_account(ഘടന proc *p, പൂർണ്ണസംഖ്യ flag);
പൂർണ്ണസംഖ്യ try_deliver_senda(ഘടന proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
ശൂന്യം cstart(ശൂന്യം);
അക്ഷരം *env_get(സ്ഥിരം അക്ഷരം *താക്കോൽ);

/* system.c */
പൂർണ്ണസംഖ്യ get_priv(രജിസ്റ്റർ ഘടന proc *rc, പൂർണ്ണസംഖ്യ proc_type);
ശൂന്യം set_sendto_bit(സ്ഥിരം ഘടന proc *rc, പൂർണ്ണസംഖ്യ id);
ശൂന്യം unset_sendto_bit(സ്ഥിരം ഘടന proc *rc, പൂർണ്ണസംഖ്യ id);
ശൂന്യം fill_sendto_mask(സ്ഥിരം ഘടന proc *rc, sys_map_t *map);
പൂർണ്ണസംഖ്യ send_sig(endpoint_t proc_nr, പൂർണ്ണസംഖ്യ sig_nr);
ശൂന്യം cause_sig(proc_nr_t proc_nr, പൂർണ്ണസംഖ്യ sig_nr);
ശൂന്യം sig_delay_done(ഘടന proc *rp);
ശൂന്യം send_diag_sig(ശൂന്യം);
ശൂന്യം kernel_call(message *m_user, ഘടന proc * caller);
ശൂന്യം system_init(ശൂന്യം);
ശൂന്യം clear_endpoint(ഘടന proc *rc);
ശൂന്യം clear_ipc_refs(ഘടന proc *rc, പൂർണ്ണസംഖ്യ caller_ret);
ശൂന്യം kernel_call_resume(ഘടന proc *p);
പൂർണ്ണസംഖ്യ sched_proc(ഘടന proc *rp, പൂർണ്ണസംഖ്യ priority, പൂർണ്ണസംഖ്യ quantum, പൂർണ്ണസംഖ്യ cpu, പൂർണ്ണസംഖ്യ niced);
പൂർണ്ണസംഖ്യ add_ipc_filter(ഘടന proc *rp, പൂർണ്ണസംഖ്യ type,
    vir_bytes address, size_t length);
ശൂന്യം clear_ipc_filters(ഘടന proc *rp);
പൂർണ്ണസംഖ്യ check_ipc_filter(ഘടന ipc_filter_s *ipcf, പൂർണ്ണസംഖ്യ fill_flags);
പൂർണ്ണസംഖ്യ allow_ipc_filtered_സന്ദേശം(ഘടന proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
പൂർണ്ണസംഖ്യ allow_ipc_filtered_memreq(ഘടന proc *src_rp, ഘടന proc *dst_rp);
പൂർണ്ണസംഖ്യ priv_add_irq(ഘടന proc *rp, പൂർണ്ണസംഖ്യ irq);
പൂർണ്ണസംഖ്യ priv_add_io(ഘടന proc *rp, ഘടന io_range *ior);
പൂർണ്ണസംഖ്യ priv_add_mem(ഘടന proc *rp, ഘടന minix_mem_range *memr);

/* system/do_vtimer.c */
ശൂന്യം vtimer_check(ഘടന proc *rp);

/* interrupt.c */
ശൂന്യം put_irq_handler(irq_hook_t *hook, പൂർണ്ണസംഖ്യ irq, irq_handler_t handler);
ശൂന്യം rm_irq_handler(സ്ഥിരം irq_hook_t *hook);
ശൂന്യം enable_irq(സ്ഥിരം irq_hook_t *hook);
പൂർണ്ണസംഖ്യ disable_irq(സ്ഥിരം irq_hook_t *hook);

ശൂന്യം interrupts_enable(ശൂന്യം);
ശൂന്യം interrupts_disable(ശൂന്യം);

/* debug.c */
പൂർണ്ണസംഖ്യ runqueues_ok(ശൂന്യം);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#അല്ലെങ്കിൽ
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
പൂർണ്ണസംഖ്യ runqueues_ok_cpu(unsigned cpu);
#endif
അക്ഷരം *rtsflagstr(u32_t flags);
അക്ഷരം *miscflagstr(u32_t flags);
അക്ഷരം *schedulerstr(ഘടന proc *scheduler);
/* prints process information */
ശൂന്യം print_proc(ഘടന proc *pp);
/* prints the given process and recursively all processes it depends on */
ശൂന്യം print_proc_recursive(ഘടന proc *pp);
ശൂന്യം printmsg(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst,
    അക്ഷരം operation, പൂർണ്ണസംഖ്യ printparams);
#എങ്കിൽ DEBUG_IPC_HOOK
ശൂന്യം hook_ipc_msgrecv(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst);
ശൂന്യം hook_ipc_msgsend(message *സന്ദേശം, ഘടന proc *src, ഘടന proc *dst);
ശൂന്യം hook_ipc_msgkcall(message *സന്ദേശം, ഘടന proc *proc);
ശൂന്യം hook_ipc_msgkresult(message *സന്ദേശം, ഘടന proc *proc);
ശൂന്യം hook_ipc_clear(ഘടന proc *proc);
#endif

/* system/do_safecopy.c */
ഘടന cp_sfinfo; /* external callers may only provide NULL */
പൂർണ്ണസംഖ്യ verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, പൂർണ്ണസംഖ്യ,
	vir_bytes, vir_bytes *, endpoint_t *, ഘടന cp_sfinfo *);

/* system/do_diagctl.c */
പൂർണ്ണസംഖ്യ do_diagctl(ഘടന proc * caller, message *m);

#എങ്കിൽ SPROFILE
/* profile.c */
ശൂന്യം init_profile_clock(u32_t);
ശൂന്യം stop_profile_clock(ശൂന്യം);
#endif

/* functions defined in architecture-dependent files. */
ശൂന്യം prot_init(ശൂന്യം);
ശൂന്യം arch_post_init(ശൂന്യം);
ശൂന്യം arch_set_secondary_ipc_return(ഘടന proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	എണ്ണം);
ശൂന്യം phys_copy_fault(ശൂന്യം);
ശൂന്യം phys_copy_fault_in_kernel(ശൂന്യം);
ശൂന്യം memset_fault(ശൂന്യം);
ശൂന്യം memset_fault_in_kernel(ശൂന്യം);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(ശൂന്യ, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
പൂർണ്ണസംഖ്യ virtual_copy_f(ഘടന proc * caller, ഘടന vir_addr *src, ഘടന
	vir_addr *dst, vir_bytes bytes, പൂർണ്ണസംഖ്യ vmcheck);
പൂർണ്ണസംഖ്യ data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
പൂർണ്ണസംഖ്യ data_copy_vmcheck(ഘടന proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(ഘടന proc* rp, പൂർണ്ണസംഖ്യ seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
പൂർണ്ണസംഖ്യ vm_memset(ഘടന proc *caller, endpoint_t who, phys_bytes dst,
	പൂർണ്ണസംഖ്യ pattern, phys_bytes എണ്ണം);
പൂർണ്ണസംഖ്യ intr_init(പൂർണ്ണസംഖ്യ);
ശൂന്യം halt_cpu(ശൂന്യം);
ശൂന്യം arch_init(ശൂന്യം);
ശൂന്യം arch_boot_proc(ഘടന boot_image *b, ഘടന proc *p);
ശൂന്യം cpu_identify(ശൂന്യം);
/* arch dependent FPU initialization per CPU */
ശൂന്യം fpu_init(ശൂന്യം);
/* returns true if pfu is present and initialized */
പൂർണ്ണസംഖ്യ is_fpu(ശൂന്യം);
ശൂന്യം ser_putc(അക്ഷരം);
__dead ശൂന്യം arch_shutdown(പൂർണ്ണസംഖ്യ);
ശൂന്യം restore_user_context(ഘടന proc * p);
ശൂന്യം read_tsc(u32_t *മുകളിലെ, u32_t *താഴെയുള്ള);
പൂർണ്ണസംഖ്യ arch_init_profile_clock(u32_t freq);
ശൂന്യം arch_stop_profile_clock(ശൂന്യം);
ശൂന്യം arch_ack_profile_clock(ശൂന്യം);
ശൂന്യം do_ser_debug(ശൂന്യം);
പൂർണ്ണസംഖ്യ arch_get_params(അക്ഷരം *parm, പൂർണ്ണസംഖ്യ max);
ശൂന്യം memory_init(ശൂന്യം);
ശൂന്യം mem_clear_mapcache(ശൂന്യം);
ശൂന്യം arch_proc_init(ഘടന proc *pr, u32_t, u32_t, u32_t, അക്ഷരം *);
പൂർണ്ണസംഖ്യ arch_do_vmctl(message *m_ptr, ഘടന proc *p);
പൂർണ്ണസംഖ്യ vm_contiguous(സ്ഥിരം ഘടന proc *targetproc, vir_bytes vir_buf,
	size_t എണ്ണം);
ശൂന്യം proc_stacktrace(ഘടന proc *proc);
പൂർണ്ണസംഖ്യ vm_lookup(സ്ഥിരം ഘടന proc *proc, vir_bytes മായാ, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(സ്ഥിരം ഘടന proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
ശൂന്യം arch_do_syscall(ഘടന proc *proc);
പൂർണ്ണസംഖ്യ arch_phys_map(പൂർണ്ണസംഖ്യ index, phys_bytes *addr, phys_bytes *len, പൂർണ്ണസംഖ്യ
	*flags);
പൂർണ്ണസംഖ്യ arch_phys_map_reply(പൂർണ്ണസംഖ്യ index, vir_bytes addr);
reg_t arch_get_sp(ഘടന proc *p);
പൂർണ്ണസംഖ്യ arch_enable_paging(ഘടന proc * caller);
പൂർണ്ണസംഖ്യ vm_check_range(ഘടന proc *caller,
       ഘടന proc *target, vir_bytes vir_addr, size_t bytes, പൂർണ്ണസംഖ്യ writable);

പൂർണ്ണസംഖ്യ copy_സന്ദേശം_from_user(message * user_mbuf, message * dst);
പൂർണ്ണസംഖ്യ copy_സന്ദേശം_to_user(message * src, message * user_mbuf);
ശൂന്യം switch_address_space(ഘടന proc * p);
ശൂന്യം release_address_space(ഘടന proc *pr);

ശൂന്യം enable_fpu_exception(ശൂന്യം);
ശൂന്യം disable_fpu_exception(ശൂന്യം);
ശൂന്യം release_fpu(ഘടന proc * p);
ശൂന്യം arch_pause(ശൂന്യം);
short cpu_load(ശൂന്യം);
ശൂന്യം busy_delay_ms(പൂർണ്ണസംഖ്യ ms);

/* utility.c */
ശൂന്യം cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
