
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#অন্তর্ভাব <machine/vm.h>

#define K_স্তূপ_আকার	I386_PAGE_আকার

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
শূন্য hwint00(শূন্য);
শূন্য hwint01(শূন্য);
শূন্য hwint02(শূন্য);
শূন্য hwint03(শূন্য);
শূন্য hwint04(শূন্য);
শূন্য hwint05(শূন্য);
শূন্য hwint06(শূন্য);
শূন্য hwint07(শূন্য);
শূন্য hwint08(শূন্য);
শূন্য hwint09(শূন্য);
শূন্য hwint10(শূন্য);
শূন্য hwint11(শূন্য);
শূন্য hwint12(শূন্য);
শূন্য hwint13(শূন্য);
শূন্য hwint14(শূন্য);
শূন্য hwint15(শূন্য);

/* Exception handlers (real or protected mode), in numerical order. */
শূন্য int00(শূন্য), divide_error (শূন্য);
শূন্য int01(শূন্য), single_step_exception (শূন্য);
শূন্য int02(শূন্য), nmi (শূন্য);
শূন্য int03(শূন্য), breakpoint_exception (শূন্য);
শূন্য int04(শূন্য), overflow (শূন্য);
শূন্য int05(শূন্য), bounds_check (শূন্য);
শূন্য int06(শূন্য), inval_opcode (শূন্য);
শূন্য int07(শূন্য), copr_not_available (শূন্য);
শূন্য double_fault(শূন্য);
শূন্য copr_seg_overrun(শূন্য);
শূন্য inval_tss(শূন্য);
শূন্য segment_not_present(শূন্য);
শূন্য স্তূপ_exception(শূন্য);
শূন্য general_protection(শূন্য);
শূন্য page_fault(শূন্য);
শূন্য copr_error(শূন্য);
শূন্য alignment_check(শূন্য);
শূন্য machine_check(শূন্য);
শূন্য simd_exception(শূন্য);

শূন্য restore_user_context_int(কাঠামো proc *);
শূন্য restore_user_context_sysenter(কাঠামো proc *);
শূন্য restore_user_context_syscall(কাঠামো proc *);

/* Software interrupt handlers, in numerical order. */
শূন্য trp(শূন্য);
শূন্য ipc_entry_softint_orig(শূন্য);
শূন্য ipc_entry_softint_um(শূন্য);
শূন্য ipc_entry_sysenter(শূন্য);
শূন্য ipc_entry_syscall_cpu0(শূন্য);
শূন্য ipc_entry_syscall_cpu1(শূন্য);
শূন্য ipc_entry_syscall_cpu2(শূন্য);
শূন্য ipc_entry_syscall_cpu3(শূন্য);
শূন্য ipc_entry_syscall_cpu4(শূন্য);
শূন্য ipc_entry_syscall_cpu5(শূন্য);
শূন্য ipc_entry_syscall_cpu6(শূন্য);
শূন্য ipc_entry_syscall_cpu7(শূন্য);
শূন্য kernel_call_entry_orig(শূন্য);
শূন্য kernel_call_entry_um(শূন্য);
শূন্য level0_call(শূন্য);

/* exception.c */
কাঠামো exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

শূন্য exception(কাঠামো exception_frame * frame);

