
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#അന്തര്ഭാവ <machine/vm.h>

#define K_അട്ടി_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
ശൂന്യം hwint00(ശൂന്യം);
ശൂന്യം hwint01(ശൂന്യം);
ശൂന്യം hwint02(ശൂന്യം);
ശൂന്യം hwint03(ശൂന്യം);
ശൂന്യം hwint04(ശൂന്യം);
ശൂന്യം hwint05(ശൂന്യം);
ശൂന്യം hwint06(ശൂന്യം);
ശൂന്യം hwint07(ശൂന്യം);
ശൂന്യം hwint08(ശൂന്യം);
ശൂന്യം hwint09(ശൂന്യം);
ശൂന്യം hwint10(ശൂന്യം);
ശൂന്യം hwint11(ശൂന്യം);
ശൂന്യം hwint12(ശൂന്യം);
ശൂന്യം hwint13(ശൂന്യം);
ശൂന്യം hwint14(ശൂന്യം);
ശൂന്യം hwint15(ശൂന്യം);

/* Exception handlers (real or protected mode), in numerical order. */
ശൂന്യം int00(ശൂന്യം), divide_error (ശൂന്യം);
ശൂന്യം int01(ശൂന്യം), single_step_exception (ശൂന്യം);
ശൂന്യം int02(ശൂന്യം), nmi (ശൂന്യം);
ശൂന്യം int03(ശൂന്യം), breakpoint_exception (ശൂന്യം);
ശൂന്യം int04(ശൂന്യം), overflow (ശൂന്യം);
ശൂന്യം int05(ശൂന്യം), bounds_check (ശൂന്യം);
ശൂന്യം int06(ശൂന്യം), inval_opcode (ശൂന്യം);
ശൂന്യം int07(ശൂന്യം), copr_not_available (ശൂന്യം);
ശൂന്യം double_fault(ശൂന്യം);
ശൂന്യം copr_seg_overrun(ശൂന്യം);
ശൂന്യം inval_tss(ശൂന്യം);
ശൂന്യം segment_not_present(ശൂന്യം);
ശൂന്യം അട്ടി_exception(ശൂന്യം);
ശൂന്യം general_protection(ശൂന്യം);
ശൂന്യം page_fault(ശൂന്യം);
ശൂന്യം copr_error(ശൂന്യം);
ശൂന്യം alignment_check(ശൂന്യം);
ശൂന്യം machine_check(ശൂന്യം);
ശൂന്യം simd_exception(ശൂന്യം);

ശൂന്യം restore_user_context_int(ഘടന proc *);
ശൂന്യം restore_user_context_sysenter(ഘടന proc *);
ശൂന്യം restore_user_context_syscall(ഘടന proc *);

/* Software interrupt handlers, in numerical order. */
ശൂന്യം trp(ശൂന്യം);
ശൂന്യം ipc_entry_softint_orig(ശൂന്യം);
ശൂന്യം ipc_entry_softint_um(ശൂന്യം);
ശൂന്യം ipc_entry_sysenter(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu0(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu1(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu2(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu3(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu4(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu5(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu6(ശൂന്യം);
ശൂന്യം ipc_entry_syscall_cpu7(ശൂന്യം);
ശൂന്യം kernel_call_entry_orig(ശൂന്യം);
ശൂന്യം kernel_call_entry_um(ശൂന്യം);
ശൂന്യം level0_call(ശൂന്യം);

/* exception.c */
ഘടന exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

ശൂന്യം exception(ഘടന exception_frame * frame);

/* klib.S */
__dead ശൂന്യം monitor(ശൂന്യം);
__dead ശൂന്യം reset(ശൂന്യം);
__dead ശൂന്യം poweroff_vmware_clihlt(ശൂന്യം);
__dead ശൂന്യം x86_triplefault(ശൂന്യം);
reg_t read_cr0(ശൂന്യം);
reg_t read_cr2(ശൂന്യം);
ശൂന്യം write_cr0(unsigned long മൂല്യം);
unsigned long read_cr4(ശൂന്യം);
ശൂന്യം write_cr4(unsigned long മൂല്യം);
ശൂന്യം write_cr3(unsigned long മൂല്യം);
unsigned long read_cpu_flags(ശൂന്യം);
phys_bytes vir2phys(ശൂന്യം *);
ശൂന്യം phys_insb(u16_t port, phys_bytes buf, size_t എണ്ണം);
ശൂന്യം phys_insw(u16_t port, phys_bytes buf, size_t എണ്ണം);
ശൂന്യം phys_outsb(u16_t port, phys_bytes buf, size_t എണ്ണം);
ശൂന്യം phys_outsw(u16_t port, phys_bytes buf, size_t എണ്ണം);
u32_t read_cr3(ശൂന്യം);
ശൂന്യം reload_cr3(ശൂന്യം);
ശൂന്യം i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
ശൂന്യം reload_ds(ശൂന്യം);
ശൂന്യം ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
ശൂന്യം ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
ശൂന്യം fninit(ശൂന്യം);
ശൂന്യം clts(ശൂന്യം);
ശൂന്യം fxsave(ശൂന്യം *);
ശൂന്യം fnsave(ശൂന്യം *);
പൂർണ്ണസംഖ്യ fxrstor(ശൂന്യം *);
പൂർണ്ണസംഖ്യ __fxrstor_end(ശൂന്യം *);
പൂർണ്ണസംഖ്യ frstor(ശൂന്യം *);
പൂർണ്ണസംഖ്യ __frstor_end(ശൂന്യം *);
പൂർണ്ണസംഖ്യ __frstor_failure(ശൂന്യം *);
unsigned short fnstsw(ശൂന്യം);
ശൂന്യം fnstcw(unsigned short* cw);
ശൂന്യം x86_lgdt(ശൂന്യം *);
ശൂന്യം x86_lldt(u32_t);
ശൂന്യം x86_ltr(u32_t);
ശൂന്യം x86_lidt(ശൂന്യം *);
ശൂന്യം x86_load_kerncs(ശൂന്യം);
ശൂന്യം x86_load_ds(u32_t);
ശൂന്യം x86_load_ss(u32_t);
ശൂന്യം x86_load_es(u32_t);
ശൂന്യം x86_load_fs(u32_t);
ശൂന്യം x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
പൂർണ്ണസംഖ്യ usermapped_send_softint(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_receive_softint(endpoint_t src, message *m_ptr, പൂർണ്ണസംഖ്യ *status_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_notify_softint(endpoint_t dest);
പൂർണ്ണസംഖ്യ usermapped_do_kernel_call_softint(message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_senda_softint(asynmsg_t *table, size_t എണ്ണം);

പൂർണ്ണസംഖ്യ usermapped_send_syscall(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_receive_syscall(endpoint_t src, message *m_ptr, പൂർണ്ണസംഖ്യ *status_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_notify_syscall(endpoint_t dest);
പൂർണ്ണസംഖ്യ usermapped_do_kernel_call_syscall(message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_senda_syscall(asynmsg_t *table, size_t എണ്ണം);

പൂർണ്ണസംഖ്യ usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_receive_sysenter(endpoint_t src, message *m_ptr, പൂർണ്ണസംഖ്യ *status_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_notify_sysenter(endpoint_t dest);
പൂർണ്ണസംഖ്യ usermapped_do_kernel_call_sysenter(message *m_ptr);
പൂർണ്ണസംഖ്യ usermapped_senda_sysenter(asynmsg_t *table, size_t എണ്ണം);

ശൂന്യം switch_k_അട്ടി(ശൂന്യം * esp, ശൂന്യം (* continuation)(ശൂന്യം));

ശൂന്യം __switch_address_space(ഘടന proc * p, ഘടന proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ശൂന്യം refresh_tlb(ശൂന്യം);

/* multiboot.c */
ശൂന്യം multiboot_init(ശൂന്യം);

/* protect.c */
ഘടന tss_s {
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

ശൂന്യം enable_iop(ഘടന proc *pp);
u32_t read_cs(ശൂന്യം);
u32_t read_ds(ശൂന്യം);
u32_t read_ss(ശൂന്യം);

ശൂന്യം add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
ശൂന്യം vm_enable_paging(ശൂന്യം);
ശൂന്യം cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
ശൂന്യം pg_info(reg_t *, u32_t **);
ശൂന്യം pg_clear(ശൂന്യം);
ശൂന്യം pg_identity(kinfo_t *);
phys_bytes pg_load(ശൂന്യം);
ശൂന്യം pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
പൂർണ്ണസംഖ്യ pg_mapkernel(ശൂന്യം);
ശൂന്യം pg_mapproc(ഘടന proc *p, ഘടന boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ഘടന gate_table_s {
 ശൂന്യം(*gate) (ശൂന്യം);
	unsigned അക്ഷരം vec_nr;
	unsigned അക്ഷരം privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
ശൂന്യം idt_copy_vectors(ഘടന gate_table_s * first);
ശൂന്യം idt_copy_vectors_pic(ശൂന്യം);
ശൂന്യം idt_reload(ശൂന്യം);

EXTERN ശൂന്യം * k_stacks_start;
ബാഹ്യ ശൂന്യം * k_stacks;

#define get_k_അട്ടി_മുകൾ(cpu)	((ശൂന്യം *)(((അക്ഷരം*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_അട്ടി_SIZE))

ശൂന്യം mfence(ശൂന്യം);
#define barrier()	ചെയ്യുക { mfence(); } ആകുമ്പോൾ(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(ശൂന്യം);
#define get_അട്ടി_frame(__X)	((reg_t)read_ebp())
#അല്ലെങ്കിൽ
/* read %ebp directly */
#define get_അട്ടി_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
പൂർണ്ണസംഖ്യ tss_init(unsigned cpu, ശൂന്യം * kernel_അട്ടി);

ശൂന്യം int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

ശൂന്യം __copy_msg_from_user_end(ശൂന്യം);
ശൂന്യം __copy_msg_to_user_end(ശൂന്യം);
ശൂന്യം __user_copy_msg_pointer_failure(ശൂന്യം);

പൂർണ്ണസംഖ്യ platform_tbl_checksum_ok(ശൂന്യം *ptr, unsigned പൂർണ്ണസംഖ്യ length);
പൂർണ്ണസംഖ്യ platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, ശൂന്യം * buff, unsigned size, phys_bytes * phys_addr, പൂർണ്ണസംഖ്യ ((*
	cmp_f)(ശൂന്യം *)));

/* breakpoints.c */
പൂർണ്ണസംഖ്യ breakpoint_set(phys_bytes linaddr, പൂർണ്ണസംഖ്യ bp, സ്ഥിരം പൂർണ്ണസംഖ്യ flags);
#define BREAKPOINT_എണ്ണം		4
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
#അന്തര്ഭാവ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
