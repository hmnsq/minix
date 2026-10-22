
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#define K_ଗଦା_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
ଶୂନ୍ୟ hwint00(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint01(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint02(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint03(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint04(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint05(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint06(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint07(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint08(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint09(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint10(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint11(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint12(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint13(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint14(ଶୂନ୍ୟ);
ଶୂନ୍ୟ hwint15(ଶୂନ୍ୟ);

/* Exception handlers (real or protected mode), in numerical order. */
ଶୂନ୍ୟ int00(ଶୂନ୍ୟ), divide_error (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int01(ଶୂନ୍ୟ), single_step_exception (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int02(ଶୂନ୍ୟ), nmi (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int03(ଶୂନ୍ୟ), breakpoint_exception (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int04(ଶୂନ୍ୟ), overflow (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int05(ଶୂନ୍ୟ), bounds_check (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int06(ଶୂନ୍ୟ), inval_opcode (ଶୂନ୍ୟ);
ଶୂନ୍ୟ int07(ଶୂନ୍ୟ), copr_not_available (ଶୂନ୍ୟ);
ଶୂନ୍ୟ double_fault(ଶୂନ୍ୟ);
ଶୂନ୍ୟ copr_seg_overrun(ଶୂନ୍ୟ);
ଶୂନ୍ୟ inval_tss(ଶୂନ୍ୟ);
ଶୂନ୍ୟ segment_not_present(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ଗଦା_exception(ଶୂନ୍ୟ);
ଶୂନ୍ୟ general_protection(ଶୂନ୍ୟ);
ଶୂନ୍ୟ page_fault(ଶୂନ୍ୟ);
ଶୂନ୍ୟ copr_error(ଶୂନ୍ୟ);
ଶୂନ୍ୟ alignment_check(ଶୂନ୍ୟ);
ଶୂନ୍ୟ machine_check(ଶୂନ୍ୟ);
ଶୂନ୍ୟ simd_exception(ଶୂନ୍ୟ);

ଶୂନ୍ୟ restore_user_context_int(ସଂରଚନା proc *);
ଶୂନ୍ୟ restore_user_context_sysenter(ସଂରଚନା proc *);
ଶୂନ୍ୟ restore_user_context_syscall(ସଂରଚନା proc *);

/* Software interrupt handlers, in numerical order. */
ଶୂନ୍ୟ trp(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_softint_orig(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_softint_um(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_sysenter(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu0(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu1(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu2(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu3(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu4(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu5(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu6(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ipc_entry_syscall_cpu7(ଶୂନ୍ୟ);
ଶୂନ୍ୟ kernel_call_entry_orig(ଶୂନ୍ୟ);
ଶୂନ୍ୟ kernel_call_entry_um(ଶୂନ୍ୟ);
ଶୂନ୍ୟ level0_call(ଶୂନ୍ୟ);

/* exception.c */
ସଂରଚନା exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

ଶୂନ୍ୟ exception(ସଂରଚନା exception_frame * frame);

/* klib.S */
__dead ଶୂନ୍ୟ monitor(ଶୂନ୍ୟ);
__dead ଶୂନ୍ୟ reset(ଶୂନ୍ୟ);
__dead ଶୂନ୍ୟ poweroff_vmware_clihlt(ଶୂନ୍ୟ);
__dead ଶୂନ୍ୟ x86_triplefault(ଶୂନ୍ୟ);
reg_t read_cr0(ଶୂନ୍ୟ);
reg_t read_cr2(ଶୂନ୍ୟ);
ଶୂନ୍ୟ write_cr0(unsigned long ମୂଲ୍ୟ);
unsigned long read_cr4(ଶୂନ୍ୟ);
ଶୂନ୍ୟ write_cr4(unsigned long ମୂଲ୍ୟ);
ଶୂନ୍ୟ write_cr3(unsigned long ମୂଲ୍ୟ);
unsigned long read_cpu_flags(ଶୂନ୍ୟ);
phys_bytes vir2phys(ଶୂନ୍ୟ *);
ଶୂନ୍ୟ phys_insb(u16_t port, phys_bytes buf, size_t ଗଣନା_2);
ଶୂନ୍ୟ phys_insw(u16_t port, phys_bytes buf, size_t ଗଣନା_2);
ଶୂନ୍ୟ phys_outsb(u16_t port, phys_bytes buf, size_t ଗଣନା_2);
ଶୂନ୍ୟ phys_outsw(u16_t port, phys_bytes buf, size_t ଗଣନା_2);
u32_t read_cr3(ଶୂନ୍ୟ);
ଶୂନ୍ୟ reload_cr3(ଶୂନ୍ୟ);
ଶୂନ୍ୟ i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
ଶୂନ୍ୟ reload_ds(ଶୂନ୍ୟ);
ଶୂନ୍ୟ ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
ଶୂନ୍ୟ ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
ଶୂନ୍ୟ fninit(ଶୂନ୍ୟ);
ଶୂନ୍ୟ clts(ଶୂନ୍ୟ);
ଶୂନ୍ୟ fxsave(ଶୂନ୍ୟ *);
ଶୂନ୍ୟ fnsave(ଶୂନ୍ୟ *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fxrstor(ଶୂନ୍ୟ *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା __fxrstor_end(ଶୂନ୍ୟ *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା frstor(ଶୂନ୍ୟ *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା __frstor_end(ଶୂନ୍ୟ *);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା __frstor_failure(ଶୂନ୍ୟ *);
unsigned short fnstsw(ଶୂନ୍ୟ);
ଶୂନ୍ୟ fnstcw(unsigned short* cw);
ଶୂନ୍ୟ x86_lgdt(ଶୂନ୍ୟ *);
ଶୂନ୍ୟ x86_lldt(u32_t);
ଶୂନ୍ୟ x86_ltr(u32_t);
ଶୂନ୍ୟ x86_lidt(ଶୂନ୍ୟ *);
ଶୂନ୍ୟ x86_load_kerncs(ଶୂନ୍ୟ);
ଶୂନ୍ୟ x86_load_ds(u32_t);
ଶୂନ୍ୟ x86_load_ss(u32_t);
ଶୂନ୍ୟ x86_load_es(u32_t);
ଶୂନ୍ୟ x86_load_fs(u32_t);
ଶୂନ୍ୟ x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_send_softint(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_receive_softint(endpoint_t src, message *m_ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *status_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_notify_softint(endpoint_t dest);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_do_kernel_call_softint(message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_senda_softint(asynmsg_t *table, size_t ଗଣନା_2);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_send_syscall(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_receive_syscall(endpoint_t src, message *m_ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *status_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_notify_syscall(endpoint_t dest);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_do_kernel_call_syscall(message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_senda_syscall(asynmsg_t *table, size_t ଗଣନା_2);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_receive_sysenter(endpoint_t src, message *m_ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *status_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_notify_sysenter(endpoint_t dest);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_do_kernel_call_sysenter(message *m_ptr);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା usermapped_senda_sysenter(asynmsg_t *table, size_t ଗଣନା_2);

ଶୂନ୍ୟ switch_k_ଗଦା(ଶୂନ୍ୟ * esp, ଶୂନ୍ୟ (* continuation)(ଶୂନ୍ୟ));

ଶୂନ୍ୟ __switch_address_space(ସଂରଚନା proc * p, ସଂରଚନା proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ଶୂନ୍ୟ refresh_tlb(ଶୂନ୍ୟ);

/* multiboot.c */
ଶୂନ୍ୟ multiboot_init(ଶୂନ୍ୟ);

/* protect.c */
ସଂରଚନା tss_s {
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

ଶୂନ୍ୟ enable_iop(ସଂରଚନା proc *pp);
u32_t read_cs(ଶୂନ୍ୟ);
u32_t read_ds(ଶୂନ୍ୟ);
u32_t read_ss(ଶୂନ୍ୟ);

ଶୂନ୍ୟ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
ଶୂନ୍ୟ vm_enable_paging(ଶୂନ୍ୟ);
ଶୂନ୍ୟ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
ଶୂନ୍ୟ pg_info(reg_t *, u32_t **);
ଶୂନ୍ୟ pg_clear(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_identity(kinfo_t *);
phys_bytes pg_load(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pg_mapkernel(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_mapproc(ସଂରଚନା proc *p, ସଂରଚନା boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ସଂରଚନା gate_table_s {
 ଶୂନ୍ୟ(*gate) (ଶୂନ୍ୟ);
	unsigned ଅକ୍ଷର vec_nr;
	unsigned ଅକ୍ଷର privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
ଶୂନ୍ୟ idt_copy_vectors(ସଂରଚନା gate_table_s * first);
ଶୂନ୍ୟ idt_copy_vectors_pic(ଶୂନ୍ୟ);
ଶୂନ୍ୟ idt_reload(ଶୂନ୍ୟ);

EXTERN ଶୂନ୍ୟ * k_stacks_start;
ବାହ୍ୟ ଶୂନ୍ୟ * k_stacks;

#define get_k_ଗଦା_ଶୀର୍ଷ(cpu)	((ଶୂନ୍ୟ *)(((ଅକ୍ଷର*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ଗଦା_SIZE))

ଶୂନ୍ୟ mfence(ଶୂନ୍ୟ);
#define barrier()	କର { mfence(); } ଯେତେବେଳେ(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(ଶୂନ୍ୟ);
#define get_ଗଦା_frame(__X)	((reg_t)read_ebp())
#ଅନ୍ୟଥା
/* read %ebp directly */
#define get_ଗଦା_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା tss_init(unsigned cpu, ଶୂନ୍ୟ * kernel_ଗଦା);

ଶୂନ୍ୟ int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

ଶୂନ୍ୟ __copy_msg_from_user_end(ଶୂନ୍ୟ);
ଶୂନ୍ୟ __copy_msg_to_user_end(ଶୂନ୍ୟ);
ଶୂନ୍ୟ __user_copy_msg_pointer_failure(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_tbl_checksum_ok(ଶୂନ୍ୟ *ptr, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, ଶୂନ୍ୟ * buff, unsigned size, phys_bytes * phys_addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ((*
	cmp_f)(ଶୂନ୍ୟ *)));

/* breakpoints.c */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା breakpoint_set(phys_bytes linaddr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bp, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
#define BREAKPOINT_ଗଣନା		4
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
#ଅନ୍ତର୍ଭାଵ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
