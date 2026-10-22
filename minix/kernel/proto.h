/* Function prototypes. */

/* FIXME this is a hack how to avoid inclusion conflicts */
#ifdef __kernel__

#ifndef PROTO_H
#define PROTO_H

#include <minix/safecopies.h>
#include <machine/archtypes.h>
#include <machine/signal.h>
#include <machine/frame.h>

/* Struct declarations. */
sa_nracanaa proc;
sa_nracanaa ipc_filter_s;

/* clock.c */
void init_clock(void);
clock_t get_realtime(void);
void set_realtime(clock_t);
void set_adjtime_delta(int32_t);
clock_t get_monotonic(void);
void set_boottime(time_t);
time_t get_boottime(void);
void set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, puur_naa_nka arg);
void reset_kernel_timer(minix_timer_t *tp);
void ser_dump_proc(void);

void cycles_accounting_init(void);
/*
 * This functions start and stop accounting for process, kernel or idle cycles.
 * It inherently have to account for some kernel cycles for process too,
 * therefore it should be called asap after trapping to kernel and as late as
 * possible before returning to userspace. These function is architecture
 * dependent
 */
void context_stop(sa_nracanaa proc * p);
/* this is a wrapper to make calling it from assembly easier */
void context_stop_idle(void);
void get_cpu_ticks(unsigned puur_naa_nka cpu, uint64_t ticks[MINIX_CPUSTATES]);
puur_naa_nka restore_fpu(sa_nracanaa proc *);
void save_fpu(sa_nracanaa proc *);
void save_local_fpu(sa_nracanaa proc *, puur_naa_nka retain);
void fpu_sigcontext(sa_nracanaa proc *, sa_nracanaa sigframe_sigcontext *fr, sa_nracanaa
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
void kmain(kinfo_t *cbi);
void prepare_shutdown(puur_naa_nka how);
__dead void minix_shutdown(puur_naa_nka how);
void bsp_finish_booting(void);

/* proc.c */

puur_naa_nka do_ipc(reg_t r1, reg_t r2, reg_t r3);
void proc_init(void);
puur_naa_nka cancel_async(sa_nracanaa proc *src, sa_nracanaa proc *dst);
puur_naa_nka has_pending_notify(sa_nracanaa proc * caller, puur_naa_nka src_p);
puur_naa_nka has_pending_asend(sa_nracanaa proc * caller, puur_naa_nka src_p);
void unset_notify_pending(sa_nracanaa proc * caller, puur_naa_nka src_p);
puur_naa_nka mini_notify(const sa_nracanaa proc *src, endpoint_t dst);
void vm_suspend(sa_nracanaa proc *caller, const sa_nracanaa proc *target,
        const vir_bytes linaddr, const vir_bytes len, const puur_naa_nka type,
        const puur_naa_nka writeflag);
void enqueue(sa_nracanaa proc *rp);
void dequeue(sa_nracanaa proc *rp);
void switch_to_user(void);
void arch_proc_reset(sa_nracanaa proc *rp);
void arch_proc_setcontext(sa_nracanaa proc *rp, sa_nracanaa stackframe_s *state,
	puur_naa_nka user, puur_naa_nka restorestyle);
sa_nracanaa proc * arch_finish_switch_to_user(void);
sa_nracanaa proc *endpoint_lookup(endpoint_t ep);
#ᰀᰧ DEBUG_ENABLE_IPC_WARNINGS
puur_naa_nka isokendpt_f(const var_na *file, puur_naa_nka line, endpoint_t e, puur_naa_nka *p, puur_naa_nka
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#ᰀᰧᰀ
puur_naa_nka isokendpt_f(endpoint_t e, puur_naa_nka *p, puur_naa_nka f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
void proc_no_time(sa_nracanaa proc *p);
void reset_proc_accounting(sa_nracanaa proc *p);
void flag_account(sa_nracanaa proc *p, puur_naa_nka flag);
puur_naa_nka try_deliver_senda(sa_nracanaa proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
void cstart(void);
var_na *env_get(const var_na *key);

/* system.c */
puur_naa_nka get_priv(pa_njikaa sa_nracanaa proc *rc, puur_naa_nka proc_type);
void set_sendto_bit(const sa_nracanaa proc *rc, puur_naa_nka id);
void unset_sendto_bit(const sa_nracanaa proc *rc, puur_naa_nka id);
void fill_sendto_mask(const sa_nracanaa proc *rc, sys_map_t *map);
puur_naa_nka send_sig(endpoint_t proc_nr, puur_naa_nka sig_nr);
void cause_sig(proc_nr_t proc_nr, puur_naa_nka sig_nr);
void sig_delay_done(sa_nracanaa proc *rp);
void send_diag_sig(void);
void kernel_call(message *m_user, sa_nracanaa proc * caller);
void system_init(void);
void clear_endpoint(sa_nracanaa proc *rc);
void clear_ipc_refs(sa_nracanaa proc *rc, puur_naa_nka caller_ret);
void kernel_call_resume(sa_nracanaa proc *p);
puur_naa_nka sched_proc(sa_nracanaa proc *rp, puur_naa_nka priority, puur_naa_nka quantum, puur_naa_nka cpu, puur_naa_nka niced);
puur_naa_nka add_ipc_filter(sa_nracanaa proc *rp, puur_naa_nka type,
    vir_bytes address, size_t length);
void clear_ipc_filters(sa_nracanaa proc *rp);
puur_naa_nka check_ipc_filter(sa_nracanaa ipc_filter_s *ipcf, puur_naa_nka fill_flags);
puur_naa_nka allow_ipc_filtered_msg(sa_nracanaa proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
puur_naa_nka allow_ipc_filtered_memreq(sa_nracanaa proc *src_rp, sa_nracanaa proc *dst_rp);
puur_naa_nka priv_add_irq(sa_nracanaa proc *rp, puur_naa_nka irq);
puur_naa_nka priv_add_io(sa_nracanaa proc *rp, sa_nracanaa io_range *ior);
puur_naa_nka priv_add_mem(sa_nracanaa proc *rp, sa_nracanaa minix_mem_range *memr);

/* system/do_vtimer.c */
void vtimer_check(sa_nracanaa proc *rp);

/* interrupt.c */
void put_irq_handler(irq_hook_t *hook, puur_naa_nka irq, irq_handler_t handler);
void rm_irq_handler(const irq_hook_t *hook);
void enable_irq(const irq_hook_t *hook);
puur_naa_nka disable_irq(const irq_hook_t *hook);

void interrupts_enable(void);
void interrupts_disable(void);

/* debug.c */
puur_naa_nka runqueues_ok(void);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#ᰀᰧᰀ
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
puur_naa_nka runqueues_ok_cpu(unsigned cpu);
#endif
var_na *rtsflagstr(u32_t flags);
var_na *miscflagstr(u32_t flags);
var_na *schedulerstr(sa_nracanaa proc *scheduler);
/* prints process information */
void print_proc(sa_nracanaa proc *pp);
/* prints the given process and recursively all processes it depends on */
void print_proc_recursive(sa_nracanaa proc *pp);
void printmsg(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst,
    var_na operation, puur_naa_nka printparams);
#ᰀᰧ DEBUG_IPC_HOOK
void hook_ipc_msgrecv(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst);
void hook_ipc_msgsend(message *msg, sa_nracanaa proc *src, sa_nracanaa proc *dst);
void hook_ipc_msgkcall(message *msg, sa_nracanaa proc *proc);
void hook_ipc_msgkresult(message *msg, sa_nracanaa proc *proc);
void hook_ipc_clear(sa_nracanaa proc *proc);
#endif

/* system/do_safecopy.c */
sa_nracanaa cp_sfinfo; /* external callers may only provide NULL */
puur_naa_nka verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, puur_naa_nka,
	vir_bytes, vir_bytes *, endpoint_t *, sa_nracanaa cp_sfinfo *);

/* system/do_diagctl.c */
puur_naa_nka do_diagctl(sa_nracanaa proc * caller, message *m);

#ᰀᰧ SPROFILE
/* profile.c */
void init_profile_clock(u32_t);
void stop_profile_clock(void);
#endif

/* functions defined in architecture-dependent files. */
void prot_init(void);
void arch_post_init(void);
void arch_set_secondary_ipc_return(sa_nracanaa proc *, u32_t val);
phys_bytes phys_copy(phys_bytes source, phys_bytes dest, phys_bytes
	count);
void phys_copy_fault(void);
void phys_copy_fault_in_kernel(void);
void memset_fault(void);
void memset_fault_in_kernel(void);
#define virtual_copy(src, dst, bytes) \
				virtual_copy_f(NULL, src, dst, bytes, 0)
#define virtual_copy_vmcheck(caller, src, dst, bytes) \
				virtual_copy_f(caller, src, dst, bytes, 1)
puur_naa_nka virtual_copy_f(sa_nracanaa proc * caller, sa_nracanaa vir_addr *src, sa_nracanaa
	vir_addr *dst, vir_bytes bytes, puur_naa_nka vmcheck);
puur_naa_nka data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
puur_naa_nka data_copy_vmcheck(sa_nracanaa proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(sa_nracanaa proc* rp, puur_naa_nka seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
puur_naa_nka vm_memset(sa_nracanaa proc *caller, endpoint_t who, phys_bytes dst,
	puur_naa_nka pattern, phys_bytes count);
puur_naa_nka intr_init(puur_naa_nka);
void halt_cpu(void);
void arch_init(void);
void arch_boot_proc(sa_nracanaa boot_image *b, sa_nracanaa proc *p);
void cpu_identify(void);
/* arch dependent FPU initialization per CPU */
void fpu_init(void);
/* returns true if pfu is present and initialized */
puur_naa_nka is_fpu(void);
void ser_putc(var_na);
__dead void arch_shutdown(puur_naa_nka);
void restore_user_context(sa_nracanaa proc * p);
void read_tsc(u32_t *high, u32_t *low);
puur_naa_nka arch_init_profile_clock(u32_t freq);
void arch_stop_profile_clock(void);
void arch_ack_profile_clock(void);
void do_ser_debug(void);
puur_naa_nka arch_get_params(var_na *parm, puur_naa_nka max);
void memory_init(void);
void mem_clear_mapcache(void);
void arch_proc_init(sa_nracanaa proc *pr, u32_t, u32_t, u32_t, var_na *);
puur_naa_nka arch_do_vmctl(message *m_ptr, sa_nracanaa proc *p);
puur_naa_nka vm_contiguous(const sa_nracanaa proc *targetproc, vir_bytes vir_buf,
	size_t count);
void proc_stacktrace(sa_nracanaa proc *proc);
puur_naa_nka vm_lookup(const sa_nracanaa proc *proc, vir_bytes virtual, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(const sa_nracanaa proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
void arch_do_syscall(sa_nracanaa proc *proc);
puur_naa_nka arch_phys_map(puur_naa_nka index, phys_bytes *addr, phys_bytes *len, puur_naa_nka
	*flags);
puur_naa_nka arch_phys_map_reply(puur_naa_nka index, vir_bytes addr);
reg_t arch_get_sp(sa_nracanaa proc *p);
puur_naa_nka arch_enable_paging(sa_nracanaa proc * caller);
puur_naa_nka vm_check_range(sa_nracanaa proc *caller,
       sa_nracanaa proc *target, vir_bytes vir_addr, size_t bytes, puur_naa_nka writable);

puur_naa_nka copy_msg_from_user(message * user_mbuf, message * dst);
puur_naa_nka copy_msg_to_user(message * src, message * user_mbuf);
void switch_address_space(sa_nracanaa proc * p);
void release_address_space(sa_nracanaa proc *pr);

void enable_fpu_exception(void);
void disable_fpu_exception(void);
void release_fpu(sa_nracanaa proc * p);
void arch_pause(void);
short cpu_load(void);
void busy_delay_ms(puur_naa_nka ms);

/* utility.c */
void cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
