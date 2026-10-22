
#ifndef _I386_PROTO_H
#define _I386_PROTO_H

#அந்தர்பாவ <machine/vm.h>

#define K_அடுக்கு_SIZE	I386_PAGE_SIZE

#ifndef __ASSEMBLY__

/* Hardware interrupt handlers. */
வெறுமை hwint00(வெறுமை);
வெறுமை hwint01(வெறுமை);
வெறுமை hwint02(வெறுமை);
வெறுமை hwint03(வெறுமை);
வெறுமை hwint04(வெறுமை);
வெறுமை hwint05(வெறுமை);
வெறுமை hwint06(வெறுமை);
வெறுமை hwint07(வெறுமை);
வெறுமை hwint08(வெறுமை);
வெறுமை hwint09(வெறுமை);
வெறுமை hwint10(வெறுமை);
வெறுமை hwint11(வெறுமை);
வெறுமை hwint12(வெறுமை);
வெறுமை hwint13(வெறுமை);
வெறுமை hwint14(வெறுமை);
வெறுமை hwint15(வெறுமை);

/* Exception handlers (real or protected mode), in numerical order. */
வெறுமை int00(வெறுமை), divide_error (வெறுமை);
வெறுமை int01(வெறுமை), single_step_exception (வெறுமை);
வெறுமை int02(வெறுமை), nmi (வெறுமை);
வெறுமை int03(வெறுமை), breakpoint_exception (வெறுமை);
வெறுமை int04(வெறுமை), overflow (வெறுமை);
வெறுமை int05(வெறுமை), bounds_check (வெறுமை);
வெறுமை int06(வெறுமை), inval_opcode (வெறுமை);
வெறுமை int07(வெறுமை), copr_not_available (வெறுமை);
வெறுமை double_fault(வெறுமை);
வெறுமை copr_seg_overrun(வெறுமை);
வெறுமை inval_tss(வெறுமை);
வெறுமை segment_not_present(வெறுமை);
வெறுமை அடுக்கு_exception(வெறுமை);
வெறுமை general_protection(வெறுமை);
வெறுமை page_fault(வெறுமை);
வெறுமை copr_error(வெறுமை);
வெறுமை alignment_check(வெறுமை);
வெறுமை machine_check(வெறுமை);
வெறுமை simd_exception(வெறுமை);

வெறுமை restore_user_context_int(கட்டமைப்பு proc *);
வெறுமை restore_user_context_sysenter(கட்டமைப்பு proc *);
வெறுமை restore_user_context_syscall(கட்டமைப்பு proc *);

/* Software interrupt handlers, in numerical order. */
வெறுமை trp(வெறுமை);
வெறுமை ipc_entry_softint_orig(வெறுமை);
வெறுமை ipc_entry_softint_um(வெறுமை);
வெறுமை ipc_entry_sysenter(வெறுமை);
வெறுமை ipc_entry_syscall_cpu0(வெறுமை);
வெறுமை ipc_entry_syscall_cpu1(வெறுமை);
வெறுமை ipc_entry_syscall_cpu2(வெறுமை);
வெறுமை ipc_entry_syscall_cpu3(வெறுமை);
வெறுமை ipc_entry_syscall_cpu4(வெறுமை);
வெறுமை ipc_entry_syscall_cpu5(வெறுமை);
வெறுமை ipc_entry_syscall_cpu6(வெறுமை);
வெறுமை ipc_entry_syscall_cpu7(வெறுமை);
வெறுமை kernel_call_entry_orig(வெறுமை);
வெறுமை kernel_call_entry_um(வெறுமை);
வெறுமை level0_call(வெறுமை);

/* exception.c */
கட்டமைப்பு exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

வெறுமை exception(கட்டமைப்பு exception_frame * frame);

/* klib.S */
__dead வெறுமை monitor(வெறுமை);
__dead வெறுமை reset(வெறுமை);
__dead வெறுமை poweroff_vmware_clihlt(வெறுமை);
__dead வெறுமை x86_triplefault(வெறுமை);
reg_t read_cr0(வெறுமை);
reg_t read_cr2(வெறுமை);
வெறுமை write_cr0(unsigned long மதிப்பு);
unsigned long read_cr4(வெறுமை);
வெறுமை write_cr4(unsigned long மதிப்பு);
வெறுமை write_cr3(unsigned long மதிப்பு);
unsigned long read_cpu_flags(வெறுமை);
phys_bytes vir2phys(வெறுமை *);
வெறுமை phys_insb(u16_t port, phys_bytes buf, size_t மொத்தம்);
வெறுமை phys_insw(u16_t port, phys_bytes buf, size_t மொத்தம்);
வெறுமை phys_outsb(u16_t port, phys_bytes buf, size_t மொத்தம்);
வெறுமை phys_outsw(u16_t port, phys_bytes buf, size_t மொத்தம்);
u32_t read_cr3(வெறுமை);
வெறுமை reload_cr3(வெறுமை);
வெறுமை i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
வெறுமை reload_ds(வெறுமை);
வெறுமை ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
வெறுமை ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
வெறுமை fninit(வெறுமை);
வெறுமை clts(வெறுமை);
வெறுமை fxsave(வெறுமை *);
வெறுமை fnsave(வெறுமை *);
முழு fxrstor(வெறுமை *);
முழு __fxrstor_end(வெறுமை *);
முழு frstor(வெறுமை *);
முழு __frstor_end(வெறுமை *);
முழு __frstor_failure(வெறுமை *);
unsigned short fnstsw(வெறுமை);
வெறுமை fnstcw(unsigned short* cw);
வெறுமை x86_lgdt(வெறுமை *);
வெறுமை x86_lldt(u32_t);
வெறுமை x86_ltr(u32_t);
வெறுமை x86_lidt(வெறுமை *);
வெறுமை x86_load_kerncs(வெறுமை);
வெறுமை x86_load_ds(u32_t);
வெறுமை x86_load_ss(u32_t);
வெறுமை x86_load_es(u32_t);
வெறுமை x86_load_fs(u32_t);
வெறுமை x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
முழு usermapped_send_softint(endpoint_t dest, message *m_ptr);
முழு usermapped_receive_softint(endpoint_t src, message *m_ptr, முழு *status_ptr);
முழு usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
முழு usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
முழு usermapped_notify_softint(endpoint_t dest);
முழு usermapped_do_kernel_call_softint(message *m_ptr);
முழு usermapped_senda_softint(asynmsg_t *table, size_t மொத்தம்);

