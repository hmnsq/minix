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
void set_kernel_timer(minix_timer_t *tp, clock_t t, tmr_func_t f, số_nguyên arg);
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
void get_cpu_ticks(unsigned số_nguyên cpu, uint64_t ticks[MINIX_CPUSTATES]);
số_nguyên restore_fpu(struct proc *);
void save_fpu(struct proc *);
void save_local_fpu(struct proc *, số_nguyên retain);
void fpu_sigcontext(struct proc *, struct sigframe_sigcontext *fr, struct
	sigcontext *sc);

/* main.c */
#ifndef UNPAGED
#define kmain __k_unpaged_kmain
#endif
void kmain(kinfo_t *cbi);
void prepare_shutdown(số_nguyên how);
__dead void minix_shutdown(số_nguyên how);
void bsp_finish_booting(void);

/* proc.c */

số_nguyên do_ipc(reg_t r1, reg_t r2, reg_t r3);
void proc_init(void);
số_nguyên cancel_async(struct proc *src, struct proc *dst);
số_nguyên has_pending_notify(struct proc * caller, số_nguyên src_p);
số_nguyên has_pending_asend(struct proc * caller, số_nguyên src_p);
void unset_notify_pending(struct proc * caller, số_nguyên src_p);
số_nguyên mini_notify(const struct proc *src, endpoint_t dst);
void vm_suspend(struct proc *caller, const struct proc *target,
        const vir_bytes linaddr, const vir_bytes len, const số_nguyên type,
        const số_nguyên writeflag);
void enqueue(struct proc *rp);
void dequeue(struct proc *rp);
void switch_to_user(void);
void arch_proc_reset(struct proc *rp);
void arch_proc_setcontext(struct proc *rp, struct stackframe_s *state,
	số_nguyên user, số_nguyên restorestyle);
struct proc * arch_finish_switch_to_user(void);
struct proc *endpoint_lookup(endpoint_t ep);
#nếu DEBUG_ENABLE_IPC_WARNINGS
số_nguyên isokendpt_f(const char *file, số_nguyên line, endpoint_t e, số_nguyên *p, số_nguyên
	f);
#define isokendpt_d(e, p, f) isokendpt_f(__FILE__, __LINE__, (e), (p), (f))
#không_thì
số_nguyên isokendpt_f(endpoint_t e, số_nguyên *p, số_nguyên f);
#define isokendpt_d(e, p, f) isokendpt_f((e), (p), (f))
#endif
void proc_no_time(struct proc *p);
void reset_proc_accounting(struct proc *p);
void flag_account(struct proc *p, số_nguyên flag);
số_nguyên try_deliver_senda(struct proc *caller_ptr, asynmsg_t *table, size_t
	size);

/* start.c */
void cstart(void);
char *env_get(const char *key);

/* system.c */
số_nguyên get_priv(register struct proc *rc, số_nguyên proc_type);
void set_sendto_bit(const struct proc *rc, số_nguyên id);
void unset_sendto_bit(const struct proc *rc, số_nguyên id);
void fill_sendto_mask(const struct proc *rc, sys_map_t *map);
số_nguyên send_sig(endpoint_t proc_nr, số_nguyên sig_nr);
void cause_sig(proc_nr_t proc_nr, số_nguyên sig_nr);
void sig_delay_done(struct proc *rp);
void send_diag_sig(void);
void kernel_call(message *m_user, struct proc * caller);
void system_init(void);
void clear_endpoint(struct proc *rc);
void clear_ipc_refs(struct proc *rc, số_nguyên caller_ret);
void kernel_call_resume(struct proc *p);
số_nguyên sched_proc(struct proc *rp, số_nguyên priority, số_nguyên quantum, số_nguyên cpu, số_nguyên niced);
số_nguyên add_ipc_filter(struct proc *rp, số_nguyên type,
    vir_bytes address, size_t length);
void clear_ipc_filters(struct proc *rp);
số_nguyên check_ipc_filter(struct ipc_filter_s *ipcf, số_nguyên fill_flags);
số_nguyên allow_ipc_filtered_msg(struct proc *rp, endpoint_t src_e,
    vir_bytes m_src_v, message *m_src_p);
số_nguyên allow_ipc_filtered_memreq(struct proc *src_rp, struct proc *dst_rp);
số_nguyên priv_add_irq(struct proc *rp, số_nguyên irq);
số_nguyên priv_add_io(struct proc *rp, struct io_range *ior);
số_nguyên priv_add_mem(struct proc *rp, struct minix_mem_range *memr);

/* system/do_vtimer.c */
void vtimer_check(struct proc *rp);

/* interrupt.c */
void put_irq_handler(irq_hook_t *hook, số_nguyên irq, irq_handler_t handler);
void rm_irq_handler(const irq_hook_t *hook);
void enable_irq(const irq_hook_t *hook);
số_nguyên disable_irq(const irq_hook_t *hook);

void interrupts_enable(void);
void interrupts_disable(void);

/* debug.c */
số_nguyên runqueues_ok(void);
#ifndef CONFIG_SMP
#define runqueues_ok_local runqueues_ok
#không_thì
#define runqueues_ok_local() runqueues_ok_cpu(cpuid)
số_nguyên runqueues_ok_cpu(unsigned cpu);
#endif
char *rtsflagstr(u32_t flags);
char *miscflagstr(u32_t flags);
char *schedulerstr(struct proc *scheduler);
/* prints process information */
void print_proc(struct proc *pp);
/* prints the given process and recursively all processes it depends on */
void print_proc_recursive(struct proc *pp);
void printmsg(message *msg, struct proc *src, struct proc *dst,
    char operation, số_nguyên printparams);
#nếu DEBUG_IPC_HOOK
void hook_ipc_msgrecv(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgsend(message *msg, struct proc *src, struct proc *dst);
void hook_ipc_msgkcall(message *msg, struct proc *proc);
void hook_ipc_msgkresult(message *msg, struct proc *proc);
void hook_ipc_clear(struct proc *proc);
#endif

/* system/do_safecopy.c */
struct cp_sfinfo; /* external callers may only provide NULL */
số_nguyên verify_grant(endpoint_t, endpoint_t, cp_grant_id_t, vir_bytes, số_nguyên,
	vir_bytes, vir_bytes *, endpoint_t *, struct cp_sfinfo *);

/* system/do_diagctl.c */
số_nguyên do_diagctl(struct proc * caller, message *m);

#nếu SPROFILE
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
số_nguyên virtual_copy_f(struct proc * caller, struct vir_addr *src, struct
	vir_addr *dst, vir_bytes bytes, số_nguyên vmcheck);
số_nguyên data_copy(endpoint_t from, vir_bytes from_addr, endpoint_t to,
	vir_bytes to_addr, size_t bytes);
số_nguyên data_copy_vmcheck(struct proc *, endpoint_t from, vir_bytes
	from_addr, endpoint_t to, vir_bytes to_addr, size_t bytes);
phys_bytes umap_virtual(struct proc* rp, số_nguyên seg, vir_bytes vir_addr,
	vir_bytes bytes);
phys_bytes seg2phys(u16_t);
số_nguyên vm_memset(struct proc *caller, endpoint_t who, phys_bytes dst,
	số_nguyên pattern, phys_bytes count);
số_nguyên intr_init(số_nguyên);
void halt_cpu(void);
void arch_init(void);
void arch_boot_proc(struct boot_image *b, struct proc *p);
void cpu_identify(void);
/* arch dependent FPU initialization per CPU */
void fpu_init(void);
/* returns true if pfu is present and initialized */
số_nguyên is_fpu(void);
void ser_putc(char);
__dead void arch_shutdown(số_nguyên);
void restore_user_context(struct proc * p);
void read_tsc(u32_t *high, u32_t *low);
số_nguyên arch_init_profile_clock(u32_t freq);
void arch_stop_profile_clock(void);
void arch_ack_profile_clock(void);
void do_ser_debug(void);
số_nguyên arch_get_params(char *parm, số_nguyên max);
void memory_init(void);
void mem_clear_mapcache(void);
void arch_proc_init(struct proc *pr, u32_t, u32_t, u32_t, char *);
số_nguyên arch_do_vmctl(message *m_ptr, struct proc *p);
số_nguyên vm_contiguous(const struct proc *targetproc, vir_bytes vir_buf,
	size_t count);
void proc_stacktrace(struct proc *proc);
số_nguyên vm_lookup(const struct proc *proc, vir_bytes virtual, phys_bytes
	*result, u32_t *ptent);
size_t vm_lookup_range(const struct proc *proc,
       vir_bytes vir_addr, phys_bytes *phys_addr, size_t bytes);
void arch_do_syscall(struct proc *proc);
số_nguyên arch_phys_map(số_nguyên index, phys_bytes *addr, phys_bytes *len, số_nguyên
	*flags);
số_nguyên arch_phys_map_reply(số_nguyên index, vir_bytes addr);
reg_t arch_get_sp(struct proc *p);
số_nguyên arch_enable_paging(struct proc * caller);
số_nguyên vm_check_range(struct proc *caller,
       struct proc *target, vir_bytes vir_addr, size_t bytes, số_nguyên writable);

số_nguyên copy_msg_from_user(message * user_mbuf, message * dst);
số_nguyên copy_msg_to_user(message * src, message * user_mbuf);
void switch_address_space(struct proc * p);
void release_address_space(struct proc *pr);

void enable_fpu_exception(void);
void disable_fpu_exception(void);
void release_fpu(struct proc * p);
void arch_pause(void);
short cpu_load(void);
void busy_delay_ms(số_nguyên ms);

/* utility.c */
void cpu_print_freq(unsigned cpu);
#endif /* __kernel__ */

#endif /* PROTO_H */