/* klib.S */
__dead শূন্য monitor(শূন্য);
__dead শূন্য reset(শূন্য);
__dead শূন্য poweroff_vmware_clihlt(শূন্য);
__dead শূন্য x86_triplefault(শূন্য);
reg_t read_cr0(শূন্য);
reg_t read_cr2(শূন্য);
শূন্য write_cr0(unsigned long মান);
unsigned long read_cr4(শূন্য);
শূন্য write_cr4(unsigned long মান);
শূন্য write_cr3(unsigned long মান);
unsigned long read_cpu_flags(শূন্য);
phys_bytes vir2phys(শূন্য *);
শূন্য phys_insb(u16_t port, phys_bytes buf, size_t গুনতি);
শূন্য phys_insw(u16_t port, phys_bytes buf, size_t গুনতি);
শূন্য phys_outsb(u16_t port, phys_bytes buf, size_t গুনতি);
শূন্য phys_outsw(u16_t port, phys_bytes buf, size_t গুনতি);
u32_t read_cr3(শূন্য);
শূন্য reload_cr3(শূন্য);
শূন্য i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
শূন্য reload_ds(শূন্য);
শূন্য ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
শূন্য ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
শূন্য fninit(শূন্য);
শূন্য clts(শূন্য);
শূন্য fxsave(শূন্য *);
শূন্য fnsave(শূন্য *);
পূর্ণসংখ্যা fxrstor(শূন্য *);
পূর্ণসংখ্যা __fxrstor_end(শূন্য *);
পূর্ণসংখ্যা frstor(শূন্য *);
পূর্ণসংখ্যা __frstor_end(শূন্য *);
পূর্ণসংখ্যা __frstor_failure(শূন্য *);
unsigned short fnstsw(শূন্য);
শূন্য fnstcw(unsigned short* cw);
শূন্য x86_lgdt(শূন্য *);
শূন্য x86_lldt(u32_t);
শূন্য x86_ltr(u32_t);
শূন্য x86_lidt(শূন্য *);
শূন্য x86_load_kerncs(শূন্য);
শূন্য x86_load_ds(u32_t);
শূন্য x86_load_ss(u32_t);
শূন্য x86_load_es(u32_t);
শূন্য x86_load_fs(u32_t);
শূন্য x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
পূর্ণসংখ্যা usermapped_send_softint(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_receive_softint(endpoint_t src, message *m_ptr, পূর্ণসংখ্যা *status_ptr);
পূর্ণসংখ্যা usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_notify_softint(endpoint_t dest);
পূর্ণসংখ্যা usermapped_do_kernel_call_softint(message *m_ptr);
পূর্ণসংখ্যা usermapped_senda_softint(asynmsg_t *table, size_t গুনতি);

পূর্ণসংখ্যা usermapped_send_syscall(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_receive_syscall(endpoint_t src, message *m_ptr, পূর্ণসংখ্যা *status_ptr);
পূর্ণসংখ্যা usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_notify_syscall(endpoint_t dest);
পূর্ণসংখ্যা usermapped_do_kernel_call_syscall(message *m_ptr);
পূর্ণসংখ্যা usermapped_senda_syscall(asynmsg_t *table, size_t গুনতি);

পূর্ণসংখ্যা usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_receive_sysenter(endpoint_t src, message *m_ptr, পূর্ণসংখ্যা *status_ptr);
পূর্ণসংখ্যা usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
পূর্ণসংখ্যা usermapped_notify_sysenter(endpoint_t dest);
পূর্ণসংখ্যা usermapped_do_kernel_call_sysenter(message *m_ptr);
পূর্ণসংখ্যা usermapped_senda_sysenter(asynmsg_t *table, size_t গুনতি);

শূন্য switch_k_স্তূপ(শূন্য * esp, শূন্য (* continuation)(শূন্য));

শূন্য __switch_address_space(কাঠামো proc * p, কাঠামো proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

শূন্য refresh_tlb(শূন্য);

/* multiboot.c */
শূন্য multiboot_init(শূন্য);

/* protect.c */
কাঠামো tss_s {
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

শূন্য enable_iop(কাঠামো proc *pp);
u32_t read_cs(শূন্য);
u32_t read_ds(শূন্য);
u32_t read_ss(শূন্য);

শূন্য add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
শূন্য vm_enable_paging(শূন্য);
শূন্য cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
শূন্য pg_info(reg_t *, u32_t **);
শূন্য pg_clear(শূন্য);
শূন্য pg_identity(kinfo_t *);
phys_bytes pg_load(শূন্য);
শূন্য pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
পূর্ণসংখ্যা pg_mapkernel(শূন্য);
শূন্য pg_mapproc(কাঠামো proc *p, কাঠামো boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
কাঠামো gate_table_s {
 শূন্য(*gate) (শূন্য);
	unsigned অক্ষর vec_nr;
	unsigned অক্ষর privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
শূন্য idt_copy_vectors(কাঠামো gate_table_s * first);
শূন্য idt_copy_vectors_pic(শূন্য);
শূন্য idt_reload(শূন্য);

EXTERN শূন্য * k_stacks_start;
বাহ্যিক শূন্য * k_stacks;

#define get_k_স্তূপ_শীর্ষ(cpu)	((শূন্য *)(((অক্ষর*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_স্তূপ_আকার))

শূন্য mfence(শূন্য);
#define barrier()	করো { mfence(); } যখন(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(শূন্য);
#define get_স্তূপ_frame(__X)	((reg_t)read_ebp())
#অন্যথা
/* read %ebp directly */
#define get_স্তূপ_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
পূর্ণসংখ্যা tss_init(unsigned cpu, শূন্য * kernel_স্তূপ);

শূন্য int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

শূন্য __copy_msg_from_user_end(শূন্য);
শূন্য __copy_msg_to_user_end(শূন্য);
শূন্য __user_copy_msg_pointer_failure(শূন্য);

পূর্ণসংখ্যা platform_tbl_checksum_ok(শূন্য *ptr, unsigned পূর্ণসংখ্যা দৈর্ঘ্য);
পূর্ণসংখ্যা platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, শূন্য * buff, unsigned আকার_2, phys_bytes * phys_addr, পূর্ণসংখ্যা ((*
	cmp_f)(শূন্য *)));

/* breakpoints.c */
পূর্ণসংখ্যা breakpoint_set(phys_bytes linaddr, পূর্ণসংখ্যা bp, ধ্রুবক পূর্ণসংখ্যা flags);
#define BREAKPOINT_গুনতি		4
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
#অন্তর্ভাব "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
