/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#నిర్వచితమైతే __kernel__

#నిర్వచితం_కాకపోతే PROTO_H
#నిర్వచించు PROTO_H

#అన్తర్భావ <minix/safecopies.h>
#అన్తర్భావ <machine/archtypes.h>
#అన్తర్భావ <machine/signal.h>
#అన్తర్భావ <machine/frame.h>

/* Struct declarations. */
నిర్మాణం proc;
నిర్మాణం ipc_filter_s;

/* clock.c */
శూన్య init_clock(శూన్య);
clock_t get_realtime(శూన్య);
శూన్య సమితి_realtime(clock_t);
శూన్య సమితి_adjtime_delta(int32_t);
clock_t get_monotonic(శూన్య);
శూన్య సమితి_boottime(time_t);
time_t get_boottime(శూన్య);
శూన్య సమితి_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, పూర్ణాంకం arg);
శూన్య reset_kernel_timer(minix_timer_t *tp);
శూన్య ser_dump_proc(శూన్య);

శూన్య చక్రాలు_accounting_init(శూన్య);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
శూన్య context_stop(నిర్మాణం proc * p);
/* this is a wrapper to make calling it from assembly easier */
శూన్య context_stop_idle(శూన్య);
శూన్య get_cpu_ticks(గుర్తులేని పూర్ణాంకం cpu, uint64_t ticks[MINIX_CPUSTATES]);
పూర్ణాంకం restore_fpu(నిర్మాణం proc *);
శూన్య save_fpu(నిర్మాణం proc *);
శూన్య save_local_fpu(నిర్మాణం proc *, పూర్ణాంకం retain);
శూన్య fpu_sigcontext(నిర్మాణం proc *, నిర్మాణం sigframe_sigcontext *fr, నిర్మాణం
	sigcontext *sc);

/* main.c */
#నిర్వచితం_కాకపోతే UNPAGED
#నిర్వచించు kmain __k_unpaged_kmain
#అయితే_ముగింపు
శూన్య kmain(kinfo_t *cbi);
శూన్య prepare_shutdown(పూర్ణాంకం how);
__dead శూన్య minix_shutdown(పూర్ణాంకం how);
శూన్య bsp_ముగింపు_booting(శూన్య);

/* proc.c */

పూర్ణాంకం do_ipc(reg_t r1, reg_t r2, reg_t r3);
శూన్య proc_init(శూన్య);
పూర్ణాంకం cancel_async(నిర్మాణం proc *src, నిర్మాణం proc *dst);
పూర్ణాంకం has_pending_notify(నిర్మాణం proc * caller, పూర్ణాంకం src_p);
పూర్ణాంకం has_pending_asend(నిర్మాణం proc * caller, పూర్ణాంకం src_p);
శూన్య unset_notify_pending(నిర్మాణం proc * caller, పూర్ణాంకం src_p);
పూర్ణాంకం mini_notify(స్థిరరాశి నిర్మాణం proc *src, endpoint_t dst);
శూన్య vm_suspend(నిర్మాణం proc *caller, స్థిరరాశి నిర్మాణం proc *లక్ష్యం,
        స్థిరరాశి vir_బైట్లు linaddr, స్థిరరాశి vir_బైట్లు len, స్థిరరాశి పూర్ణాంకం type,
        స్థిరరాశి పూర్ణాంకం writeflag);
శూన్య వరుసలో_చేర్చు(నిర్మాణం proc *rp);
శూన్య వరుస_నుండి_తీయి(నిర్మాణం proc *rp);
శూన్య switch_గమ్యం_user(శూన్య);
శూన్య arch_proc_reset(నిర్మాణం proc *rp);
శూన్య arch_proc_setcontext(నిర్మాణం proc *rp, నిర్మాణం stackframe_s *state,
	పూర్ణాంకం user, పూర్ణాంకం restorestyle);
నిర్మాణం proc * arch_ముగింపు_switch_గమ్యం_user(శూన్య);
నిర్మాణం proc *endpoint_lookup(endpoint_t ep);
#ఒకవేళ DEBUG_ENABLE_IPC_WARNINGS
పూర్ణాంకం isokendpt_f(స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, endpoint_t e, పూర్ణాంకం *p, పూర్ణాంకం
	f);
