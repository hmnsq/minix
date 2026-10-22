
#నిర్వచితం_కాకపోతే _I386_PROTO_H
#నిర్వచించు _I386_PROTO_H

#అన్తర్భావ <machine/vm.h>

#నిర్వచించు K_దొంతర_కొలత	I386_పుట_కొలత

#నిర్వచితం_కాకపోతే __ASSEMBLY__

/* Hardware interrupt handlers. */
శూన్య hwint00(శూన్య);
శూన్య hwint01(శూన్య);
శూన్య hwint02(శూన్య);
శూన్య hwint03(శూన్య);
శూన్య hwint04(శూన్య);
శూన్య hwint05(శూన్య);
శూన్య hwint06(శూన్య);
శూన్య hwint07(శూన్య);
శూన్య hwint08(శూన్య);
శూన్య hwint09(శూన్య);
శూన్య hwint10(శూన్య);
శూన్య hwint11(శూన్య);
శూన్య hwint12(శూన్య);
శూన్య hwint13(శూన్య);
శూన్య hwint14(శూన్య);
శూన్య hwint15(శూన్య);

/* Exception handlers (real or protected mode), in numerical order. */
శూన్య int00(శూన్య), divide_error (శూన్య);
శూన్య int01(శూన్య), single_అడుగు_exception (శూన్య);
శూన్య int02(శూన్య), nmi (శూన్య);
శూన్య int03(శూన్య), breakpoint_exception (శూన్య);
శూన్య int04(శూన్య), అధికప్రవాహం (శూన్య);
శూన్య int05(శూన్య), bounds_check (శూన్య);
శూన్య int06(శూన్య), inval_opcode (శూన్య);
శూన్య int07(శూన్య), copr_not_అందుబాటు (శూన్య);
శూన్య double_fault(శూన్య);
శూన్య copr_seg_overrun(శూన్య);
శూన్య inval_tss(శూన్య);
శూన్య segment_not_present(శూన్య);
శూన్య దొంతర_exception(శూన్య);
శూన్య general_protection(శూన్య);
శూన్య పుట_fault(శూన్య);
శూన్య copr_error(శూన్య);
శూన్య alignment_check(శూన్య);
శూన్య machine_check(శూన్య);
శూన్య simd_exception(శూన్య);

శూన్య restore_user_context_int(నిర్మాణం proc *);
శూన్య restore_user_context_sysenter(నిర్మాణం proc *);
శూన్య restore_user_context_syscall(నిర్మాణం proc *);

/* Software interrupt handlers, in numerical order. */
శూన్య trp(శూన్య);
శూన్య ipc_నమోదు_softint_orig(శూన్య);
శూన్య ipc_నమోదు_softint_um(శూన్య);
శూన్య ipc_నమోదు_sysenter(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu0(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu1(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu2(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu3(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu4(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu5(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu6(శూన్య);
శూన్య ipc_నమోదు_syscall_cpu7(శూన్య);
శూన్య kernel_call_నమోదు_orig(శూన్య);
శూన్య kernel_call_నమోదు_um(శూన్య);
శూన్య level0_call(శూన్య);

/* exception.c */
నిర్మాణం exception_చట్రం {
	reg_t	సదిశ;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

శూన్య exception(నిర్మాణం exception_చట్రం * చట్రం);

/* klib.S */
__dead శూన్య monitor(శూన్య);
__dead శూన్య reset(శూన్య);
__dead శూన్య poweroff_vmware_clihlt(శూన్య);
__dead శూన్య x86_triplefault(శూన్య);
reg_t read_cr0(శూన్య);
reg_t read_cr2(శూన్య);
శూన్య write_cr0(గుర్తులేని దీర్ఘ విలువ);
గుర్తులేని దీర్ఘ read_cr4(శూన్య);
శూన్య write_cr4(గుర్తులేని దీర్ఘ విలువ);
శూన్య write_cr3(గుర్తులేని దీర్ఘ విలువ);
గుర్తులేని దీర్ఘ read_cpu_flags(శూన్య);
phys_బైట్లు vir2phys(శూన్య *);
శూన్య phys_insb(u16_t పోర్టు, phys_బైట్లు buf, size_t లెక్క);
శూన్య phys_insw(u16_t పోర్టు, phys_బైట్లు buf, size_t లెక్క);
శూన్య phys_outsb(u16_t పోర్టు, phys_బైట్లు buf, size_t లెక్క);
శూన్య phys_outsw(u16_t పోర్టు, phys_బైట్లు buf, size_t లెక్క);
u32_t read_cr3(శూన్య);
శూన్య reload_cr3(శూన్య);
శూన్య i386_invlpg(phys_బైట్లు linaddr);
vir_బైట్లు phys_memset(phys_బైట్లు ph, u32_t c, phys_బైట్లు బైట్లు);
శూన్య reload_ds(శూన్య);
శూన్య ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
శూన్య ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
శూన్య fninit(శూన్య);
శూన్య clts(శూన్య);
శూన్య fxsave(శూన్య *);
శూన్య fnsave(శూన్య *);
పూర్ణాంకం fxrstor(శూన్య *);
పూర్ణాంకం __fxrstor_end(శూన్య *);
పూర్ణాంకం frstor(శూన్య *);
పూర్ణాంకం __frstor_end(శూన్య *);
పూర్ణాంకం __frstor_failure(శూన్య *);
గుర్తులేని పొట్టి fnstsw(శూన్య);
శూన్య fnstcw(గుర్తులేని పొట్టి* cw);
శూన్య x86_lgdt(శూన్య *);
శూన్య x86_lldt(u32_t);
శూన్య x86_ltr(u32_t);
శూన్య x86_lidt(శూన్య *);
శూన్య x86_ఎక్కించు_kerncs(శూన్య);
శూన్య x86_ఎక్కించు_ds(u32_t);
శూన్య x86_ఎక్కించు_ss(u32_t);
శూన్య x86_ఎక్కించు_es(u32_t);
శూన్య x86_ఎక్కించు_fs(u32_t);
శూన్య x86_ఎక్కించు_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
పూర్ణాంకం usermapped_send_softint(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_receive_softint(endpoint_t src, సందేశం *m_ptr, పూర్ణాంకం *స్థితి_ptr);
పూర్ణాంకం usermapped_sendrec_softint(endpoint_t src_dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_sendnb_softint(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_notify_softint(endpoint_t dest);
పూర్ణాంకం usermapped_do_kernel_call_softint(సందేశం *m_ptr);
పూర్ణాంకం usermapped_senda_softint(asynmsg_t *పట్టిక, size_t లెక్క);

పూర్ణాంకం usermapped_send_syscall(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_receive_syscall(endpoint_t src, సందేశం *m_ptr, పూర్ణాంకం *స్థితి_ptr);
పూర్ణాంకం usermapped_sendrec_syscall(endpoint_t src_dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_sendnb_syscall(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_notify_syscall(endpoint_t dest);
పూర్ణాంకం usermapped_do_kernel_call_syscall(సందేశం *m_ptr);
పూర్ణాంకం usermapped_senda_syscall(asynmsg_t *పట్టిక, size_t లెక్క);

పూర్ణాంకం usermapped_send_sysenter(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_receive_sysenter(endpoint_t src, సందేశం *m_ptr, పూర్ణాంకం *స్థితి_ptr);
పూర్ణాంకం usermapped_sendrec_sysenter(endpoint_t src_dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_sendnb_sysenter(endpoint_t dest, సందేశం *m_ptr);
పూర్ణాంకం usermapped_notify_sysenter(endpoint_t dest);
పూర్ణాంకం usermapped_do_kernel_call_sysenter(సందేశం *m_ptr);
పూర్ణాంకం usermapped_senda_sysenter(asynmsg_t *పట్టిక, size_t లెక్క);

శూన్య switch_k_దొంతర(శూన్య * esp, శూన్య (* continuation)(శూన్య));

శూన్య __switch_address_space(నిర్మాణం proc * p, నిర్మాణం proc ** __ptproc);
#నిర్వచించు switch_చిరునామా_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

శూన్య refresh_tlb(శూన్య);

/* multiboot.c */
శూన్య multiboot_init(శూన్య);

/* protect.c */
నిర్మాణం tss_s {
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

శూన్య enable_iop(నిర్మాణం proc *pp);
u32_t read_cs(శూన్య);
u32_t read_ds(శూన్య);
u32_t read_ss(శూన్య);

శూన్య కూడు_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_బైట్లు alloc_అత్యల్ప(kinfo_t *cbi, phys_బైట్లు len);
శూన్య vm_enable_paging(శూన్య);
శూన్య cut_memmap(kinfo_t *cbi, phys_బైట్లు ప్రారంభం, phys_బైట్లు చివర);
phys_బైట్లు pg_roundup(phys_బైట్లు b);
శూన్య pg_info(reg_t *, u32_t **);
శూన్య pg_clear(శూన్య);
శూన్య pg_తటస్థం(kinfo_t *);
phys_బైట్లు pg_ఎక్కించు(శూన్య);
శూన్య pg_map(phys_బైట్లు phys, vir_బైట్లు vaddr, vir_బైట్లు vaddr_చివర, kinfo_t *cbi);
పూర్ణాంకం pg_mapkernel(శూన్య);
శూన్య pg_mapproc(నిర్మాణం proc *p, నిర్మాణం boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
నిర్మాణం gate_పట్టిక_s {
 శూన్య(*gate) (శూన్య);
	గుర్తులేని అక్షరం vec_nr;
	గుర్తులేని అక్షరం privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
శూన్య idt_copy_vectors(నిర్మాణం gate_పట్టిక_s * మొదటి);
శూన్య idt_copy_vectors_pic(శూన్య);
శూన్య idt_reload(శూన్య);

EXTERN శూన్య * k_stacks_ప్రారంభం;
బాహ్య శూన్య * k_stacks;

#నిర్వచించు get_k_దొంతర_శిఖరం(cpu)	((శూన్య *)(((అక్షరం*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_దొంతర_కొలత))

శూన్య mfence(శూన్య);
#నిర్వచించు barrier()	చేయి { mfence(); } అయితే(0)


#నిర్వచితం_కాకపోతే __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(శూన్య);
#నిర్వచించు get_దొంతర_చట్రం(__X)	((reg_t)read_ebp())
#లేకపోతే
/* read %ebp directly */
#నిర్వచించు get_దొంతర_చట్రం(__X)	((reg_t)__builtin_frame_address(0))
#అయితే_ముగింపు

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
పూర్ణాంకం tss_init(గుర్తులేని cpu, శూన్య * kernel_దొంతర);

శూన్య int_gate_idt(గుర్తులేని vec_nr, vir_బైట్లు స్థానభ్రంశం, గుర్తులేని dpl_type);

శూన్య __copy_msg_from_user_end(శూన్య);
శూన్య __copy_msg_to_user_end(శూన్య);
శూన్య __user_copy_msg_pointer_failure(శూన్య);

పూర్ణాంకం platform_tbl_checksum_ok(శూన్య *ptr, గుర్తులేని పూర్ణాంకం నిడివి);
పూర్ణాంకం platform_tbl_ptr(phys_బైట్లు ప్రారంభం, phys_బైట్లు చివర, గుర్తులేని
	increment, శూన్య * buff, గుర్తులేని కొలత, phys_బైట్లు * phys_addr, పూర్ణాంకం ((*
	cmp_f)(శూన్య *)));

/* breakpoints.c */
పూర్ణాంకం breakpoint_సమితి(phys_బైట్లు linaddr, పూర్ణాంకం bp, స్థిరరాశి పూర్ణాంకం flags);
#నిర్వచించు BREAKPOINT_లెక్క		4
#నిర్వచించు BREAKPOINT_FLAG_RW_ముసుగు		(3 << 0)
#నిర్వచించు BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#నిర్వచించు BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#నిర్వచించు BREAKPOINT_FLAG_RW_RW		(2 << 0)
#నిర్వచించు BREAKPOINT_FLAG_LEN_ముసుగు	(3 << 2)
#నిర్వచించు BREAKPOINT_FLAG_LEN_1		(0 << 2)
#నిర్వచించు BREAKPOINT_FLAG_LEN_2		(1 << 2)
#నిర్వచించు BREAKPOINT_FLAG_LEN_4		(2 << 2)
#నిర్వచించు BREAKPOINT_FLAG_బహుళకం_ముసుగు	(3 << 4)
#నిర్వచించు BREAKPOINT_FLAG_బహుళకం_OFF	(0 << 4)
#నిర్వచించు BREAKPOINT_FLAG_బహుళకం_LOCAL	(1 << 4)
#నిర్వచించు BREAKPOINT_FLAG_బహుళకం_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#అన్తర్భావ "kernel/proto.h"

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు
