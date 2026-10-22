/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#अगर_घोषित __kernel__

#अगर_अघोषित PROTO_H
#घोषणा PROTO_H

#समावेश <minix/safecopies.h>
#समावेश <machine/archtypes.h>
#समावेश <machine/signal.h>
#समावेश <machine/frame.h>

/* Struct declarations. */
काष्ठा proc;
काष्ठा ipc_filter_s;

/* clock.c */
व्योम init_clock(व्योम);
घड़ी_प्रकार get_realtime(व्योम);
व्योम set_realtime(घड़ी_प्रकार);
व्योम set_adjtime_delta(int32_t);
घड़ी_प्रकार get_monotonic(व्योम);
व्योम set_boottime(समय_प्रकार);
समय_प्रकार get_boottime(व्योम);
व्योम set_kernel_timer(minix_timer_t *tp, घड़ी_प्रकार t, tmr_func_t f, पूर्णांक arg);
व्योम reset_kernel_timer(minix_timer_t *tp);
व्योम ser_dump_proc(व्योम);

व्योम cycles_accounting_init(व्योम);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
व्योम context_stop(काष्ठा proc * p);
/* this is a wrapper to make calling it from assembly easier */
व्योम context_stop_idle(व्योम);
व्योम get_cpu_ticks(अचिन्हित पूर्णांक cpu, uint64_t ticks[MINIX_CPUSTATES]);
पूर्णांक restore_fpu(काष्ठा proc *);
व्योम save_fpu(काष्ठा proc *);
व्योम save_local_fpu(काष्ठा proc *, पूर्णांक retain);
व्योम fpu_sigcontext(काष्ठा proc *, काष्ठा sigframe_sigcontext *fr, काष्ठा
	sigcontext *sc);

/* main.c */
#अगर_अघोषित UNPAGED
#घोषणा kmain __k_unpaged_kmain
#पूर्ण_अगर
व्योम kmain(kinfo_t *cbi);
व्योम prepare_shutdown(पूर्णांक how);
__dead व्योम minix_shutdown(पूर्णांक how);
व्योम bsp_finish_booting(व्योम);

/* proc.c */

पूर्णांक do_ipc(reg_t r1, reg_t r2, reg_t r3);
व्योम proc_init(व्योम);
पूर्णांक cancel_async(काष्ठा proc *src, काष्ठा proc *dst);
पूर्णांक has_pending_notify(काष्ठा proc * caller, पूर्णांक src_p);
पूर्णांक has_pending_asend(काष्ठा proc * caller, पूर्णांक src_p);
व्योम unset_notify_pending(काष्ठा proc * caller, पूर्णांक src_p);
पूर्णांक mini_notify(स्थिर काष्ठा proc *src, endpoint_t dst);
व्योम vm_suspend(काष्ठा proc *caller, स्थिर काष्ठा proc *target,
        स्थिर vir_bytes linaddr, स्थिर vir_bytes len, स्थिर पूर्णांक type,
        स्थिर पूर्णांक writeflag);
व्योम enqueue(काष्ठा proc *rp);
व्योम dequeue(काष्ठा proc *rp);
व्योम switch_to_user(व्योम);
व्योम arch_proc_reset(काष्ठा proc *rp);
व्योम arch_proc_setcontext(काष्ठा proc *rp, काष्ठा stackframe_s *state,
	पूर्णांक user, पूर्णांक restorestyle);
काष्ठा proc * arch_finish_switch_to_user(व्योम);
काष्ठा proc *endpoint_lookup(endpoint_t ep);
#अगर DEBUG_ENABLE_IPC_WARNINGS
पूर्णांक isokendpt_f(स्थिर अक्षर *file, पूर्णांक पंक्ति_2, endpoint_t e, पूर्णांक *p, पूर्णांक
	f);
#घोषणा isokendpt_d(e, p, f) isokendpt_f(__खाता__, __LINE__, (e), (p), (f))
#अन्यथा
पूर्णांक isokendpt_f(endpoint_t e, पूर्णांक *p, पूर्णांक f);
#घोषणा isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#पूर्ण_अगर
व्योम proc_no_time(काष्ठा proc *p);
व्योम reset_proc_accounting(काष्ठा proc *p);
व्योम flag_account(काष्ठा proc *p, पूर्णांक flag);
पूर्णांक try_deliver_senda(काष्ठा proc *caller_ptr, asynmsg_t *table, माप_प्रकार
	आकार);

/* start.c */
व्योम cstart(व्योम);
अक्षर *env_get(स्थिर अक्षर *कुंजी);

/* system.c */
पूर्णांक get_priv(रेजिस्टर काष्ठा proc *rc, पूर्णांक proc_type);
व्योम set_sendto_bit(स्थिर काष्ठा proc *rc, पूर्णांक id);
व्योम unset_sendto_bit(स्थिर काष्ठा proc *rc, पूर्णांक id);
व्योम fill_sendto_mask(स्थिर काष्ठा proc *rc, sys_map_t *map);
पूर्णांक send_sig(endpoint_t proc_nr, पूर्णांक sig_nr);
व्योम cause_sig(proc_nr_t proc_nr, पूर्णांक sig_nr);
व्योम sig_delay_done(काष्ठा proc *rp);
व्योम send_diag_sig(व्योम);
व्योम kernel_call(message *m_user, काष्ठा proc * caller);
व्योम system_init(व्योम);
व्योम clear_endpoint(काष्ठा proc *rc);
व्योम clear_ipc_refs(काष्ठा proc *rc, पूर्णांक caller_ret);
व्योम kernel_call_resume(काष्ठा proc *p);
पूर्णांक sched_proc(काष्ठा proc *rp, पूर्णांक priority, पूर्णांक quantum, पूर्णांक cpu, पूर्णांक niced);
पूर्णांक add_ipc_filter(काष्ठा proc *rp, पूर्णांक type,
    vir_bytes address, माप_प्रकार लंबाई);
व्योम clear_ipc_filters(काष्ठा proc *rp);
पूर्णांक check_ipc_filter(काष्ठा ipc_filter_s *ipcf, पूर्णांक fill_flags);
पूर्णांक allow_ipc_filtered_संदेश(काष्ठा proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
पूर्णांक allow_ipc_filtered_memreq(काष्ठा proc *src_rp, काष्ठा proc *dst_rp);
पूर्णांक priv_add_irq(काष्ठा proc *rp, पूर्णांक irq);
पूर्णांक priv_add_io(काष्ठा proc *rp, काष्ठा io_range *ior);
पूर्णांक priv_add_mem(काष्ठा proc *rp, काष्ठा minix_mem_range *memr);

/* system/do_vtimer.c */
व्योम vtimer_check(काष्ठा proc *rp);

/* interrupt.c */
व्योम put_irq_handler(irq_hook_t *hook, पूर्णांक irq, irq_handler_t handler);
व्योम rm_irq_handler(स्थिर irq_hook_t *hook);
व्योम enable_irq(स्थिर irq_hook_t *hook);
पूर्णांक disable_irq(स्थिर irq_hook_t *hook);

व्योम interrupts_enable(व्योम);
व्योम interrupts_disable(व्योम);

/* debug.c */
पूर्णांक runqueues_ok(व्योम);
#अगर_अघोषित CONFIG_SMP
#घोषणा runqueues_ok_local runqueues_ok
#अन्यथा
#घोषणा runqueues_ok_local() runqueues_ok_cpu(cpuid)
पूर्णांक runqueues_ok_cpu(अचिन्हित cpu);
#पूर्ण_अगर
अक्षर *rtsflagstr(u32_t flags);
अक्षर *miscflagstr(u32_t flags);
अक्षर *schedulerstr(काष्ठा proc *scheduler);
/* prints process information */
व्योम print_proc(काष्ठा proc *pp);
/* prints the given process and recursively all processes it depends on */
व्योम print_proc_recursive(काष्ठा proc *pp);
व्योम printmsg(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst,
    अक्षर operation, पूर्णांक printparams);
#अगर DEBUG_IPC_HOOK
व्योम hook_ipc_msgrecv(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst);
व्योम hook_ipc_msgsend(message *संदेश, काष्ठा proc *src, काष्ठा proc *dst);
व्योम hook_ipc_msgkcall(message *संदेश, काष्ठा proc *proc);
व्योम hook_ipc_msgkresult(message *संदेश, काष्ठा proc *proc);
व्योम hook_ipc_clear(काष्ठा proc *proc);
#पूर्ण_अगर

/* system/do_safecopy.c */
काष्ठा cp_sfinfo; /* external callers may only provide NULL */
पूर्णांक verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, पूर्णांक,
	vir_bytes, vir_bytes *, endpoint_t *, काष्ठा cp_sfinfo *);

/* system/do_diagctl.c */
पूर्णांक do_diagctl(काष्ठा proc * caller, message *m);

#अगर SPROFILE
/* profile.c */
व्योम init_profile_clock(u32_t);
व्योम stop_profile_clock(व्योम);
#पूर्ण_अगर