#నిర్వచించు isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#లేకపోతే
పూర్ణాంకం isokendpt_f(endpoint_t e, పూర్ణాంకం *p, పూర్ణాంకం f);
#నిర్వచించు isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#అయితే_ముగింపు
శూన్య proc_no_సమయం(నిర్మాణం proc *p);
శూన్య reset_proc_accounting(నిర్మాణం proc *p);
శూన్య flag_account(నిర్మాణం proc *p, పూర్ణాంకం flag);
పూర్ణాంకం try_deliver_senda(నిర్మాణం proc *caller_ptr, asynmsg_t *పట్టిక, size_t
	కొలత);

/* start.c */
శూన్య cstart(శూన్య);
అక్షరం *env_get(స్థిరరాశి అక్షరం *కీలకం);

/* system.c */
పూర్ణాంకం get_priv(నమోదు నిర్మాణం proc *rc, పూర్ణాంకం proc_type);
శూన్య సమితి_sendto_బిట్టు(స్థిరరాశి నిర్మాణం proc *rc, పూర్ణాంకం id);
శూన్య unset_sendto_బిట్టు(స్థిరరాశి నిర్మాణం proc *rc, పూర్ణాంకం id);
శూన్య fill_sendto_ముసుగు(స్థిరరాశి నిర్మాణం proc *rc, sys_map_t *map);
పూర్ణాంకం send_sig(endpoint_t proc_nr, పూర్ణాంకం sig_nr);
శూన్య cause_sig(proc_nr_t proc_nr, పూర్ణాంకం sig_nr);
శూన్య sig_delay_పూర్తి(నిర్మాణం proc *rp);
శూన్య send_diag_sig(శూన్య);
శూన్య kernel_call(సందేశం *m_user, నిర్మాణం proc * caller);
శూన్య system_init(శూన్య);
శూన్య clear_endpoint(నిర్మాణం proc *rc);
శూన్య clear_ipc_refs(నిర్మాణం proc *rc, పూర్ణాంకం caller_ret);
శూన్య kernel_call_resume(నిర్మాణం proc *p);
పూర్ణాంకం sched_proc(నిర్మాణం proc *rp, పూర్ణాంకం ప్రాధాన్యత, పూర్ణాంకం కాలాంశం, పూర్ణాంకం cpu, పూర్ణాంకం niced);
పూర్ణాంకం కూడు_ipc_filter(నిర్మాణం proc *rp, పూర్ణాంకం type,
    vir_బైట్లు చిరునామా, size_t నిడివి);
శూన్య clear_ipc_filters(నిర్మాణం proc *rp);
పూర్ణాంకం check_ipc_filter(నిర్మాణం ipc_filter_s *ipcf, పూర్ణాంకం fill_flags);
పూర్ణాంకం allow_ipc_filtered_సందేశం(నిర్మాణం proc *rp, endpoint_t src_e,
    vir_బైట్లు m_src_v, సందేశం *m_src_p);
పూర్ణాంకం allow_ipc_filtered_memreq(నిర్మాణం proc *src_rp, నిర్మాణం proc *dst_rp);
పూర్ణాంకం priv_కూడు_irq(నిర్మాణం proc *rp, పూర్ణాంకం irq);
పూర్ణాంకం priv_కూడు_io(నిర్మాణం proc *rp, నిర్మాణం io_range *ior);
పూర్ణాంకం priv_కూడు_mem(నిర్మాణం proc *rp, నిర్మాణం minix_mem_range *memr);

/* system/do_vtimer.c */
శూన్య vtimer_check(నిర్మాణం proc *rp);

/* interrupt.c */
శూన్య put_irq_handler(irq_hook_t *hook, పూర్ణాంకం irq, irq_handler_t handler);
శూన్య rm_irq_handler(స్థిరరాశి irq_hook_t *hook);
శూన్య enable_irq(స్థిరరాశి irq_hook_t *hook);
పూర్ణాంకం disable_irq(స్థిరరాశి irq_hook_t *hook);

శూన్య interrupts_enable(శూన్య);
శూన్య interrupts_disable(శూన్య);

