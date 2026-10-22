
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#अन्तर्भाव <machine/vm.h>

#define K_ढेर_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
शून्य hwint00(शून्य);
शून्य hwint01(शून्य);
शून्य hwint02(शून्य);
शून्य hwint03(शून्य);
शून्य hwint04(शून्य);
शून्य hwint05(शून्य);
शून्य hwint06(शून्य);
शून्य hwint07(शून्य);
शून्य hwint08(शून्य);
शून्य hwint09(शून्य);
शून्य hwint10(शून्य);
शून्य hwint11(शून्य);
शून्य hwint12(शून्य);
शून्य hwint13(शून्य);
शून्य hwint14(शून्य);
शून्य hwint15(शून्य);

/* Exception handlers (real or protected mode), in numerical order. */
शून्य int00(शून्य), divide_error (शून्य);
शून्य int01(शून्य), single_step_exception (शून्य);
शून्य int02(शून्य), nmi (शून्य);
शून्य int03(शून्य), breakpoint_exception (शून्य);
शून्य int04(शून्य), overflow (शून्य);
शून्य int05(शून्य), bounds_check (शून्य);
शून्य int06(शून्य), inval_opcode (शून्य);
शून्य int07(शून्य), copr_not_available (शून्य);
शून्य double_fault(शून्य);
शून्य copr_seg_overrun(शून्य);
शून्य inval_tss(शून्य);
शून्य segment_not_present(शून्य);
शून्य ढेर_exception(शून्य);
शून्य general_protection(शून्य);
शून्य page_fault(शून्य);
शून्य copr_error(शून्य);
शून्य alignment_check(शून्य);
शून्य machine_check(शून्य);
शून्य simd_exception(शून्य);

शून्य restore_user_context_int(संरचना proc *);
शून्य restore_user_context_sysenter(संरचना proc *);
शून्य restore_user_context_syscall(संरचना proc *);

/* Software interrupt handlers, in numerical order. */
शून्य trp(शून्य);
शून्य ipc_entry_softint_orig(शून्य);
शून्य ipc_entry_softint_um(शून्य);
शून्य ipc_entry_sysenter(शून्य);
शून्य ipc_entry_syscall_cpu0(शून्य);
शून्य ipc_entry_syscall_cpu1(शून्य);
शून्य ipc_entry_syscall_cpu2(शून्य);
शून्य ipc_entry_syscall_cpu3(शून्य);
शून्य ipc_entry_syscall_cpu4(शून्य);
शून्य ipc_entry_syscall_cpu5(शून्य);
शून्य ipc_entry_syscall_cpu6(शून्य);
शून्य ipc_entry_syscall_cpu7(शून्य);
शून्य kernel_call_entry_orig(शून्य);
शून्य kernel_call_entry_um(शून्य);
शून्य level0_call(शून्य);

/* exception.c */
संरचना exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

शून्य exception(संरचना exception_frame * frame);