முழு usermapped_send_syscall(endpoint_t dest, message *m_ptr);
முழு usermapped_receive_syscall(endpoint_t src, message *m_ptr, முழு *status_ptr);
முழு usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
முழு usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
முழு usermapped_notify_syscall(endpoint_t dest);
முழு usermapped_do_kernel_call_syscall(message *m_ptr);
முழு usermapped_senda_syscall(asynmsg_t *table, size_t மொத்தம்);

முழு usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
முழு usermapped_receive_sysenter(endpoint_t src, message *m_ptr, முழு *status_ptr);
முழு usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
முழு usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
முழு usermapped_notify_sysenter(endpoint_t dest);
முழு usermapped_do_kernel_call_sysenter(message *m_ptr);
முழு usermapped_senda_sysenter(asynmsg_t *table, size_t மொத்தம்);

வெறுமை switch_k_அடுக்கு(வெறுமை * esp, வெறுமை (* continuation)(வெறுமை));

வெறுமை __switch_address_space(கட்டமைப்பு proc * p, கட்டமைப்பு proc ** __ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

வெறுமை refresh_tlb(வெறுமை);

/* multiboot.c */
வெறுமை multiboot_init(வெறுமை);

/* protect.c */
கட்டமைப்பு tss_s {
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

வெறுமை enable_iop(கட்டமைப்பு proc *pp);
u32_t read_cs(வெறுமை);
u32_t read_ds(வெறுமை);
u32_t read_ss(வெறுமை);

வெறுமை add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
வெறுமை vm_enable_paging(வெறுமை);
வெறுமை cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
வெறுமை pg_info(reg_t *, u32_t **);
வெறுமை pg_clear(வெறுமை);
வெறுமை pg_identity(kinfo_t *);
phys_bytes pg_load(வெறுமை);
வெறுமை pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
முழு pg_mapkernel(வெறுமை);
வெறுமை pg_mapproc(கட்டமைப்பு proc *p, கட்டமைப்பு boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
கட்டமைப்பு gate_table_s {
 வெறுமை(*gate) (வெறுமை);
	unsigned எழுத்து vec_nr;
	unsigned எழுத்து privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
வெறுமை idt_copy_vectors(கட்டமைப்பு gate_table_s * first);
வெறுமை idt_copy_vectors_pic(வெறுமை);
வெறுமை idt_reload(வெறுமை);

EXTERN வெறுமை * k_stacks_start;
வெளி வெறுமை * k_stacks;

#define get_k_அடுக்கு_உச்சி(cpu)	((வெறுமை *)(((எழுத்து*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_அடுக்கு_SIZE))

வெறுமை mfence(வெறுமை);
#define barrier()	செய் { mfence(); } போது(0)


#ifndef __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(வெறுமை);
#define get_அடுக்கு_frame(__X)	((reg_t)read_ebp())
#இல்லையெனில்
/* read %ebp directly */
#define get_அடுக்கு_frame(__X)	((reg_t)__builtin_frame_address(0))
#endif

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
முழு tss_init(unsigned cpu, வெறுமை * kernel_அடுக்கு);

வெறுமை int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type);

வெறுமை __copy_msg_from_user_end(வெறுமை);
வெறுமை __copy_msg_to_user_end(வெறுமை);
வெறுமை __user_copy_msg_pointer_failure(வெறுமை);

முழு platform_tbl_checksum_ok(வெறுமை *ptr, unsigned முழு length);
முழு platform_tbl_ptr(phys_bytes start, phys_bytes end, unsigned
	increment, வெறுமை * buff, unsigned size, phys_bytes * phys_addr, முழு ((*
	cmp_f)(வெறுமை *)));

/* breakpoints.c */
முழு breakpoint_set(phys_bytes linaddr, முழு bp, மாறிலி முழு flags);
#define BREAKPOINT_மொத்தம்		4
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
#அந்தர்பாவ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
