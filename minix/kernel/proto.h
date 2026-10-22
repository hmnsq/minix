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
void set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, цео arg);
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
void get_cpu_ticks(unsigned цео cpu, uint64_t ticks[MINIX_CPUSTATES]);
цео restore_fpu(struct proc *);
void save_fpu(struct proc *);
void save_local_fpu(struct proc *, цео retain);
void fpu_sigcontext(struct proc *, struct sigframe_sigcontext *fr, struct
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
void kmain(kinfo_t *cbi);
void prepare_shutdown(цео how);
__dead void minix_shutdown(цео how);
void bsp_finish_booting(void);

/* proc.c */

цео do_ipc(reg_t r1, reg_t r2, reg_t r3);
void proc_init(void);
цео cancel_async(struct proc *src, struct proc *dst);
цео has_pending_notify(struct proc * caller, цео src_p);
цео has_pending_asend(struct proc * caller, цео src_p);
void unset_notify_pending(struct proc * caller, цео src_p);
цео mini_notify(const struct proc *src, endpoint_t dst);
void vm_suspend(struct proc *caller, const struct proc *target,
        const vir_bytes linaddr, const vir_bytes len, const цео type,
        const цео writeflag);
void enqueue(struct proc *rp);
void dequeue(struct proc *rp);
void switch_to_user(void);
void arch_proc_reset(struct proc *rp);
void arch_proc_setcontext(struct proc *rp, struct stackframe_s *state,
	цео user, цео restorestyle);
struct proc * arch_finish_switch_to_user(void);
struct proc *endpoint_lookup(endpoint_t ep);
#ако DEBUG_ENABLE_IPC_WARNINGS
цео isokendpt_f(const char *file, цео line, endpoint_t e, цео *p, цео
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#инако
цео isokendpt_f(endpoint_t e, цео *p, цео f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
void proc_no_time(struct proc *p);
void reset_proc_accounting(struct proc *p);
void flag_account(struct proc *p, цео flag);
цео try_deliver_senda(struct proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
void cstart(void);
char *env_get(const char *key);

/* system.c */
цео get_priv(register struct proc *rc, цео proc_type);
void set_sendto_bit(const struct proc *rc, цео id);
void unset_sendto_bit(const struct proc *rc, цео id);
void fill_sendto_mask(const struct proc *rc, sys_map_t *map);
цео send_sig(endpoint_t proc_nr, цео sig_nr);
void cause_sig(proc_nr_t proc_nr, цео sig_nr);
void sig_delay_done(struct proc *rp);
void send_diag_sig(void);
void kernel_call(message *m_user, struct proc * caller);
void system_init(void);
void clear_endpoint(struct proc *rc);
void clear_ipc_refs(struct proc *rc, цео caller_ret);
void kernel_call_resume(struct proc *p);
цео sched_proc(struct proc *rp, цео priority, цео quantum, цео cpu, цео niced);
цео add_ipc_filter(struct proc *rp, цео type,
    vir_bytes address, size_t length);
void clear_ipc_filters(struct proc *rp);
цео check_ipc_filter(struct ipc_filter_s *ipcf, цео fill_flags);
цео allow_ipc_filtered_msg(struct proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
цео allow_ipc_filtered_memreq(struct proc *src_rp, struct proc *dst_rp);
цео priv_add_irq(struct proc *rp, цео irq);
цео priv_add_io(struct proc *rp, struct io_range *ior);
цео priv_add_mem(struct proc *rp, struct minix_mem_range *memr);

/* system/do_vtimer.c */
void vtimer_check(struct proc *rp);

/* interrupt.c */
void put_irq_handler(irq_hook_t *hook, цео irq, irq_handler_t handler);
void rm_irq_handler(const irq_hook_t *hook);
void enable_irq(const irq_hook_t *hook);
цео disable_irq(const irq_hook_t *hook);

void interrupts_enable(void);
void interrupts_disable(void);

/* debug.c */
цео runqueues_ok(void);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#инако
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
цео runqueues_ok_cpu(unsigned cpu);
#endif
char *rtsflagstr(u32_t flags);
char *miscflagstr(u32_t flags);
char *schedulerstr(struct proc *scheduler);
/* prints process information */
void print_proc(struct proc *pp);
/* prints the given process and recursively all processes it depends on */
void print_proc_recursive(struct proc *pp);
void printmsg(message *msg, struct proc *src, struct proc *dst,
    char operation, цео printparams);
#ако DEBUG_IPC_HOOK
void hook_ipc_msgrecv(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgsend(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgkcall(message *msg, struct proc *proc);
void hook_ipc_msgkresult(message *msg, struct proc *proc);
void hook_ipc_clear(struct proc *proc);
#endif

/* system/do_safecopy.c */
struct cp_sfinfo; /* external callers may only provide NULL */
цео verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, цео,
	vir_bytes, vir_bytes *, endpoint_t *, struct cp_sfinfo *);

/* system/do_diagctl.c */
цео do_diagctl(struct proc * caller, message *m);

#ако SPROFILE
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
цео virtual_copy_f(struct proc * caller, struct vir_addr *src, struct
	vir_addr *dst, vir_bytes bytes, цео vmcheck);
цео data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
цео data_copy_vmcheck(struct proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(struct proc* rp, цео seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
цео vm_memset(struct proc *caller, endpoint_t who, phys_bytes dst,
	цео pattern, phys_bytes count);
цео intr_init(цео);
void halt_cpu(void);
void arch_init(void);
void arch_boot_proc(struct boot_image *b, struct proc *p);
void cpu_identify(void);
/* arch dependent FPU initialization per CPU */
void fpu_init(void);
/* returns true if pfu is present and initialized */
цео is_fpu(void);
void ser_putc(char);
__dead void arch_shutdown(цео);
void restore_user_context(struct proc * p);
void read_tsc(u32_t *high, u32_t *low);
цео arch_init_profile_clock(u32_t freq);
void arch_stop_profile_clock(void);
void arch_ack_profile_clock(void);
void do_ser_debug(void);
цео arch_get_params(char *parm, цео max);
void memory_init(void);
void mem_clear_mapcache(void);
void arch_proc_init(struct proc *pr, u32_t, u32_t, u32_t, char *);
цео arch_do_vmctl(message *m_ptr, struct proc *p);
цео vm_contiguous(const struct proc *targetproc, vir_bytes vir_buf,
	size_t count);
void proc_stacktrace(struct proc *proc);
цео vm_lookup(const struct proc *proc, vir_bytes virtual, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(const struct proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
void arch_do_syscall(struct proc *proc);
цео arch_phys_map(цео index, phys_bytes *addr, phys_bytes *len, цео
	*flags);
цео arch_phys_map_reply(цео index, vir_bytes addr);
reg_t arch_get_sp(struct proc *p);
цео arch_enable_paging(struct proc * caller);
цео vm_check_range(struct proc *caller,
       struct proc *target, vir_bytes vir_addr, size_t bytes, цео writable);

цео copy_msg_from_user(message * user_mbuf, message * dst);
цео copy_msg_to_user(message * src, message * user_mbuf);
void switch_address_space(struct proc * p);
void release_address_space(struct proc *pr);

void enable_fpu_exception(void);
void disable_fpu_exception(void);
void release_fpu(struct proc * p);
void arch_pause(void);
short cpu_load(void);
void busy_delay_ms(цео ms);

/* utility.c */
void cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
