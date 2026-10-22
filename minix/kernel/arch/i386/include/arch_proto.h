
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _I386_PROTO_H
#ವ್ಯಾಖ್ಯಾನಿಸು _I386_PROTO_H

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ವ್ಯಾಖ್ಯಾನಿಸು K_ರಾಶಿ_ಗಾತ್ರ	I386_ಪುಟ_ಗಾತ್ರ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

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
ಶೂನ್ಯ int01(ಶೂನ್ಯ), single_ಹೆಜ್ಜೆ_exception (ಶೂನ್ಯ);
ಶೂನ್ಯ int02(ಶೂನ್ಯ), nmi (ಶೂನ್ಯ);
ಶೂನ್ಯ int03(ಶೂನ್ಯ), breakpoint_exception (ಶೂನ್ಯ);
ಶೂನ್ಯ int04(ಶೂನ್ಯ), ಅತಿಪ್ರವಾಹ (ಶೂನ್ಯ);
ಶೂನ್ಯ int05(ಶೂನ್ಯ), bounds_check (ಶೂನ್ಯ);
ಶೂನ್ಯ int06(ಶೂನ್ಯ), inval_opcode (ಶೂನ್ಯ);
ಶೂನ್ಯ int07(ಶೂನ್ಯ), copr_not_ಲಭ್ಯ (ಶೂನ್ಯ);
ಶೂನ್ಯ double_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ copr_seg_overrun(ಶೂನ್ಯ);
ಶೂನ್ಯ inval_tss(ಶೂನ್ಯ);
ಶೂನ್ಯ segment_not_present(ಶೂನ್ಯ);
ಶೂನ್ಯ ರಾಶಿ_exception(ಶೂನ್ಯ);
ಶೂನ್ಯ general_protection(ಶೂನ್ಯ);
ಶೂನ್ಯ ಪುಟ_fault(ಶೂನ್ಯ);
ಶೂನ್ಯ copr_error(ಶೂನ್ಯ);
ಶೂನ್ಯ alignment_check(ಶೂನ್ಯ);
ಶೂನ್ಯ machine_check(ಶೂನ್ಯ);
ಶೂನ್ಯ simd_exception(ಶೂನ್ಯ);

ಶೂನ್ಯ restore_user_context_int(ರಚನೆ proc *);
ಶೂನ್ಯ restore_user_context_sysenter(ರಚನೆ proc *);
ಶೂನ್ಯ restore_user_context_syscall(ರಚನೆ proc *);

/* Software interrupt handlers, in numerical order. */
ಶೂನ್ಯ trp(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_softint_orig(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_softint_um(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_sysenter(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu0(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu1(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu2(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu3(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu4(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu5(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu6(ಶೂನ್ಯ);
ಶೂನ್ಯ ipc_ನಮೂದು_syscall_cpu7(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call_ನಮೂದು_orig(ಶೂನ್ಯ);
ಶೂನ್ಯ kernel_call_ನಮೂದು_um(ಶೂನ್ಯ);
ಶೂನ್ಯ level0_call(ಶೂನ್ಯ);

/* exception.c */
ರಚನೆ exception_ಚೌಕಟ್ಟು {
	reg_t	ಸದಿಶ;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

ಶೂನ್ಯ exception(ರಚನೆ exception_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು);

/* klib.S */
__dead ಶೂನ್ಯ monitor(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ reset(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ poweroff_vmware_clihlt(ಶೂನ್ಯ);
__dead ಶೂನ್ಯ x86_triplefault(ಶೂನ್ಯ);
reg_t read_cr0(ಶೂನ್ಯ);
reg_t read_cr2(ಶೂನ್ಯ);
ಶೂನ್ಯ write_cr0(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ read_cr4(ಶೂನ್ಯ);
ಶೂನ್ಯ write_cr4(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
ಶೂನ್ಯ write_cr3(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ read_cpu_flags(ಶೂನ್ಯ);
phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *);
ಶೂನ್ಯ phys_insb(u16_t ಪೋರ್ಟ್, phys_ಬೈಟುಗಳು buf, size_t ಎಣಿಕೆ_2);
ಶೂನ್ಯ phys_insw(u16_t ಪೋರ್ಟ್, phys_ಬೈಟುಗಳು buf, size_t ಎಣಿಕೆ_2);
ಶೂನ್ಯ phys_outsb(u16_t ಪೋರ್ಟ್, phys_ಬೈಟುಗಳು buf, size_t ಎಣಿಕೆ_2);
ಶೂನ್ಯ phys_outsw(u16_t ಪೋರ್ಟ್, phys_ಬೈಟುಗಳು buf, size_t ಎಣಿಕೆ_2);
u32_t read_cr3(ಶೂನ್ಯ);
ಶೂನ್ಯ reload_cr3(ಶೂನ್ಯ);
ಶೂನ್ಯ i386_invlpg(phys_ಬೈಟುಗಳು linaddr);
vir_ಬೈಟುಗಳು phys_memset(phys_ಬೈಟುಗಳು ph, u32_t c, phys_ಬೈಟುಗಳು ಬೈಟುಗಳು);
ಶೂನ್ಯ reload_ds(ಶೂನ್ಯ);
ಶೂನ್ಯ ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
ಶೂನ್ಯ ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
ಶೂನ್ಯ fninit(ಶೂನ್ಯ);
ಶೂನ್ಯ clts(ಶೂನ್ಯ);
ಶೂನ್ಯ fxsave(ಶೂನ್ಯ *);
ಶೂನ್ಯ fnsave(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಂಕ fxrstor(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಂಕ __fxrstor_end(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಂಕ frstor(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಂಕ __frstor_end(ಶೂನ್ಯ *);
ಪೂರ್ಣಾಂಕ __frstor_failure(ಶೂನ್ಯ *);
ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ fnstsw(ಶೂನ್ಯ);
ಶೂನ್ಯ fnstcw(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ* cw);
ಶೂನ್ಯ x86_lgdt(ಶೂನ್ಯ *);
ಶೂನ್ಯ x86_lldt(u32_t);
ಶೂನ್ಯ x86_ltr(u32_t);
ಶೂನ್ಯ x86_lidt(ಶೂನ್ಯ *);
ಶೂನ್ಯ x86_ತುಂಬು_kerncs(ಶೂನ್ಯ);
ಶೂನ್ಯ x86_ತುಂಬು_ds(u32_t);
ಶೂನ್ಯ x86_ತುಂಬು_ss(u32_t);
ಶೂನ್ಯ x86_ತುಂಬು_es(u32_t);
ಶೂನ್ಯ x86_ತುಂಬು_fs(u32_t);
ಶೂನ್ಯ x86_ತುಂಬು_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
ಪೂರ್ಣಾಂಕ usermapped_send_softint(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_receive_softint(endpoint_t src, ಸಂದೇಶ *m_ptr, ಪೂರ್ಣಾಂಕ *ಸ್ಥಿತಿ_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendrec_softint(endpoint_t src_dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendnb_softint(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_notify_softint(endpoint_t dest);
ಪೂರ್ಣಾಂಕ usermapped_do_kernel_call_softint(ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_senda_softint(asynmsg_t *ಕೋಷ್ಟಕ, size_t ಎಣಿಕೆ_2);

ಪೂರ್ಣಾಂಕ usermapped_send_syscall(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_receive_syscall(endpoint_t src, ಸಂದೇಶ *m_ptr, ಪೂರ್ಣಾಂಕ *ಸ್ಥಿತಿ_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendrec_syscall(endpoint_t src_dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendnb_syscall(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_notify_syscall(endpoint_t dest);
ಪೂರ್ಣಾಂಕ usermapped_do_kernel_call_syscall(ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_senda_syscall(asynmsg_t *ಕೋಷ್ಟಕ, size_t ಎಣಿಕೆ_2);

ಪೂರ್ಣಾಂಕ usermapped_send_sysenter(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_receive_sysenter(endpoint_t src, ಸಂದೇಶ *m_ptr, ಪೂರ್ಣಾಂಕ *ಸ್ಥಿತಿ_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendrec_sysenter(endpoint_t src_dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_sendnb_sysenter(endpoint_t dest, ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_notify_sysenter(endpoint_t dest);
ಪೂರ್ಣಾಂಕ usermapped_do_kernel_call_sysenter(ಸಂದೇಶ *m_ptr);
ಪೂರ್ಣಾಂಕ usermapped_senda_sysenter(asynmsg_t *ಕೋಷ್ಟಕ, size_t ಎಣಿಕೆ_2);

ಶೂನ್ಯ switch_k_ರಾಶಿ(ಶೂನ್ಯ * esp, ಶೂನ್ಯ (* continuation)(ಶೂನ್ಯ));

ಶೂನ್ಯ __switch_address_space(ರಚನೆ proc * p, ರಚನೆ proc ** __ptproc);
#ವ್ಯಾಖ್ಯಾನಿಸು switch_ವಿಳಾಸ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ಶೂನ್ಯ refresh_tlb(ಶೂನ್ಯ);

/* multiboot.c */
ಶೂನ್ಯ multiboot_init(ಶೂನ್ಯ);

/* protect.c */
ರಚನೆ tss_s {
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

ಶೂನ್ಯ enable_iop(ರಚನೆ proc *pp);
u32_t read_cs(ಶೂನ್ಯ);
u32_t read_ds(ಶೂನ್ಯ);
u32_t read_ss(ಶೂನ್ಯ);

ಶೂನ್ಯ ಕೂಡಿಸು_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_ಬೈಟುಗಳು alloc_ಅತಿಕಡಿಮೆ(kinfo_t *cbi, phys_ಬೈಟುಗಳು len);
ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ);
ಶೂನ್ಯ cut_memmap(kinfo_t *cbi, phys_ಬೈಟುಗಳು ಆರಂಭ, phys_ಬೈಟುಗಳು ಕೊನೆ);
phys_ಬೈಟುಗಳು pg_roundup(phys_ಬೈಟುಗಳು b);
ಶೂನ್ಯ pg_info(reg_t *, u32_t **);
ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_ತಟಸ್ಥ(kinfo_t *);
phys_ಬೈಟುಗಳು pg_ತುಂಬು(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_map(phys_ಬೈಟುಗಳು phys, vir_ಬೈಟುಗಳು vaddr, vir_ಬೈಟುಗಳು vaddr_ಕೊನೆ, kinfo_t *cbi);
ಪೂರ್ಣಾಂಕ pg_mapkernel(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_mapproc(ರಚನೆ proc *p, ರಚನೆ boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ರಚನೆ gate_ಕೋಷ್ಟಕ_s {
 ಶೂನ್ಯ(*gate) (ಶೂನ್ಯ);
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ vec_nr;
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
ಶೂನ್ಯ idt_copy_vectors(ರಚನೆ gate_ಕೋಷ್ಟಕ_s * ಮೊದಲ);
ಶೂನ್ಯ idt_copy_vectors_pic(ಶೂನ್ಯ);
ಶೂನ್ಯ idt_reload(ಶೂನ್ಯ);

EXTERN ಶೂನ್ಯ * k_stacks_ಆರಂಭ;
ಬಾಹ್ಯ ಶೂನ್ಯ * k_stacks;

#ವ್ಯಾಖ್ಯಾನಿಸು get_k_ರಾಶಿ_ತುದಿ(cpu)	((ಶೂನ್ಯ *)(((ಅಕ್ಷರ*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ರಾಶಿ_ಗಾತ್ರ))

ಶೂನ್ಯ mfence(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿಸು barrier()	ಮಾಡು { mfence(); } ಆಗ(0)


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿಸು get_ರಾಶಿ_ಚೌಕಟ್ಟು(__X)	((reg_t)read_ebp())
#ಇಲ್ಲದಿದ್ದರೆ
/* read %ebp directly */
#ವ್ಯಾಖ್ಯಾನಿಸು get_ರಾಶಿ_ಚೌಕಟ್ಟು(__X)	((reg_t)__builtin_frame_address(0))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
ಪೂರ್ಣಾಂಕ tss_init(ಚಿಹ್ನೆರಹಿತ cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ);

ಶೂನ್ಯ int_gate_idt(ಚಿಹ್ನೆರಹಿತ vec_nr, vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ dpl_type);

ಶೂನ್ಯ __copy_msg_from_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __copy_msg_to_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __user_copy_msg_pointer_failure(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ platform_tbl_checksum_ok(ಶೂನ್ಯ *ptr, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ);
ಪೂರ್ಣಾಂಕ platform_tbl_ptr(phys_ಬೈಟುಗಳು ಆರಂಭ, phys_ಬೈಟುಗಳು ಕೊನೆ, ಚಿಹ್ನೆರಹಿತ
	increment, ಶೂನ್ಯ * buff, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, phys_ಬೈಟುಗಳು * phys_addr, ಪೂರ್ಣಾಂಕ ((*
	cmp_f)(ಶೂನ್ಯ *)));

/* breakpoints.c */
ಪೂರ್ಣಾಂಕ breakpoint_ಗಣ(phys_ಬೈಟುಗಳು linaddr, ಪೂರ್ಣಾಂಕ bp, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags);
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_ಎಣಿಕೆ		4
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_RW_ಮುಖವಾಡ		(3 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_RW_RW		(2 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_LEN_ಮುಖವಾಡ	(3 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_LEN_1		(0 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_LEN_2		(1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_LEN_4		(2 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_ಬಹುಲಕ_ಮುಖವಾಡ	(3 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_ಬಹುಲಕ_OFF	(0 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_ಬಹುಲಕ_LOCAL	(1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು BREAKPOINT_FLAG_ಬಹುಲಕ_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ
