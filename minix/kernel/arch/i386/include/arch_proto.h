
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _I386_PROTO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _I386_PROTO_H

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਢੇਰ_ਮਾਪ	I386_ਪੰਨਾ_ਮਾਪ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

/* Hardware interrupt handlers. */
ਖਾਲੀ hwint00(ਖਾਲੀ);
ਖਾਲੀ hwint01(ਖਾਲੀ);
ਖਾਲੀ hwint02(ਖਾਲੀ);
ਖਾਲੀ hwint03(ਖਾਲੀ);
ਖਾਲੀ hwint04(ਖਾਲੀ);
ਖਾਲੀ hwint05(ਖਾਲੀ);
ਖਾਲੀ hwint06(ਖਾਲੀ);
ਖਾਲੀ hwint07(ਖਾਲੀ);
ਖਾਲੀ hwint08(ਖਾਲੀ);
ਖਾਲੀ hwint09(ਖਾਲੀ);
ਖਾਲੀ hwint10(ਖਾਲੀ);
ਖਾਲੀ hwint11(ਖਾਲੀ);
ਖਾਲੀ hwint12(ਖਾਲੀ);
ਖਾਲੀ hwint13(ਖਾਲੀ);
ਖਾਲੀ hwint14(ਖਾਲੀ);
ਖਾਲੀ hwint15(ਖਾਲੀ);

/* Exception handlers (real or protected mode), in numerical order. */
ਖਾਲੀ int00(ਖਾਲੀ), divide_error (ਖਾਲੀ);
ਖਾਲੀ int01(ਖਾਲੀ), single_ਕਦਮ_exception (ਖਾਲੀ);
ਖਾਲੀ int02(ਖਾਲੀ), nmi (ਖਾਲੀ);
ਖਾਲੀ int03(ਖਾਲੀ), breakpoint_exception (ਖਾਲੀ);
ਖਾਲੀ int04(ਖਾਲੀ), ਉਛਾਲ (ਖਾਲੀ);
ਖਾਲੀ int05(ਖਾਲੀ), bounds_check (ਖਾਲੀ);
ਖਾਲੀ int06(ਖਾਲੀ), inval_opcode (ਖਾਲੀ);
ਖਾਲੀ int07(ਖਾਲੀ), copr_not_ਉਪਲਬਧ (ਖਾਲੀ);
ਖਾਲੀ double_fault(ਖਾਲੀ);
ਖਾਲੀ copr_seg_overrun(ਖਾਲੀ);
ਖਾਲੀ inval_tss(ਖਾਲੀ);
ਖਾਲੀ segment_not_present(ਖਾਲੀ);
ਖਾਲੀ ਢੇਰ_exception(ਖਾਲੀ);
ਖਾਲੀ general_protection(ਖਾਲੀ);
ਖਾਲੀ ਪੰਨਾ_fault(ਖਾਲੀ);
ਖਾਲੀ copr_error(ਖਾਲੀ);
ਖਾਲੀ alignment_check(ਖਾਲੀ);
ਖਾਲੀ machine_check(ਖਾਲੀ);
ਖਾਲੀ simd_exception(ਖਾਲੀ);

ਖਾਲੀ restore_user_context_int(ਬਣਤਰ proc *);
ਖਾਲੀ restore_user_context_sysenter(ਬਣਤਰ proc *);
ਖਾਲੀ restore_user_context_syscall(ਬਣਤਰ proc *);

/* Software interrupt handlers, in numerical order. */
ਖਾਲੀ trp(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_softint_orig(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_softint_um(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_sysenter(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu0(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu1(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu2(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu3(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu4(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu5(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu6(ਖਾਲੀ);
ਖਾਲੀ ipc_ਇੰਦਰਾਜ_syscall_cpu7(ਖਾਲੀ);
ਖਾਲੀ kernel_call_ਇੰਦਰਾਜ_orig(ਖਾਲੀ);
ਖਾਲੀ kernel_call_ਇੰਦਰਾਜ_um(ਖਾਲੀ);
ਖਾਲੀ level0_call(ਖਾਲੀ);

/* exception.c */
ਬਣਤਰ exception_ਚੌਖਟਾ {
	reg_t	ਸਦਿਸ਼;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

ਖਾਲੀ exception(ਬਣਤਰ exception_ਚੌਖਟਾ * ਚੌਖਟਾ);

/* klib.S */
__dead ਖਾਲੀ monitor(ਖਾਲੀ);
__dead ਖਾਲੀ reset(ਖਾਲੀ);
__dead ਖਾਲੀ poweroff_vmware_clihlt(ਖਾਲੀ);
__dead ਖਾਲੀ x86_triplefault(ਖਾਲੀ);
reg_t read_cr0(ਖਾਲੀ);
reg_t read_cr2(ਖਾਲੀ);
ਖਾਲੀ write_cr0(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ read_cr4(ਖਾਲੀ);
ਖਾਲੀ write_cr4(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
ਖਾਲੀ write_cr3(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ read_cpu_flags(ਖਾਲੀ);
phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *);
ਖਾਲੀ phys_insb(u16_t ਪੋਰਟ, phys_ਬਾਈਟਾਂ buf, size_t ਗਿਣਤੀ_2);
ਖਾਲੀ phys_insw(u16_t ਪੋਰਟ, phys_ਬਾਈਟਾਂ buf, size_t ਗਿਣਤੀ_2);
ਖਾਲੀ phys_outsb(u16_t ਪੋਰਟ, phys_ਬਾਈਟਾਂ buf, size_t ਗਿਣਤੀ_2);
ਖਾਲੀ phys_outsw(u16_t ਪੋਰਟ, phys_ਬਾਈਟਾਂ buf, size_t ਗਿਣਤੀ_2);
u32_t read_cr3(ਖਾਲੀ);
ਖਾਲੀ reload_cr3(ਖਾਲੀ);
ਖਾਲੀ i386_invlpg(phys_ਬਾਈਟਾਂ linaddr);
vir_ਬਾਈਟਾਂ phys_memset(phys_ਬਾਈਟਾਂ ph, u32_t c, phys_ਬਾਈਟਾਂ ਬਾਈਟਾਂ);
ਖਾਲੀ reload_ds(ਖਾਲੀ);
ਖਾਲੀ ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
ਖਾਲੀ ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
ਖਾਲੀ fninit(ਖਾਲੀ);
ਖਾਲੀ clts(ਖਾਲੀ);
ਖਾਲੀ fxsave(ਖਾਲੀ *);
ਖਾਲੀ fnsave(ਖਾਲੀ *);
ਪੂਰਨ fxrstor(ਖਾਲੀ *);
ਪੂਰਨ __fxrstor_end(ਖਾਲੀ *);
ਪੂਰਨ frstor(ਖਾਲੀ *);
ਪੂਰਨ __frstor_end(ਖਾਲੀ *);
ਪੂਰਨ __frstor_failure(ਖਾਲੀ *);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ fnstsw(ਖਾਲੀ);
ਖਾਲੀ fnstcw(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ* cw);
ਖਾਲੀ x86_lgdt(ਖਾਲੀ *);
ਖਾਲੀ x86_lldt(u32_t);
ਖਾਲੀ x86_ltr(u32_t);
ਖਾਲੀ x86_lidt(ਖਾਲੀ *);
ਖਾਲੀ x86_ਲੱਦੋ_kerncs(ਖਾਲੀ);
ਖਾਲੀ x86_ਲੱਦੋ_ds(u32_t);
ਖਾਲੀ x86_ਲੱਦੋ_ss(u32_t);
ਖਾਲੀ x86_ਲੱਦੋ_es(u32_t);
ਖਾਲੀ x86_ਲੱਦੋ_fs(u32_t);
ਖਾਲੀ x86_ਲੱਦੋ_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
ਪੂਰਨ usermapped_send_softint(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_receive_softint(endpoint_t src, ਸੁਨੇਹਾ *m_ptr, ਪੂਰਨ *ਸਥਿਤੀ_ptr);
ਪੂਰਨ usermapped_sendrec_softint(endpoint_t src_dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_sendnb_softint(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_notify_softint(endpoint_t dest);
ਪੂਰਨ usermapped_do_kernel_call_softint(ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_senda_softint(asynmsg_t *ਸਾਰਣੀ, size_t ਗਿਣਤੀ_2);

ਪੂਰਨ usermapped_send_syscall(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_receive_syscall(endpoint_t src, ਸੁਨੇਹਾ *m_ptr, ਪੂਰਨ *ਸਥਿਤੀ_ptr);
ਪੂਰਨ usermapped_sendrec_syscall(endpoint_t src_dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_sendnb_syscall(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_notify_syscall(endpoint_t dest);
ਪੂਰਨ usermapped_do_kernel_call_syscall(ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_senda_syscall(asynmsg_t *ਸਾਰਣੀ, size_t ਗਿਣਤੀ_2);

ਪੂਰਨ usermapped_send_sysenter(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_receive_sysenter(endpoint_t src, ਸੁਨੇਹਾ *m_ptr, ਪੂਰਨ *ਸਥਿਤੀ_ptr);
ਪੂਰਨ usermapped_sendrec_sysenter(endpoint_t src_dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_sendnb_sysenter(endpoint_t dest, ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_notify_sysenter(endpoint_t dest);
ਪੂਰਨ usermapped_do_kernel_call_sysenter(ਸੁਨੇਹਾ *m_ptr);
ਪੂਰਨ usermapped_senda_sysenter(asynmsg_t *ਸਾਰਣੀ, size_t ਗਿਣਤੀ_2);

ਖਾਲੀ switch_k_ਢੇਰ(ਖਾਲੀ * esp, ਖਾਲੀ (* continuation)(ਖਾਲੀ));

ਖਾਲੀ __switch_address_space(ਬਣਤਰ proc * p, ਬਣਤਰ proc ** __ptproc);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switch_ਪਤਾ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ਖਾਲੀ refresh_tlb(ਖਾਲੀ);

/* multiboot.c */
ਖਾਲੀ multiboot_init(ਖਾਲੀ);

/* protect.c */
ਬਣਤਰ tss_s {
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

ਖਾਲੀ enable_iop(ਬਣਤਰ proc *pp);
u32_t read_cs(ਖਾਲੀ);
u32_t read_ds(ਖਾਲੀ);
u32_t read_ss(ਖਾਲੀ);

ਖਾਲੀ ਜੋੜੋ_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_ਬਾਈਟਾਂ alloc_ਸਭ_ਤੋਂ_ਨੀਵਾਂ(kinfo_t *cbi, phys_ਬਾਈਟਾਂ len);
ਖਾਲੀ vm_enable_paging(ਖਾਲੀ);
ਖਾਲੀ cut_memmap(kinfo_t *cbi, phys_ਬਾਈਟਾਂ ਸ਼ੁਰੂ, phys_ਬਾਈਟਾਂ ਸਿਰਾ);
phys_ਬਾਈਟਾਂ pg_roundup(phys_ਬਾਈਟਾਂ b);
ਖਾਲੀ pg_info(reg_t *, u32_t **);
ਖਾਲੀ pg_clear(ਖਾਲੀ);
ਖਾਲੀ pg_ਪਛਾਣ_ਤੱਤ(kinfo_t *);
phys_ਬਾਈਟਾਂ pg_ਲੱਦੋ(ਖਾਲੀ);
ਖਾਲੀ pg_map(phys_ਬਾਈਟਾਂ phys, vir_ਬਾਈਟਾਂ vaddr, vir_ਬਾਈਟਾਂ vaddr_ਸਿਰਾ, kinfo_t *cbi);
ਪੂਰਨ pg_mapkernel(ਖਾਲੀ);
ਖਾਲੀ pg_mapproc(ਬਣਤਰ proc *p, ਬਣਤਰ boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ਬਣਤਰ gate_ਸਾਰਣੀ_s {
 ਖਾਲੀ(*gate) (ਖਾਲੀ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ vec_nr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
ਖਾਲੀ idt_copy_vectors(ਬਣਤਰ gate_ਸਾਰਣੀ_s * ਪਹਿਲਾ);
ਖਾਲੀ idt_copy_vectors_pic(ਖਾਲੀ);
ਖਾਲੀ idt_reload(ਖਾਲੀ);

EXTERN ਖਾਲੀ * k_stacks_ਸ਼ੁਰੂ;
ਬਾਹਰੀ ਖਾਲੀ * k_stacks;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_k_ਢੇਰ_ਸਿਖਰ(cpu)	((ਖਾਲੀ *)(((ਅੱਖਰ*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ਢੇਰ_ਮਾਪ))

ਖਾਲੀ mfence(ਖਾਲੀ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ barrier()	ਕਰੋ { mfence(); } ਜਦੋਂ(0)


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(ਖਾਲੀ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_ਢੇਰ_ਚੌਖਟਾ(__X)	((reg_t)read_ebp())
#ਨਹੀਂਤਾਂ
/* read %ebp directly */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_ਢੇਰ_ਚੌਖਟਾ(__X)	((reg_t)__builtin_frame_address(0))
#ਜੇ_ਅੰਤ

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
ਪੂਰਨ tss_init(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਖਾਲੀ * kernel_ਢੇਰ);

ਖਾਲੀ int_gate_idt(ਬਿਨਾਨਿਸ਼ਾਨੀ vec_nr, vir_ਬਾਈਟਾਂ ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ dpl_type);

ਖਾਲੀ __copy_msg_from_user_end(ਖਾਲੀ);
ਖਾਲੀ __copy_msg_to_user_end(ਖਾਲੀ);
ਖਾਲੀ __user_copy_msg_pointer_failure(ਖਾਲੀ);

ਪੂਰਨ platform_tbl_checksum_ok(ਖਾਲੀ *ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ);
ਪੂਰਨ platform_tbl_ptr(phys_ਬਾਈਟਾਂ ਸ਼ੁਰੂ, phys_ਬਾਈਟਾਂ ਸਿਰਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ
	increment, ਖਾਲੀ * buff, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, phys_ਬਾਈਟਾਂ * phys_addr, ਪੂਰਨ ((*
	cmp_f)(ਖਾਲੀ *)));

/* breakpoints.c */
ਪੂਰਨ breakpoint_ਸੈੱਟ(phys_ਬਾਈਟਾਂ linaddr, ਪੂਰਨ bp, ਅਚਲ ਪੂਰਨ flags);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_ਗਿਣਤੀ		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_RW_ਨਕਾਬ		(3 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_RW_RW		(2 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_LEN_ਨਕਾਬ	(3 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_LEN_1		(0 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_LEN_2		(1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_LEN_4		(2 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_ਬਹੁਲਕ_ਨਕਾਬ	(3 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_ਬਹੁਲਕ_OFF	(0 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_ਬਹੁਲਕ_LOCAL	(1 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BREAKPOINT_FLAG_ਬਹੁਲਕ_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ
