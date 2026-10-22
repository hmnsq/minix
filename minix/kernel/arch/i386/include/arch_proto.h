
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#અન્તર્ભાવ <machine/vm.h>

#define K_ઢગલો_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
શૂન્ય hwint00(શૂન્ય);
શૂન્ય hwint01(શૂન્ય);
શૂન્ય hwint02(શૂન્ય);
શૂન્ય hwint03(શૂન્ય);
શૂન્ય hwint04(શૂન્ય);
શૂન્ય hwint05(શૂન્ય);
શૂન્ય hwint06(શૂન્ય);
શૂન્ય hwint07(શૂન્ય);
શૂન્ય hwint08(શૂન્ય);
શૂન્ય hwint09(શૂન્ય);
શૂન્ય hwint10(શૂન્ય);
શૂન્ય hwint11(શૂન્ય);
શૂન્ય hwint12(શૂન્ય);
શૂન્ય hwint13(શૂન્ય);
શૂન્ય hwint14(શૂન્ય);
શૂન્ય hwint15(શૂન્ય);

/* Exception handlers (real or protected mode), in numerical order. */
શૂન્ય int00(શૂન્ય), divide_error (શૂન્ય);
શૂન્ય int01(શૂન્ય), single_step_exception (શૂન્ય);
શૂન્ય int02(શૂન્ય), nmi (શૂન્ય);
શૂન્ય int03(શૂન્ય), breakpoint_exception (શૂન્ય);
શૂન્ય int04(શૂન્ય), overflow (શૂન્ય);
શૂન્ય int05(શૂન્ય), bounds_check (શૂન્ય);
શૂન્ય int06(શૂન્ય), inval_opcode (શૂન્ય);
શૂન્ય int07(શૂન્ય), copr_not_available (શૂન્ય);
શૂન્ય double_fault(શૂન્ય);
શૂન્ય copr_seg_overrun(શૂન્ય);
શૂન્ય inval_tss(શૂન્ય);
શૂન્ય segment_not_present(શૂન્ય);
શૂન્ય ઢગલો_exception(શૂન્ય);
શૂન્ય general_protection(શૂન્ય);
શૂન્ય page_fault(શૂન્ય);
શૂન્ય copr_error(શૂન્ય);
શૂન્ય alignment_check(શૂન્ય);
શૂન્ય machine_check(શૂન્ય);
શૂન્ય simd_exception(શૂન્ય);

શૂન્ય restore_user_context_int(સંરચના proc *);
શૂન્ય restore_user_context_sysenter(સંરચના proc *);
શૂન્ય restore_user_context_syscall(સંરચના proc *);

/* Software interrupt handlers, in numerical order. */
શૂન્ય trp(શૂન્ય);
શૂન્ય ipc_entry_softint_orig(શૂન્ય);
શૂન્ય ipc_entry_softint_um(શૂન્ય);
શૂન્ય ipc_entry_sysenter(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu0(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu1(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu2(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu3(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu4(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu5(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu6(શૂન્ય);
શૂન્ય ipc_entry_syscall_cpu7(શૂન્ય);
શૂન્ય kernel_call_entry_orig(શૂન્ય);
શૂન્ય kernel_call_entry_um(શૂન્ય);
શૂન્ય level0_call(શૂન્ય);

/* exception.c */
સંરચના exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

શૂન્ય exception(સંરચના exception_frame * frame);

/* klib.S */
__dead શૂન્ય monitor(શૂન્ય);
__dead શૂન્ય reset(શૂન્ય);
__dead શૂન્ય poweroff_vmware_clihlt(શૂન્ય);
__dead શૂન્ય x86_triplefault(શૂન્ય);
reg_t read_cr0(શૂન્ય);
reg_t read_cr2(શૂન્ય);
શૂન્ય write_cr0(unsigned long કિંમત);
unsigned long read_cr4(શૂન્ય);
શૂન્ય write_cr4(unsigned long કિંમત);
શૂન્ય write_cr3(unsigned long કિંમત);
unsigned long read_cpu_flags(શૂન્ય);
phys_bytes vir2phys(શૂન્ય *);
શૂન્ય phys_insb(u16_t port, phys_bytes buf, size_t ગણતરી);
શૂન્ય phys_insw(u16_t port, phys_bytes buf, size_t ગણતરી);
શૂન્ય phys_outsb(u16_t port, phys_bytes buf, size_t ગણતરી);
શૂન્ય phys_outsw(u16_t port, phys_bytes buf, size_t ગણતરી);
u32_t read_cr3(શૂન્ય);
શૂન્ય reload_cr3(શૂન્ય);
શૂન્ય i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
શૂન્ય reload_ds(શૂન્ય);
શૂન્ય ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
શૂન્ય ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
શૂન્ય fninit(શૂન્ય);
શૂન્ય clts(શૂન્ય);
શૂન્ય fxsave(શૂન્ય *);
શૂન્ય fnsave(શૂન્ય *);
પૂર્ણાંક fxrstor(શૂન્ય *);
પૂર્ણાંક __fxrstor_end(શૂન્ય *);
પૂર્ણાંક frstor(શૂન્ય *);
પૂર્ણાંક __frstor_end(શૂન્ય *);
પૂર્ણાંક __frstor_failure(શૂન્ય *);
unsigned short fnstsw(શૂન્ય);
શૂન્ય fnstcw(unsigned short* cw);
શૂન્ય x86_lgdt(શૂન્ય *);
શૂન્ય x86_lldt(u32_t);
શૂન્ય x86_ltr(u32_t);
શૂન્ય x86_lidt(શૂન્ય *);
શૂન્ય x86_load_kerncs(શૂન્ય);
શૂન્ય x86_load_ds(u32_t);
શૂન્ય x86_load_ss(u32_t);
શૂન્ય x86_load_es(u32_t);
શૂન્ય x86_load_fs(u32_t);
શૂન્ય x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
પૂર્ણાંક usermapped_send_softint(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_receive_softint(endpoint_t src, message *m_ptr, પૂર્ણાંક *status_ptr);
પૂર્ણાંક usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
પૂર્ણાંક usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_notify_softint(endpoint_t dest);
પૂર્ણાંક usermapped_do_kernel_call_softint(message *m_ptr);
પૂર્ણાંક usermapped_senda_softint(asynmsg_t *table, size_t ગણતરી);

પૂર્ણાંક usermapped_send_syscall(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_receive_syscall(endpoint_t src, message *m_ptr, પૂર્ણાંક *status_ptr);
પૂર્ણાંક usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
પૂર્ણાંક usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_notify_syscall(endpoint_t dest);
પૂર્ણાંક usermapped_do_kernel_call_syscall(message *m_ptr);
પૂર્ણાંક usermapped_senda_syscall(asynmsg_t *table, size_t ગણતરી);

પૂર્ણાંક usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_receive_sysenter(endpoint_t src, message *m_ptr, પૂર્ણાંક *status_ptr);
પૂર્ણાંક usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
પૂર્ણાંક usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
પૂર્ણાંક usermapped_notify_sysenter(endpoint_t dest);
પૂર્ણાંક usermapped_do_kernel_call_sysenter(message *m_ptr);
પૂર્ણાંક usermapped_senda_sysenter(asynmsg_t *table, size_t ગણતરી);

શૂન્ય switch_k_ઢગલો(શૂન્ય * esp, શૂન્ય (* continuation)(શૂન્ય));

શૂન્ય __switch_address_space(સંરચના proc * p, સંરચના proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

શૂન્ય refresh_tlb(શૂન્ય);

/* multiboot.c */
શૂન્ય multiboot_init(શૂન્ય);

/* protect.c */
સંરચના tss_s {
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

શૂન્ય enable_iop(સંરચના proc *pp);
u32_t read_cs(શૂન્ય);
u32_t read_ds(શૂન્ય);
u32_t read_ss(શૂન્ય);

શૂન્ય add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
શૂન્ય vm_enable_paging(શૂન્ય);
શૂન્ય cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
શૂન્ય pg_info(reg_t *, u32_t **);
શૂન્ય pg_clear(શૂન્ય);
શૂન્ય pg_identity(kinfo_t *);
phys_bytes pg_load(શૂન્ય);
શૂન્ય pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
પૂર્ણાંક pg_mapkernel(શૂન્ય);
શૂન્ય pg_mapproc(સંરચના proc *p, સંરચના boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
સંરચના gate_table_s {
 શૂન્ય(*gate) (શૂન્ય);
	unsigned અક્ષર vec_nr;
	unsigned અક્ષર privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
શૂન્ય idt_copy_vectors(સંરચના gate_table_s * first);
શૂન્ય idt_copy_vectors_pic(શૂન્ય);
શૂન્ય idt_reload(શૂન્ય);

EXTERN શૂન્ય * k_stacks_start;
બાહ્ય શૂન્ય * k_stacks;

#define get_k_ઢગલો_ટોચ(cpu)	((શૂન્ય *)(((અક્ષર*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ઢગલો_SIZE))

શૂન્ય mfence(શૂન્ય);
#define barrier()	કરો { mfence(); } જ્યારે(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(શૂન્ય);
#define get_ઢગલો_frame(__X)	((reg_t)read_ebp())
#નહીંતર
/* read %ebp directly */
#define get_ઢગલો_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
પૂર્ણાંક tss_init(unsigned cpu, શૂન્ય * kernel_ઢગલો);

શૂન્ય int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

શૂન્ય __copy_msg_from_user_end(શૂન્ય);
શૂન્ય __copy_msg_to_user_end(શૂન્ય);
શૂન્ય __user_copy_msg_pointer_failure(શૂન્ય);

પૂર્ણાંક platform_tbl_checksum_ok(શૂન્ય *ptr, unsigned પૂર્ણાંક length);
પૂર્ણાંક platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, શૂન્ય * buff, unsigned size, phys_bytes * phys_addr, પૂર્ણાંક ((*
	cmp_f)(શૂન્ય *)));

/* breakpoints.c */
પૂર્ણાંક breakpoint_set(phys_bytes linaddr, પૂર્ણાંક bp, અચલ પૂર્ણાંક flags);
#define BREAKPOINT_ગણતરી		4
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
#અન્તર્ભાવ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