/* functions defined in architecture-dependent files. */
व्योम prot_init(व्योम);
व्योम arch_post_init(व्योम);
व्योम arch_set_secondary_ipc_return(काष्ठा proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	गिनती);
व्योम phys_copy_fault(व्योम);
व्योम phys_copy_fault_in_kernel(व्योम);
व्योम memset_fault(व्योम);
व्योम memset_fault_in_kernel(व्योम);
#घोषणा virtual_copy(src, dst, bytes) \
				virtual_copy_f(शून्य, src, dst, bytes, 0)
#घोषणा virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
पूर्णांक virtual_copy_f(काष्ठा proc * caller, काष्ठा vir_addr *src, काष्ठा
	vir_addr *dst, vir_bytes bytes, पूर्णांक vmcheck);
पूर्णांक तथ्य_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, माप_प्रकार bytes);
पूर्णांक तथ्य_copy_vmcheck(काष्ठा proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, माप_प्रकार bytes);
phys_bytes umap_virtual(काष्ठा proc* rp, पूर्णांक seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
पूर्णांक vm_memset(काष्ठा proc *caller, endpoint_t who, phys_bytes dst,
	पूर्णांक pattern, phys_bytes गिनती);
पूर्णांक intr_init(पूर्णांक);
व्योम halt_cpu(व्योम);
व्योम arch_init(व्योम);
व्योम arch_boot_proc(काष्ठा boot_image *b, काष्ठा proc *p);
व्योम cpu_identify(व्योम);
/* arch dependent FPU initialization per CPU */
व्योम fpu_init(व्योम);
/* returns true if pfu is present and initialized */
पूर्णांक is_fpu(व्योम);
व्योम ser_putc(अक्षर);
__dead व्योम arch_shutdown(पूर्णांक);
व्योम restore_user_context(काष्ठा proc * p);
व्योम read_tsc(u32_t *ऊपरी, u32_t *निचला);
पूर्णांक arch_init_profile_clock(u32_t freq);
व्योम arch_stop_profile_clock(व्योम);
व्योम arch_ack_profile_clock(व्योम);
व्योम do_ser_debug(व्योम);
पूर्णांक arch_get_params(अक्षर *parm, पूर्णांक max);
व्योम memory_init(व्योम);
व्योम mem_clear_mapcache(व्योम);
व्योम arch_proc_init(काष्ठा proc *pr, u32_t, u32_t, u32_t, अक्षर *);
पूर्णांक arch_do_vmctl(message *m_ptr, काष्ठा proc *p);
पूर्णांक vm_contiguous(स्थिर काष्ठा proc *targetproc, vir_bytes vir_buf,
	माप_प्रकार गिनती);
व्योम proc_stacktrace(काष्ठा proc *proc);
पूर्णांक vm_lookup(स्थिर काष्ठा proc *proc, vir_bytes भव, phys_bytes
	*परिणाम, u32_t *ptent);
माप_प्रकार vm_lookup_range(स्थिर काष्ठा proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, माप_प्रकार bytes);
व्योम arch_do_syscall(काष्ठा proc *proc);
पूर्णांक arch_phys_map(पूर्णांक सूचकांक, phys_bytes *addr, phys_bytes *len, पूर्णांक
	*flags);
पूर्णांक arch_phys_map_reply(पूर्णांक सूचकांक, vir_bytes addr);
reg_t arch_get_sp(काष्ठा proc *p);
पूर्णांक arch_enable_paging(काष्ठा proc * caller);
पूर्णांक vm_check_range(काष्ठा proc *caller,
       काष्ठा proc *target, vir_bytes vir_addr, माप_प्रकार bytes, पूर्णांक writable);

पूर्णांक copy_संदेश_from_user(message * user_mbuf, message * dst);
पूर्णांक copy_संदेश_to_user(message * src, message * user_mbuf);
व्योम switch_address_space(काष्ठा proc * p);
व्योम release_address_space(काष्ठा proc *pr);

व्योम enable_fpu_exception(व्योम);
व्योम disable_fpu_exception(व्योम);
व्योम release_fpu(काष्ठा proc * p);
व्योम arch_pause(व्योम);
लघु cpu_load(व्योम);
व्योम busy_delay_ms(पूर्णांक ms);

/* utility.c */
व्योम cpu_print_freq(अचिन्हित cpu);
#पूर्ण_अगर /* __kernel__ */

#पूर्ण_अगर /* PROTO_H */
