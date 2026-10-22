
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#ضمن <machine/vm.h>

#define K_STACK_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
فراغ hwint00(فراغ);
فراغ hwint01(فراغ);
فراغ hwint02(فراغ);
فراغ hwint03(فراغ);
فراغ hwint04(فراغ);
فراغ hwint05(فراغ);
فراغ hwint06(فراغ);
فراغ hwint07(فراغ);
فراغ hwint08(فراغ);
فراغ hwint09(فراغ);
فراغ hwint10(فراغ);
فراغ hwint11(فراغ);
فراغ hwint12(فراغ);
فراغ hwint13(فراغ);
فراغ hwint14(فراغ);
فراغ hwint15(فراغ);

/* Exception handlers (real or protected mode), in numerical order. */
فراغ int00(فراغ), divide_error (فراغ);
فراغ int01(فراغ), single_step_exception (فراغ);
فراغ int02(فراغ), nmi (فراغ);
فراغ int03(فراغ), breakpoint_exception (فراغ);
فراغ int04(فراغ), overflow (فراغ);
فراغ int05(فراغ), bounds_check (فراغ);
فراغ int06(فراغ), inval_opcode (فراغ);
فراغ int07(فراغ), copr_not_available (فراغ);
فراغ double_fault(فراغ);
فراغ copr_seg_overrun(فراغ);
فراغ inval_tss(فراغ);
فراغ segment_not_present(فراغ);
فراغ stack_exception(فراغ);
فراغ general_protection(فراغ);
فراغ page_fault(فراغ);
فراغ copr_error(فراغ);
فراغ alignment_check(فراغ);
فراغ machine_check(فراغ);
فراغ simd_exception(فراغ);

فراغ restore_user_context_int(بنية proc *);
فراغ restore_user_context_sysenter(بنية proc *);
فراغ restore_user_context_syscall(بنية proc *);

/* Software interrupt handlers, in numerical order. */
فراغ trp(فراغ);
فراغ ipc_entry_softint_orig(فراغ);
فراغ ipc_entry_softint_um(فراغ);
فراغ ipc_entry_sysenter(فراغ);
فراغ ipc_entry_syscall_cpu0(فراغ);
فراغ ipc_entry_syscall_cpu1(فراغ);
فراغ ipc_entry_syscall_cpu2(فراغ);
فراغ ipc_entry_syscall_cpu3(فراغ);
فراغ ipc_entry_syscall_cpu4(فراغ);
فراغ ipc_entry_syscall_cpu5(فراغ);
فراغ ipc_entry_syscall_cpu6(فراغ);
فراغ ipc_entry_syscall_cpu7(فراغ);
فراغ kernel_call_entry_orig(فراغ);
فراغ kernel_call_entry_um(فراغ);
فراغ level0_call(فراغ);

/* exception.c */
بنية exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

فراغ exception(بنية exception_frame * frame);

/* klib.S */
__dead فراغ monitor(فراغ);
__dead فراغ reset(فراغ);
__dead فراغ poweroff_vmware_clihlt(فراغ);
__dead فراغ x86_triplefault(فراغ);
reg_t read_cr0(فراغ);
reg_t read_cr2(فراغ);
فراغ write_cr0(unsigned long value);
unsigned long read_cr4(فراغ);
فراغ write_cr4(unsigned long value);
فراغ write_cr3(unsigned long value);
unsigned long read_cpu_flags(فراغ);
phys_bytes vir2phys(فراغ *);
فراغ phys_insb(u16_t port, phys_bytes buf, size_t count);
فراغ phys_insw(u16_t port, phys_bytes buf, size_t count);
فراغ phys_outsb(u16_t port, phys_bytes buf, size_t count);
فراغ phys_outsw(u16_t port, phys_bytes buf, size_t count);
u32_t read_cr3(فراغ);
فراغ reload_cr3(فراغ);
فراغ i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
فراغ reload_ds(فراغ);
فراغ ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
فراغ ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
فراغ fninit(فراغ);
فراغ clts(فراغ);
فراغ fxsave(فراغ *);
فراغ fnsave(فراغ *);
صحيح fxrstor(فراغ *);
صحيح __fxrstor_end(فراغ *);
صحيح frstor(فراغ *);
صحيح __frstor_end(فراغ *);
صحيح __frstor_failure(فراغ *);
unsigned short fnstsw(فراغ);
فراغ fnstcw(unsigned short* cw);
فراغ x86_lgdt(فراغ *);
فراغ x86_lldt(u32_t);
فراغ x86_ltr(u32_t);
فراغ x86_lidt(فراغ *);
فراغ x86_load_kerncs(فراغ);
فراغ x86_load_ds(u32_t);
فراغ x86_load_ss(u32_t);
فراغ x86_load_es(u32_t);
فراغ x86_load_fs(u32_t);
فراغ x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
صحيح usermapped_send_softint(endpoint_t dest, message *m_ptr);
صحيح usermapped_receive_softint(endpoint_t src, message *m_ptr, صحيح *status_ptr);
صحيح usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
صحيح usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
صحيح usermapped_notify_softint(endpoint_t dest);
صحيح usermapped_do_kernel_call_softint(message *m_ptr);
صحيح usermapped_senda_softint(asynmsg_t *table, size_t count);

صحيح usermapped_send_syscall(endpoint_t dest, message *m_ptr);
صحيح usermapped_receive_syscall(endpoint_t src, message *m_ptr, صحيح *status_ptr);
صحيح usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
صحيح usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
صحيح usermapped_notify_syscall(endpoint_t dest);
صحيح usermapped_do_kernel_call_syscall(message *m_ptr);
صحيح usermapped_senda_syscall(asynmsg_t *table, size_t count);

صحيح usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
صحيح usermapped_receive_sysenter(endpoint_t src, message *m_ptr, صحيح *status_ptr);
صحيح usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
صحيح usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
صحيح usermapped_notify_sysenter(endpoint_t dest);
صحيح usermapped_do_kernel_call_sysenter(message *m_ptr);
صحيح usermapped_senda_sysenter(asynmsg_t *table, size_t count);

فراغ switch_k_stack(فراغ * esp, فراغ (* continuation)(فراغ));

فراغ __switch_address_space(بنية proc * p, بنية proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

فراغ refresh_tlb(فراغ);

/* multiboot.c */
فراغ multiboot_init(فراغ);

/* protect.c */
بنية tss_s {
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

فراغ enable_iop(بنية proc *pp);
u32_t read_cs(فراغ);
u32_t read_ds(فراغ);
u32_t read_ss(فراغ);

فراغ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_identity(kinfo_t *);
phys_bytes pg_load(فراغ);
فراغ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
بنية gate_table_s {
 فراغ(*gate) (فراغ);
	unsigned حرف vec_nr;
	unsigned حرف privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
فراغ idt_copy_vectors(بنية gate_table_s * first);
فراغ idt_copy_vectors_pic(فراغ);
فراغ idt_reload(فراغ);

EXTERN فراغ * k_stacks_start;
خارجي فراغ * k_stacks;

#define get_k_stack_top(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_STACK_SIZE))

فراغ mfence(فراغ);
#define barrier()	ܥܒܕ { mfence(); } ܟܡܐ(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(فراغ);
#define get_stack_frame(__X)	((reg_t)read_ebp())
#ܐܚܪܢܐ
/* read %ebp directly */
#define get_stack_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
صحيح tss_init(unsigned cpu, فراغ * kernel_stack);

فراغ int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

فراغ __copy_msg_from_user_end(فراغ);
فراغ __copy_msg_to_user_end(فراغ);
فراغ __user_copy_msg_pointer_failure(فراغ);

صحيح platform_tbl_checksum_ok(فراغ *ptr, unsigned صحيح length);
صحيح platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, فراغ * buff, unsigned size, phys_bytes * phys_addr, صحيح ((*
	cmp_f)(فراغ *)));

/* breakpoints.c */
صحيح breakpoint_set(phys_bytes linaddr, صحيح bp, ثابت صحيح flags);
#define BREAKPOINT_COUNT		4
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
#ضمن "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