/* debug.c */
పూర్ణాంకం runqueues_ok(శూన్య);
#నిర్వచితం_కాకపోతే CONFIG_SMP
#నిర్వచించు runqueues_ok_local runqueues_ok
#లేకపోతే
#నిర్వచించు runqueues_ok_local() runqueues_ok_cpu(cpuid)
పూర్ణాంకం runqueues_ok_cpu(గుర్తులేని cpu);
#అయితే_ముగింపు
అక్షరం *rtsflagstr(u32_t flags);
అక్షరం *miscflagstr(u32_t flags);
అక్షరం *schedulerstr(నిర్మాణం proc *scheduler);
/* prints process information */
శూన్య print_proc(నిర్మాణం proc *pp);
/* prints the given process and recursively all processes it depends on */
శూన్య print_proc_recursive(నిర్మాణం proc *pp);
శూన్య printmsg(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst,
    అక్షరం operation, పూర్ణాంకం printparams);
#ఒకవేళ DEBUG_IPC_HOOK
శూన్య hook_ipc_msgrecv(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst);
శూన్య hook_ipc_msgsend(సందేశం *సందేశం_2, నిర్మాణం proc *src, నిర్మాణం proc *dst);
శూన్య hook_ipc_msgkcall(సందేశం *సందేశం_2, నిర్మాణం proc *proc);
శూన్య hook_ipc_msgkresult(సందేశం *సందేశం_2, నిర్మాణం proc *proc);
శూన్య hook_ipc_clear(నిర్మాణం proc *proc);
#అయితే_ముగింపు

/* system/do_safecopy.c */
నిర్మాణం cp_sfinfo; /* external callers may only provide NULL */
పూర్ణాంకం verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_బైట్లు, పూర్ణాంకం,
	vir_బైట్లు, vir_బైట్లు *, endpoint_t *, నిర్మాణం cp_sfinfo *);

/* system/do_diagctl.c */
పూర్ణాంకం do_diagctl(నిర్మాణం proc * caller, సందేశం *m);

#ఒకవేళ SPROFILE
/* profile.c */
శూన్య init_profile_clock(u32_t);
శూన్య stop_profile_clock(శూన్య);
#అయితే_ముగింపు

/* functions defined in architecture-dependent files. */
శూన్య prot_init(శూన్య);
శూన్య arch_post_init(శూన్య);
శూన్య arch_సమితి_secondary_ipc_return(నిర్మాణం proc *, u32_t val);
phys_బైట్లు phys_copy(phys_బైట్లు ఆధారం, phys_బైట్లు dest, phys_బైట్లు
	లెక్క);
శూన్య phys_copy_fault(శూన్య);
శూన్య phys_copy_fault_లోపల_kernel(శూన్య);
శూన్య memset_fault(శూన్య);
శూన్య memset_fault_లోపల_kernel(శూన్య);
#నిర్వచించు virtual_copy(src, dst, బైట్లు) \
				virtual_copy_f(ఖాళీ, src, dst, బైట్లు, 0)
#నిర్వచించు virtual_copy_vmcheck(caller, src, dst, బైట్లు) \
				virtual_copy_f(caller, src, dst, బైట్లు, 1)
పూర్ణాంకం virtual_copy_f(నిర్మాణం proc * caller, నిర్మాణం vir_addr *src, నిర్మాణం
	vir_addr *dst, vir_బైట్లు బైట్లు, పూర్ణాంకం vmcheck);
పూర్ణాంకం దత్తాంశం_copy(endpoint_t from, vir_బైట్లు from_addr, endpoint_t గమ్యం,
	vir_బైట్లు గమ్యం_addr, size_t బైట్లు);
పూర్ణాంకం దత్తాంశం_copy_vmcheck(నిర్మాణం proc *, endpoint_t from, vir_బైట్లు
	from_addr, endpoint_t గమ్యం, vir_బైట్లు గమ్యం_addr, size_t బైట్లు);
phys_బైట్లు umap_virtual(నిర్మాణం proc* rp, పూర్ణాంకం seg, vir_బైట్లు vir_addr,
	vir_బైట్లు బైట్లు);
