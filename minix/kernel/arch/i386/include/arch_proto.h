
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#define K_ರಾಶಿ_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
ಶೂನ್ಯ hwint00(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint01(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint02(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint03(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint04(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint05(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint06(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint07(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint08(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint09(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint10(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint11(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint12(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint13(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint14(ಶೂನ್ಯ);
ಶೂನ್ಯ hwint15(ಶೂನ್ಯ);

/* Exception handlers (real or protected mode), in numerical order. */
ಶೂನ್ಯ int00(ಶೂನ್ಯ), divide_error (ಶೂನ್ಯ);
ಶೂನ್ಯ int01(ಶೂನ್ಯ), single_step_exception (ಶೂನ್ಯ);
ಶೂನ್ಯ int02(ಶೂನ್ಯ), nmi (ಶೂನ್ಯ);
ಶೂನ್ಯ int03(ಶೂನ್ಯ), breakpoint_exception (ಶೂನ್ಯ);
ಶೂನ್ಯ int04(ಶೂನ್ಯ), overflow (ಶೂನ್ಯ);
ಶೂನ್ಯ int05(ಶೂನ್ಯ), bounds_check (ಶೂನ್ಯ);
ಶೂನ್ಯ int06(ಶೂನ್ಯ), inval_opcode (ಶೂನ್ಯ);
ಶೂನ್ಯ int07(ಶೂನ್ಯ), copr_not_available (ಶೂನ್ಯ);
ಶೂನ್ಯ double_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ copr_seg_overrun(ಶೂನ್ಯ);
ಶೂನ್ಯ inval_tss(ಶೂನ್ಯ);
ಶೂನ್ಯ segment_not_present(ಶೂನ್ಯ);
ಶೂನ್ಯ ರಾಶಿ_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ general_protection(ಶೂನ್ಯ);
ಶೂನ್ಯ page_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ copr_error(ಶೂನ್ಯ);
ಶೂನ್ಯ alignment_check(ಶೂನ್ಯ);
ಶೂನ್ಯ machine_check(ಶೂನ್ಯ);
ಶೂನ್ಯ simd_exception(ಶೂನ್ಯ);

ಶೂನ್ಯ restore_user_context_int(ಸಂರಚನಾ proc *);
ಶೂನ್ಯ restore_user_context_sysenter(ಸಂರಚನಾ proc *);
ಶೂನ್ಯ restore_user_context_syscall(ಸಂರಚನಾ proc *);

/* Software interrupt handlers, in numerical order. */
ಶೂನ್ಯ trp(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_softint_orig(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_softint_um(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_sysenter(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu0(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu1(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu2(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu3(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu4(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu5(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu6(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_entry_syscall_cpu7(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call_entry_orig(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call_entry_um(ಶೂನ್ಯ);
ಶೂನ್ಯ level0_call(ಶೂನ್ಯ);

/* exception.c */
ಸಂರಚನಾ exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

ಶೂನ್ಯ exception(ಸಂರಚನಾ exception_frame * frame);

/* klib.S */
__dead ಶೂನ್ಯ monitor(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ reset(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ poweroff_vmware_clihlt(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ x86_triplefault(ಶೂನ್ಯ);
reg_t read_cr0(ಶೂನ್ಯ);
reg_t read_cr2(ಶೂನ್ಯ);
ಶೂನ್ಯ write_cr0(unsigned long ಮೌಲ್ಯ);
unsigned long read_cr4(ಶೂನ್ಯ);
ಶೂನ್ಯ write_cr4(unsigned long ಮೌಲ್ಯ);
ಶೂನ್ಯ write_cr3(unsigned long ಮೌಲ್ಯ);
unsigned long read_cpu_flags(ಶೂನ್ಯ);
phys_bytes vir2phys(ಶೂನ್ಯ *);
ಶೂನ್ಯ phys_insb(u16_t port, phys_bytes buf, size_t ಎಣಿಕೆ);
ಶೂನ್ಯ phys_insw(u16_t port, phys_bytes buf, size_t ಎಣಿಕೆ);
ಶೂನ್ಯ phys_outsb(u16_t port, phys_bytes buf, size_t ಎಣಿಕೆ);
ಶೂನ್ಯ phys_outsw(u16_t port, phys_bytes buf, size_t ಎಣಿಕೆ);
u32_t read_cr3(ಶೂನ್ಯ);
ಶೂನ್ಯ reload_cr3(ಶೂನ್ಯ);
ಶೂನ್ಯ i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
ಶೂನ್ಯ reload_ds(ಶೂನ್ಯ);
ಶೂನ್ಯ ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
ಶೂನ್ಯ ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
ಶೂನ್ಯ fninit(ಶೂನ್ಯ);
ಶೂನ್ಯ clts(ಶೂನ್ಯ);
ಶೂನ್ಯ fxsave(ಶೂನ್ಯ *);
ಶೂನ್ಯ fnsave(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಙ್ಕ fxrstor(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಙ್ಕ __fxrstor_end(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಙ್ಕ frstor(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಙ್ಕ __frstor_end(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಙ್ಕ __frstor_failure(ಶೂನ್ಯ *);
unsigned short fnstsw(ಶೂನ್ಯ);
ಶೂನ್ಯ fnstcw(unsigned short* cw);
ಶೂನ್ಯ x86_lgdt(ಶೂನ್ಯ *);
ಶೂನ್ಯ x86_lldt(u32_t);
ಶೂನ್ಯ x86_ltr(u32_t);
ಶೂನ್ಯ x86_lidt(ಶೂನ್ಯ *);
ಶೂನ್ಯ x86_load_kerncs(ಶೂನ್ಯ);
ಶೂನ್ಯ x86_load_ds(u32_t);
ಶೂನ್ಯ x86_load_ss(u32_t);
ಶೂನ್ಯ x86_load_es(u32_t);
ಶೂನ್ಯ x86_load_fs(u32_t);
ಶೂನ್ಯ x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
ಪೂರ್ಣಾಙ್ಕ usermapped_send_softint(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_receive_softint(endpoint_t src, message *m_ptr, ಪೂರ್ಣಾಙ್ಕ *status_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_notify_softint(endpoint_t dest);
ಪೂರ್ಣಾಙ್ಕ usermapped_do_kernel_call_softint(message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_senda_softint(asynmsg_t *table, size_t ಎಣಿಕೆ);

ಪೂರ್ಣಾಙ್ಕ usermapped_send_syscall(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_receive_syscall(endpoint_t src, message *m_ptr, ಪೂರ್ಣಾಙ್ಕ *status_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_notify_syscall(endpoint_t dest);
ಪೂರ್ಣಾಙ್ಕ usermapped_do_kernel_call_syscall(message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_senda_syscall(asynmsg_t *table, size_t ಎಣಿಕೆ);

ಪೂರ್ಣಾಙ್ಕ usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_receive_sysenter(endpoint_t src, message *m_ptr, ಪೂರ್ಣಾಙ್ಕ *status_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_notify_sysenter(endpoint_t dest);
ಪೂರ್ಣಾಙ್ಕ usermapped_do_kernel_call_sysenter(message *m_ptr);
ಪೂರ್ಣಾಙ್ಕ usermapped_senda_sysenter(asynmsg_t *table, size_t ಎಣಿಕೆ);

ಶೂನ್ಯ switch_k_ರಾಶಿ(ಶೂನ್ಯ * esp, ಶೂನ್ಯ (* continuation)(ಶೂನ್ಯ));

ಶೂನ್ಯ __switch_address_space(ಸಂರಚನಾ proc * p, ಸಂರಚನಾ proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ಶೂನ್ಯ refresh_tlb(ಶೂನ್ಯ);

/* multiboot.c */
ಶೂನ್ಯ multiboot_init(ಶೂನ್ಯ);

/* protect.c */
ಸಂರಚನಾ tss_s {
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

ಶೂನ್ಯ enable_iop(ಸಂರಚನಾ proc *pp);
u32_t read_cs(ಶೂನ್ಯ);
u32_t read_ds(ಶೂನ್ಯ);
u32_t read_ss(ಶೂನ್ಯ);

ಶೂನ್ಯ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ);
ಶೂನ್ಯ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
ಶೂನ್ಯ pg_info(reg_t *, u32_t **);
ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_identity(kinfo_t *);
phys_bytes pg_load(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
ಪೂರ್ಣಾಙ್ಕ pg_mapkernel(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_mapproc(ಸಂರಚನಾ proc *p, ಸಂರಚನಾ boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ಸಂರಚನಾ gate_table_s {
 ಶೂನ್ಯ(*gate) (ಶೂನ್ಯ);
	unsigned ವರ್ಣ vec_nr;
	unsigned ವರ್ಣ privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
ಶೂನ್ಯ idt_copy_vectors(ಸಂರಚನಾ gate_table_s * first);
ಶೂನ್ಯ idt_copy_vectors_pic(ಶೂನ್ಯ);
ಶೂನ್ಯ idt_reload(ಶೂನ್ಯ);

EXTERN ಶೂನ್ಯ * k_stacks_start;
ಬಾಹ್ಯ ಶೂನ್ಯ * k_stacks;

#define get_k_ರಾಶಿ_ತುದಿ(cpu)	((ಶೂನ್ಯ *)(((ವರ್ಣ*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ರಾಶಿ_SIZE))

ಶೂನ್ಯ mfence(ಶೂನ್ಯ);
#define barrier()	ಕುರು { mfence(); } ಯಾವತ್(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(ಶೂನ್ಯ);
#define get_ರಾಶಿ_frame(__X)	((reg_t)read_ebp())
#ಅನ್ಯಥಾ
/* read %ebp directly */
#define get_ರಾಶಿ_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
ಪೂರ್ಣಾಙ್ಕ tss_init(unsigned cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ);

ಶೂನ್ಯ int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

ಶೂನ್ಯ __copy_msg_from_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __copy_msg_to_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __user_copy_msg_pointer_failure(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ platform_tbl_checksum_ok(ಶೂನ್ಯ *ptr, unsigned ಪೂರ್ಣಾಙ್ಕ length);
ಪೂರ್ಣಾಙ್ಕ platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, ಶೂನ್ಯ * buff, unsigned size, phys_bytes * phys_addr, ಪೂರ್ಣಾಙ್ಕ ((*
	cmp_f)(ಶೂನ್ಯ *)));

/* breakpoints.c */
ಪೂರ್ಣಾಙ್ಕ breakpoint_set(phys_bytes linaddr, ಪೂರ್ಣಾಙ್ಕ bp, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ flags);
#define BREAKPOINT_ಎಣಿಕೆ		4
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
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