/* klib.S */
__dead शून्य monitor(शून्य);
__dead शून्य reset(शून्य);
__dead शून्य poweroff_vmware_clihlt(शून्य);
__dead शून्य x86_triplefault(शून्य);
reg_t read_cr0(शून्य);
reg_t read_cr2(शून्य);
शून्य write_cr0(unsigned long मूल्य);
unsigned long read_cr4(शून्य);
शून्य write_cr4(unsigned long मूल्य);
शून्य write_cr3(unsigned long मूल्य);
unsigned long read_cpu_flags(शून्य);
phys_bytes vir2phys(शून्य *);
शून्य phys_insb(u16_t port, phys_bytes buf, size_t गिनती);
शून्य phys_insw(u16_t port, phys_bytes buf, size_t गिनती);
शून्य phys_outsb(u16_t port, phys_bytes buf, size_t गिनती);
शून्य phys_outsw(u16_t port, phys_bytes buf, size_t गिनती);
u32_t read_cr3(शून्य);
शून्य reload_cr3(शून्य);
शून्य i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
शून्य reload_ds(शून्य);
शून्य ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
शून्य ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
शून्य fninit(शून्य);
शून्य clts(शून्य);
शून्य fxsave(शून्य *);
शून्य fnsave(शून्य *);
पूर्णाङ्क fxrstor(शून्य *);
पूर्णाङ्क __fxrstor_end(शून्य *);
पूर्णाङ्क frstor(शून्य *);
पूर्णाङ्क __frstor_end(शून्य *);
पूर्णाङ्क __frstor_failure(शून्य *);
unsigned short fnstsw(शून्य);
शून्य fnstcw(unsigned short* cw);
शून्य x86_lgdt(शून्य *);
शून्य x86_lldt(u32_t);
शून्य x86_ltr(u32_t);
शून्य x86_lidt(शून्य *);
शून्य x86_load_kerncs(शून्य);
शून्य x86_load_ds(u32_t);
शून्य x86_load_ss(u32_t);
शून्य x86_load_es(u32_t);
शून्य x86_load_fs(u32_t);
शून्य x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
पूर्णाङ्क usermapped_send_softint(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_receive_softint(endpoint_t src, message *m_ptr, पूर्णाङ्क *status_ptr);
पूर्णाङ्क usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
पूर्णाङ्क usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_notify_softint(endpoint_t dest);
पूर्णाङ्क usermapped_do_kernel_call_softint(message *m_ptr);
पूर्णाङ्क usermapped_senda_softint(asynmsg_t *table, size_t गिनती);

पूर्णाङ्क usermapped_send_syscall(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_receive_syscall(endpoint_t src, message *m_ptr, पूर्णाङ्क *status_ptr);
पूर्णाङ्क usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
पूर्णाङ्क usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_notify_syscall(endpoint_t dest);
पूर्णाङ्क usermapped_do_kernel_call_syscall(message *m_ptr);
पूर्णाङ्क usermapped_senda_syscall(asynmsg_t *table, size_t गिनती);

पूर्णाङ्क usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_receive_sysenter(endpoint_t src, message *m_ptr, पूर्णाङ्क *status_ptr);
पूर्णाङ्क usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
पूर्णाङ्क usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
पूर्णाङ्क usermapped_notify_sysenter(endpoint_t dest);
पूर्णाङ्क usermapped_do_kernel_call_sysenter(message *m_ptr);
पूर्णाङ्क usermapped_senda_sysenter(asynmsg_t *table, size_t गिनती);

शून्य switch_k_ढेर(शून्य * esp, शून्य (* continuation)(शून्य));

शून्य __switch_address_space(संरचना proc * p, संरचना proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

शून्य refresh_tlb(शून्य);

/* multiboot.c */
शून्य multiboot_init(शून्य);

/* protect.c */
संरचना tss_s {
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

शून्य enable_iop(संरचना proc *pp);
u32_t read_cs(शून्य);
u32_t read_ds(शून्य);
u32_t read_ss(शून्य);

शून्य add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
शून्य vm_enable_paging(शून्य);
शून्य cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
शून्य pg_info(reg_t *, u32_t **);
शून्य pg_clear(शून्य);
शून्य pg_identity(kinfo_t *);
phys_bytes pg_load(शून्य);
शून्य pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
पूर्णाङ्क pg_mapkernel(शून्य);
शून्य pg_mapproc(संरचना proc *p, संरचना boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
संरचना gate_table_s {
 शून्य(*gate) (शून्य);
	unsigned वर्ण vec_nr;
	unsigned वर्ण privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
शून्य idt_copy_vectors(संरचना gate_table_s * first);
शून्य idt_copy_vectors_pic(शून्य);
शून्य idt_reload(शून्य);

EXTERN शून्य * k_stacks_start;
बाह्य शून्य * k_stacks;

#define get_k_ढेर_शीर्ष(cpu)	((शून्य *)(((वर्ण*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ढेर_SIZE))

शून्य mfence(शून्य);
#define barrier()	कुरु { mfence(); } यावत्(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(शून्य);
#define get_ढेर_frame(__X)	((reg_t)read_ebp())
#अन्यथा
/* read %ebp directly */
#define get_ढेर_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
पूर्णाङ्क tss_init(unsigned cpu, शून्य * kernel_ढेर);

शून्य int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

शून्य __copy_msg_from_user_end(शून्य);
शून्य __copy_msg_to_user_end(शून्य);
शून्य __user_copy_msg_pointer_failure(शून्य);

पूर्णाङ्क platform_tbl_checksum_ok(शून्य *ptr, unsigned पूर्णाङ्क length);
पूर्णाङ्क platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, शून्य * buff, unsigned size, phys_bytes * phys_addr, पूर्णाङ्क ((*
	cmp_f)(शून्य *)));

/* breakpoints.c */
पूर्णाङ्क breakpoint_set(phys_bytes linaddr, पूर्णाङ्क bp, ध्रुव पूर्णाङ्क flags);
#define BREAKPOINT_गिनती		4
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
#अन्तर्भाव "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
