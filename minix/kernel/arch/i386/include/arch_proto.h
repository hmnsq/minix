
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#شامل <machine/vm.h>

#define K_پشته_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
تهی hwint00(تهی);
تهی hwint01(تهی);
تهی hwint02(تهی);
تهی hwint03(تهی);
تهی hwint04(تهی);
تهی hwint05(تهی);
تهی hwint06(تهی);
تهی hwint07(تهی);
تهی hwint08(تهی);
تهی hwint09(تهی);
تهی hwint10(تهی);
تهی hwint11(تهی);
تهی hwint12(تهی);
تهی hwint13(تهی);
تهی hwint14(تهی);
تهی hwint15(تهی);

/* Exception handlers (real or protected mode), in numerical order. */
تهی int00(تهی), divide_error (تهی);
تهی int01(تهی), single_step_exception (تهی);
تهی int02(تهی), nmi (تهی);
تهی int03(تهی), breakpoint_exception (تهی);
تهی int04(تهی), overflow (تهی);
تهی int05(تهی), bounds_check (تهی);
تهی int06(تهی), inval_opcode (تهی);
تهی int07(تهی), copr_not_available (تهی);
تهی double_fault(تهی);
تهی copr_seg_overrun(تهی);
تهی inval_tss(تهی);
تهی segment_not_present(تهی);
تهی پشته_exception(تهی);
تهی general_protection(تهی);
تهی page_fault(تهی);
تهی copr_error(تهی);
تهی alignment_check(تهی);
تهی machine_check(تهی);
تهی simd_exception(تهی);

تهی restore_user_context_int(ساختار proc *);
تهی restore_user_context_sysenter(ساختار proc *);
تهی restore_user_context_syscall(ساختار proc *);

/* Software interrupt handlers, in numerical order. */
تهی trp(تهی);
تهی ipc_entry_softint_orig(تهی);
تهی ipc_entry_softint_um(تهی);
تهی ipc_entry_sysenter(تهی);
تهی ipc_entry_syscall_cpu0(تهی);
تهی ipc_entry_syscall_cpu1(تهی);
تهی ipc_entry_syscall_cpu2(تهی);
تهی ipc_entry_syscall_cpu3(تهی);
تهی ipc_entry_syscall_cpu4(تهی);
تهی ipc_entry_syscall_cpu5(تهی);
تهی ipc_entry_syscall_cpu6(تهی);
تهی ipc_entry_syscall_cpu7(تهی);
تهی kernel_call_entry_orig(تهی);
تهی kernel_call_entry_um(تهی);
تهی level0_call(تهی);

/* exception.c */
ساختار exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

تهی exception(ساختار exception_frame * frame);

/* klib.S */
__dead تهی monitor(تهی);
__dead تهی reset(تهی);
__dead تهی poweroff_vmware_clihlt(تهی);
__dead تهی x86_triplefault(تهی);
reg_t read_cr0(تهی);
reg_t read_cr2(تهی);
تهی write_cr0(unsigned long مقدار);
unsigned long read_cr4(تهی);
تهی write_cr4(unsigned long مقدار);
تهی write_cr3(unsigned long مقدار);
unsigned long read_cpu_flags(تهی);
phys_bytes vir2phys(تهی *);
تهی phys_insb(u16_t port, phys_bytes buf, size_t شمار);
تهی phys_insw(u16_t port, phys_bytes buf, size_t شمار);
تهی phys_outsb(u16_t port, phys_bytes buf, size_t شمار);
تهی phys_outsw(u16_t port, phys_bytes buf, size_t شمار);
u32_t read_cr3(تهی);
تهی reload_cr3(تهی);
تهی i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
تهی reload_ds(تهی);
تهی ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
تهی ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
تهی fninit(تهی);
تهی clts(تهی);
تهی fxsave(تهی *);
تهی fnsave(تهی *);
صحیح fxrstor(تهی *);
صحیح __fxrstor_end(تهی *);
صحیح frstor(تهی *);
صحیح __frstor_end(تهی *);
صحیح __frstor_failure(تهی *);
unsigned short fnstsw(تهی);
تهی fnstcw(unsigned short* cw);
تهی x86_lgdt(تهی *);
تهی x86_lldt(u32_t);
تهی x86_ltr(u32_t);
تهی x86_lidt(تهی *);
تهی x86_load_kerncs(تهی);
تهی x86_load_ds(u32_t);
تهی x86_load_ss(u32_t);
تهی x86_load_es(u32_t);
تهی x86_load_fs(u32_t);
تهی x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
صحیح usermapped_send_softint(endpoint_t dest, message *m_ptr);
صحیح usermapped_receive_softint(endpoint_t src, message *m_ptr, صحیح *status_ptr);
صحیح usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
صحیح usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
صحیح usermapped_notify_softint(endpoint_t dest);
صحیح usermapped_do_kernel_call_softint(message *m_ptr);
صحیح usermapped_senda_softint(asynmsg_t *table, size_t شمار);