phys_బైట్లు seg2phys(u16_t);
పూర్ణాంకం vm_memset(నిర్మాణం proc *caller, endpoint_t who, phys_బైట్లు dst,
	పూర్ణాంకం నమూనా, phys_బైట్లు లెక్క);
పూర్ణాంకం intr_init(పూర్ణాంకం);
శూన్య halt_cpu(శూన్య);
శూన్య arch_init(శూన్య);
శూన్య arch_boot_proc(నిర్మాణం boot_image *b, నిర్మాణం proc *p);
శూన్య cpu_identify(శూన్య);
/* arch dependent FPU initialization per CPU */
శూన్య fpu_init(శూన్య);
/* returns true if pfu is present and initialized */
పూర్ణాంకం is_fpu(శూన్య);
శూన్య ser_putc(అక్షరం);
__dead శూన్య arch_shutdown(పూర్ణాంకం);
శూన్య restore_user_context(నిర్మాణం proc * p);
శూన్య read_tsc(u32_t *ఎగువ, u32_t *దిగువ);
పూర్ణాంకం arch_init_profile_clock(u32_t freq);
శూన్య arch_stop_profile_clock(శూన్య);
శూన్య arch_ack_profile_clock(శూన్య);
శూన్య do_ser_debug(శూన్య);
పూర్ణాంకం arch_get_params(అక్షరం *parm, పూర్ణాంకం max);
శూన్య జ్ఞాపకం_init(శూన్య);
శూన్య mem_clear_mapcache(శూన్య);
శూన్య arch_proc_init(నిర్మాణం proc *pr, u32_t, u32_t, u32_t, అక్షరం *);
పూర్ణాంకం arch_do_vmctl(సందేశం *m_ptr, నిర్మాణం proc *p);
పూర్ణాంకం vm_contiguous(స్థిరరాశి నిర్మాణం proc *targetproc, vir_బైట్లు vir_buf,
	size_t లెక్క);
శూన్య proc_stacktrace(నిర్మాణం proc *proc);
పూర్ణాంకం vm_lookup(స్థిరరాశి నిర్మాణం proc *proc, vir_బైట్లు మాయా, phys_బైట్లు
	*ఫలితం, u32_t *ptent);
size_t vm_lookup_range(స్థిరరాశి నిర్మాణం proc *proc,
       vir_బైట్లు vir_addr, phys_బైట్లు *phys_addr, size_t బైట్లు);
శూన్య arch_do_syscall(నిర్మాణం proc *proc);
పూర్ణాంకం arch_phys_map(పూర్ణాంకం సూచీ, phys_బైట్లు *addr, phys_బైట్లు *len, పూర్ణాంకం
	*flags);
పూర్ణాంకం arch_phys_map_జవాబు(పూర్ణాంకం సూచీ, vir_బైట్లు addr);
reg_t arch_get_sp(నిర్మాణం proc *p);
పూర్ణాంకం arch_enable_paging(నిర్మాణం proc * caller);
పూర్ణాంకం vm_check_range(నిర్మాణం proc *caller,
       నిర్మాణం proc *లక్ష్యం, vir_బైట్లు vir_addr, size_t బైట్లు, పూర్ణాంకం writable);

పూర్ణాంకం copy_సందేశం_from_user(సందేశం * user_mbuf, సందేశం * dst);
పూర్ణాంకం copy_సందేశం_గమ్యం_user(సందేశం * src, సందేశం * user_mbuf);
శూన్య switch_చిరునామా_space(నిర్మాణం proc * p);
శూన్య release_చిరునామా_space(నిర్మాణం proc *pr);

శూన్య enable_fpu_exception(శూన్య);
శూన్య disable_fpu_exception(శూన్య);
శూన్య release_fpu(నిర్మాణం proc * p);
శూన్య arch_pause(శూన్య);
పొట్టి cpu_ఎక్కించు(శూన్య);
శూన్య busy_delay_ms(పూర్ణాంకం ms);

/* utility.c */
శూన్య cpu_print_freq(గుర్తులేని cpu);
#అయితే_ముగింపు /* __kernel__ */

#అయితే_ముగింపు /* PROTO_H */
