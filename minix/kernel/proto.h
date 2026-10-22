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
struct proc;
struct ipc_filter_s;

/* clock.c */
void init_clock(void);
clock_t get_realtime(void);
void set_realtime(clock_t);
void set_adjtime_delta(int32_t);
clock_t get_monotonic(void);
void set_boottime(time_t);
time_t get_boottime(void);
void set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, 정수 arg);
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
void context_stop(struct proc * p);
/* this is a wrapper to make calling it from assembly easier */
void context_stop_idle(void);
void get_cpu_ticks(unsigned 정수 cpu, uint64_t ticks[MINIX_CPUSTATES]);
정수 restore_fpu(struct proc *);
void save_fpu(struct proc *);
void save_local_fpu(struct proc *, 정수 retain);
void fpu_sigcontext(struct proc *, struct sigframe_sigcontext *fr, struct
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
void kmain(kinfo_t *cbi);
void prepare_shutdown(정수 how);
__dead void minix_shutdown(정수 how);
void bsp_finish_booting(void);

/* proc.c */

정수 do_ipc(reg_t r1, reg_t r2, reg_t r3);
void proc_init(void);
정수 cancel_async(struct proc *src, struct proc *dst);
정수 has_pending_notify(struct proc * caller, 정수 src_p);
정수 has_pending_asend(struct proc * caller, 정수 src_p);
void unset_notify_pending(struct proc * caller, 정수 src_p);
정수 mini_notify(const struct proc *src, endpoint_t dst);
void vm_suspend(struct proc *caller, const struct proc *target,
        const vir_bytes linaddr, const vir_bytes len, const 정수 type,
        const 정수 writeflag);
void enqueue(struct proc *rp);
void dequeue(struct proc *rp);
void switch_to_user(void);
void arch_proc_reset(struct proc *rp);
void arch_proc_setcontext(struct proc *rp, struct stackframe_s *state,
	정수 user, 정수 restorestyle);
struct proc * arch_finish_switch_to_user(void);
struct proc *endpoint_lookup(endpoint_t ep);
#만약 DEBUG_ENABLE_IPC_WARNINGS
정수 isokendpt_f(const char *file, 정수 line, endpoint_t e, 정수 *p, 정수
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#아니면
정수 isokendpt_f(endpoint_t e, 정수 *p, 정수 f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
void proc_no_time(struct proc *p);
void reset_proc_accounting(struct proc *p);
void flag_account(struct proc *p, 정수 flag);
정수 try_deliver_senda(struct proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
void cstart(void);
char *env_get(const char *key);

/* system.c */
정수 get_priv(register struct proc *rc, 정수 proc_type);
void set_sendto_bit(const struct proc *rc, 정수 id);
void unset_sendto_bit(const struct proc *rc, 정수 id);
void fill_sendto_mask(const struct proc *rc, sys_map_t *map);
정수 send_sig(endpoint_t proc_nr, 정수 sig_nr);
void cause_sig(proc_nr_t proc_nr, 정수 sig_nr);
void sig_delay_done(struct proc *rp);
void send_diag_sig(void);
void kernel_call(message *m_user, struct proc * caller);
void system_init(void);
void clear_endpoint(struct proc *rc);
void clear_ipc_refs(struct proc *rc, 정수 caller_ret);
void kernel_call_resume(struct proc *p);
정수 sched_proc(struct proc *rp, 정수 priority, 정수 quantum, 정수 cpu, 정수 niced);
정수 add_ipc_filter(struct proc *rp, 정수 type,
    vir_bytes address, size_t length);
void clear_ipc_filters(struct proc *rp);
정수 check_ipc_filter(struct ipc_filter_s *ipcf, 정수 fill_flags);
정수 allow_ipc_filtered_msg(struct proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
정수 allow_ipc_filtered_memreq(struct proc *src_rp, struct proc *dst_rp);
정수 priv_add_irq(struct proc *rp, 정수 irq);
정수 priv_add_io(struct proc *rp, struct io_range *ior);
정수 priv_add_mem(struct proc *rp, struct minix_mem_range *memr);

/* system/do_vtimer.c */
void vtimer_check(struct proc *rp);

/* interrupt.c */
void put_irq_handler(irq_hook_t *hook, 정수 irq, irq_handler_t handler);
void rm_irq_handler(const irq_hook_t *hook);
void enable_irq(const irq_hook_t *hook);
정수 disable_irq(const irq_hook_t *hook);

void interrupts_enable(void);
void interrupts_disable(void);

/* debug.c */
정수 runqueues_ok(void);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#아니면
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
정수 runqueues_ok_cpu(unsigned cpu);
#endif
char *rtsflagstr(u32_t flags);
char *miscflagstr(u32_t flags);
char *schedulerstr(struct proc *scheduler);
/* prints process information */
void print_proc(struct proc *pp);
/* prints the given process and recursively all processes it depends on */
void print_proc_recursive(struct proc *pp);
void printmsg(message *msg, struct proc *src, struct proc *dst,
    char operation, 정수 printparams);
#만약 DEBUG_IPC_HOOK
void hook_ipc_msgrecv(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgsend(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgkcall(message *msg, struct proc *proc);
void hook_ipc_msgkresult(message *msg, struct proc *proc);
void hook_ipc_clear(struct proc *proc);
#endif

/* system/do_safecopy.c */
struct cp_sfinfo; /* external callers may only provide NULL */
정수 verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, 정수,
	vir_bytes, vir_bytes *, endpoint_t *, struct cp_sfinfo *);

/* system/do_diagctl.c */
정수 do_diagctl(struct proc * caller, message *m);

#만약 SPROFILE
/* profile.c */
void init_profile_clock(u32_t);
void stop_profile_clock(void);
#endif

/* functions defined in architecture-dependent files. */
void prot_init(void);
void arch_post_init(void);
void arch_set_secondary_ipc_return(struct proc *, u32_t val);
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
정수 virtual_copy_f(struct proc * caller, struct vir_addr *src, struct
	vir_addr *dst, vir_bytes bytes, 정수 vmcheck);
정수 data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
정수 data_copy_vmcheck(struct proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(struct proc* rp, 정수 seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
정수 vm_memset(struct proc *caller, endpoint_t who, phys_bytes dst,
	정수 pattern, phys_bytes count);
정수 intr_init(정수);
void halt_cpu(void);
void arch_init(void);
void arch_boot_proc(struct boot_image *b, struct proc *p);
void cpu_identify(void);
/* arch dependent FPU initialization per CPU */
void fpu_init(void);
/* returns true if pfu is present and initialized */
정수 is_fpu(void);
void ser_putc(char);
__dead void arch_shutdown(정수);
void restore_user_context(struct proc * p);
void read_tsc(u32_t *high, u32_t *low);
정수 arch_init_profile_clock(u32_t freq);
void arch_stop_profile_clock(void);
void arch_ack_profile_clock(void);
void do_ser_debug(void);
정수 arch_get_params(char *parm, 정수 max);
void memory_init(void);
void mem_clear_mapcache(void);
void arch_proc_init(struct proc *pr, u32_t, u32_t, u32_t, char *);
정수 arch_do_vmctl(message *m_ptr, struct proc *p);
정수 vm_contiguous(const struct proc *targetproc, vir_bytes vir_buf,
	size_t count);
void proc_stacktrace(struct proc *proc);
정수 vm_lookup(const struct proc *proc, vir_bytes virtual, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(const struct proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
void arch_do_syscall(struct proc *proc);
정수 arch_phys_map(정수 index, phys_bytes *addr, phys_bytes *len, 정수
	*flags);
정수 arch_phys_map_reply(정수 index, vir_bytes addr);
reg_t arch_get_sp(struct proc *p);
정수 arch_enable_paging(struct proc * caller);
정수 vm_check_range(struct proc *caller,
       struct proc *target, vir_bytes vir_addr, size_t bytes, 정수 writable);

정수 copy_msg_from_user(message * user_mbuf, message * dst);
정수 copy_msg_to_user(message * src, message * user_mbuf);
void switch_address_space(struct proc * p);
void release_address_space(struct proc *pr);

void enable_fpu_exception(void);
void disable_fpu_exception(void);
void release_fpu(struct proc * p);
void arch_pause(void);
short cpu_load(void);
void busy_delay_ms(정수 ms);

/* utility.c */
void cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