صحیح usermapped_send_syscall(endpoint_t dest, message *m_ptr);
صحیح usermapped_receive_syscall(endpoint_t src, message *m_ptr, صحیح *status_ptr);
صحیح usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
صحیح usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
صحیح usermapped_notify_syscall(endpoint_t dest);
صحیح usermapped_do_kernel_call_syscall(message *m_ptr);
صحیح usermapped_senda_syscall(asynmsg_t *table, size_t شمار);

صحیح usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
صحیح usermapped_receive_sysenter(endpoint_t src, message *m_ptr, صحیح *status_ptr);
صحیح usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
صحیح usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
صحیح usermapped_notify_sysenter(endpoint_t dest);
صحیح usermapped_do_kernel_call_sysenter(message *m_ptr);
صحیح usermapped_senda_sysenter(asynmsg_t *table, size_t شمار);

تهی switch_k_پشته(تهی * esp, تهی (* continuation)(تهی));

تهی __switch_address_space(ساختار proc * p, ساختار proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

تهی refresh_tlb(تهی);

/* multiboot.c */
تهی multiboot_init(تهی);

/* protect.c */
ساختار tss_s {
  reg_t backlink;
  reg_t sp0;                    /* stack pointer to use during interrupt */
  reg_t ss0;                    /*   "   segment  "  "    "        "     */
  reg_t sp1;
  reg_t ss1;
  reg_t sp2;
  reg_t ss2;
  reg_t cr3;
  reg_t ip;
  reg_t flags;
  reg_t ax;
  reg_t cx;
  reg_t dx;
  reg_t bx;
  reg_t sp;
  reg_t bp;
  reg_t si;
  reg_t di;
  reg_t es;
  reg_t cs;
  reg_t ss;
  reg_t ds;
  reg_t fs;
  reg_t gs;
  reg_t ldt;
  u16_t trap;
  u16_t iobase;
/* u8_t iomap[0]; */
} __attribute__((packed));

تهی enable_iop(ساختار proc *pp);
u32_t read_cs(تهی);
u32_t read_ds(تهی);
u32_t read_ss(تهی);

تهی add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
تهی vm_enable_paging(تهی);
تهی cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
تهی pg_info(reg_t *, u32_t **);
تهی pg_clear(تهی);
تهی pg_identity(kinfo_t *);
phys_bytes pg_load(تهی);
تهی pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
صحیح pg_mapkernel(تهی);
تهی pg_mapproc(ساختار proc *p, ساختار boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ساختار gate_table_s {
 تهی(*gate) (تهی);
	unsigned نویسه vec_nr;
	unsigned نویسه privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
تهی idt_copy_vectors(ساختار gate_table_s * first);
تهی idt_copy_vectors_pic(تهی);
تهی idt_reload(تهی);

EXTERN تهی * k_stacks_start;
بیرونی تهی * k_stacks;

#define get_k_پشته_بالاترین(cpu)	((تهی *)(((نویسه*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_پشته_SIZE))

تهی mfence(تهی);
#define barrier()	انجامبده { mfence(); } وقتی(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(تهی);
#define get_پشته_frame(__X)	((reg_t)read_ebp())
#وگرنه
/* read %ebp directly */
#define get_پشته_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
صحیح tss_init(unsigned cpu, تهی * kernel_پشته);

تهی int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

تهی __copy_msg_from_user_end(تهی);
تهی __copy_msg_to_user_end(تهی);
تهی __user_copy_msg_pointer_failure(تهی);

صحیح platform_tbl_checksum_ok(تهی *ptr, unsigned صحیح length);
صحیح platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, تهی * buff, unsigned size, phys_bytes * phys_addr, صحیح ((*
	cmp_f)(تهی *)));

/* breakpoints.c */
صحیح breakpoint_set(phys_bytes linaddr, صحیح bp, ثابت صحیح flags);
#define BREAKPOINT_شمار		4
#define BREAKPOINT_FLAG_RW_MASK		(3 << 0)
#define BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#define BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#define BREAKPOINT_FLAG_RW_RW		(2 << 0)
#define BREAKPOINT_FLAG_LEN_MASK	(3 << 2)
#define BREAKPOINT_FLAG_LEN_1		(0 << 2)
#define BREAKPOINT_FLAG_LEN_2		(1 << 2)
#define BREAKPOINT_FLAG_LEN_4		(2 << 2)
#define BREAKPOINT_FLAG_MODE_MASK	(3 << 4)
#define BREAKPOINT_FLAG_MODE_OFF	(0 << 4)
#define BREAKPOINT_FLAG_MODE_LOCAL	(1 << 4)
#define BREAKPOINT_FLAG_MODE_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#شامل "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
