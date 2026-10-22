
#جيڪڏهن_وصف_نه_ٿيل _I386_PROTO_H
#وصف_ڪريو _I386_PROTO_H

#ضمن <machine/vm.h>

#وصف_ڪريو K_ڍير_سائيز	I386_صفحو_سائيز

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

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
فراغ int01(فراغ), single_قدم_exception (فراغ);
فراغ int02(فراغ), nmi (فراغ);
فراغ int03(فراغ), breakpoint_exception (فراغ);
فراغ int04(فراغ), حد_کان_مٿي (فراغ);
فراغ int05(فراغ), bounds_check (فراغ);
فراغ int06(فراغ), inval_opcode (فراغ);
فراغ int07(فراغ), copr_not_موجود (فراغ);
فراغ double_fault(فراغ);
فراغ copr_seg_overrun(فراغ);
فراغ inval_tss(فراغ);
فراغ segment_not_present(فراغ);
فراغ ڍير_exception(فراغ);
فراغ general_protection(فراغ);
فراغ صفحو_fault(فراغ);
فراغ copr_error(فراغ);
فراغ alignment_check(فراغ);
فراغ machine_check(فراغ);
فراغ simd_exception(فراغ);

فراغ restore_user_context_int(بنية proc *);
فراغ restore_user_context_sysenter(بنية proc *);
فراغ restore_user_context_syscall(بنية proc *);

/* Software interrupt handlers, in numerical order. */
فراغ trp(فراغ);
فراغ ipc_داخلا_softint_orig(فراغ);
فراغ ipc_داخلا_softint_um(فراغ);
فراغ ipc_داخلا_sysenter(فراغ);
فراغ ipc_داخلا_syscall_cpu0(فراغ);
فراغ ipc_داخلا_syscall_cpu1(فراغ);
فراغ ipc_داخلا_syscall_cpu2(فراغ);
فراغ ipc_داخلا_syscall_cpu3(فراغ);
فراغ ipc_داخلا_syscall_cpu4(فراغ);
فراغ ipc_داخلا_syscall_cpu5(فراغ);
فراغ ipc_داخلا_syscall_cpu6(فراغ);
فراغ ipc_داخلا_syscall_cpu7(فراغ);
فراغ kernel_call_داخلا_orig(فراغ);
فراغ kernel_call_داخلا_um(فراغ);
فراغ level0_call(فراغ);

/* exception.c */
بنية exception_چوکٺو {
	reg_t	ويڪٽر;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

فراغ exception(بنية exception_چوکٺو * چوکٺو);

/* klib.S */
__dead فراغ monitor(فراغ);
__dead فراغ reset(فراغ);
__dead فراغ poweroff_vmware_clihlt(فراغ);
__dead فراغ x86_triplefault(فراغ);
reg_t read_cr0(فراغ);
reg_t read_cr2(فراغ);
فراغ write_cr0(بنانشان ڊگهو قدر);
بنانشان ڊگهو read_cr4(فراغ);
فراغ write_cr4(بنانشان ڊگهو قدر);
فراغ write_cr3(بنانشان ڊگهو قدر);
بنانشان ڊگهو read_cpu_flags(فراغ);
phys_بائيٽون vir2phys(فراغ *);
فراغ phys_insb(u16_t پورٽ, phys_بائيٽون buf, size_t ڳڻپ);
فراغ phys_insw(u16_t پورٽ, phys_بائيٽون buf, size_t ڳڻپ);
فراغ phys_outsb(u16_t پورٽ, phys_بائيٽون buf, size_t ڳڻپ);
فراغ phys_outsw(u16_t پورٽ, phys_بائيٽون buf, size_t ڳڻپ);
u32_t read_cr3(فراغ);
فراغ reload_cr3(فراغ);
فراغ i386_invlpg(phys_بائيٽون linaddr);
vir_بائيٽون phys_memset(phys_بائيٽون ph, u32_t c, phys_بائيٽون بائيٽون);
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
بنانشان ننڍو fnstsw(فراغ);
فراغ fnstcw(بنانشان ننڍو* cw);
فراغ x86_lgdt(فراغ *);
فراغ x86_lldt(u32_t);
فراغ x86_ltr(u32_t);
فراغ x86_lidt(فراغ *);
فراغ x86_ڀريو_kerncs(فراغ);
فراغ x86_ڀريو_ds(u32_t);
فراغ x86_ڀريو_ss(u32_t);
فراغ x86_ڀريو_es(u32_t);
فراغ x86_ڀريو_fs(u32_t);
فراغ x86_ڀريو_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
صحيح usermapped_send_softint(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_receive_softint(endpoint_t src, پيغام *m_ptr, صحيح *حالت_ptr);
صحيح usermapped_sendrec_softint(endpoint_t src_dest, پيغام *m_ptr);
صحيح usermapped_sendnb_softint(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_notify_softint(endpoint_t dest);
صحيح usermapped_do_kernel_call_softint(پيغام *m_ptr);
صحيح usermapped_senda_softint(asynmsg_t *جدول, size_t ڳڻپ);

صحيح usermapped_send_syscall(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_receive_syscall(endpoint_t src, پيغام *m_ptr, صحيح *حالت_ptr);
صحيح usermapped_sendrec_syscall(endpoint_t src_dest, پيغام *m_ptr);
صحيح usermapped_sendnb_syscall(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_notify_syscall(endpoint_t dest);
صحيح usermapped_do_kernel_call_syscall(پيغام *m_ptr);
صحيح usermapped_senda_syscall(asynmsg_t *جدول, size_t ڳڻپ);

صحيح usermapped_send_sysenter(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_receive_sysenter(endpoint_t src, پيغام *m_ptr, صحيح *حالت_ptr);
صحيح usermapped_sendrec_sysenter(endpoint_t src_dest, پيغام *m_ptr);
صحيح usermapped_sendnb_sysenter(endpoint_t dest, پيغام *m_ptr);
صحيح usermapped_notify_sysenter(endpoint_t dest);
صحيح usermapped_do_kernel_call_sysenter(پيغام *m_ptr);
صحيح usermapped_senda_sysenter(asynmsg_t *جدول, size_t ڳڻپ);

فراغ switch_k_ڍير(فراغ * esp, فراغ (* continuation)(فراغ));

فراغ __switch_address_space(بنية proc * p, بنية proc ** __ptproc);
#وصف_ڪريو switch_پتو_space(proc)	\
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

فراغ جوڙيو_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بائيٽون alloc_سڀ_کان_هيٺ(kinfo_t *cbi, phys_بائيٽون len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_بائيٽون شروعات, phys_بائيٽون پڇاڙي);
phys_بائيٽون pg_roundup(phys_بائيٽون b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_سڃاڻپ_عنصر(kinfo_t *);
phys_بائيٽون pg_ڀريو(فراغ);
فراغ pg_map(phys_بائيٽون phys, vir_بائيٽون vaddr, vir_بائيٽون vaddr_پڇاڙي, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
بنية gate_جدول_s {
 فراغ(*gate) (فراغ);
	بنانشان حرف vec_nr;
	بنانشان حرف privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
فراغ idt_copy_vectors(بنية gate_جدول_s * پهريون);
فراغ idt_copy_vectors_pic(فراغ);
فراغ idt_reload(فراغ);

EXTERN فراغ * k_stacks_شروعات;
خارجي فراغ * k_stacks;

#وصف_ڪريو get_k_ڍير_چوٽي(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ڍير_سائيز))

فراغ mfence(فراغ);
#وصف_ڪريو barrier()	افعل { mfence(); } طالما(0)


#جيڪڏهن_وصف_نه_ٿيل __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(فراغ);
#وصف_ڪريو get_ڍير_چوکٺو(__X)	((reg_t)read_ebp())
#وإلا
/* read %ebp directly */
#وصف_ڪريو get_ڍير_چوکٺو(__X)	((reg_t)__builtin_frame_address(0))
#جيڪڏهن_پورو

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
صحيح tss_init(بنانشان cpu, فراغ * kernel_ڍير);

فراغ int_gate_idt(بنانشان vec_nr, vir_بائيٽون وٿي, بنانشان dpl_type);

فراغ __copy_msg_from_user_end(فراغ);
فراغ __copy_msg_to_user_end(فراغ);
فراغ __user_copy_msg_pointer_failure(فراغ);

صحيح platform_tbl_checksum_ok(فراغ *ptr, بنانشان صحيح ڊيگهائي);
صحيح platform_tbl_ptr(phys_بائيٽون شروعات, phys_بائيٽون پڇاڙي, بنانشان
	increment, فراغ * buff, بنانشان سائيز, phys_بائيٽون * phys_addr, صحيح ((*
	cmp_f)(فراغ *)));

/* breakpoints.c */
صحيح breakpoint_سيٽ(phys_بائيٽون linaddr, صحيح bp, ثابت صحيح flags);
#وصف_ڪريو BREAKPOINT_ڳڻپ		4
#وصف_ڪريو BREAKPOINT_FLAG_RW_نقاب		(3 << 0)
#وصف_ڪريو BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#وصف_ڪريو BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#وصف_ڪريو BREAKPOINT_FLAG_RW_RW		(2 << 0)
#وصف_ڪريو BREAKPOINT_FLAG_LEN_نقاب	(3 << 2)
#وصف_ڪريو BREAKPOINT_FLAG_LEN_1		(0 << 2)
#وصف_ڪريو BREAKPOINT_FLAG_LEN_2		(1 << 2)
#وصف_ڪريو BREAKPOINT_FLAG_LEN_4		(2 << 2)
#وصف_ڪريو BREAKPOINT_FLAG_گهڻائي_نقاب	(3 << 4)
#وصف_ڪريو BREAKPOINT_FLAG_گهڻائي_OFF	(0 << 4)
#وصف_ڪريو BREAKPOINT_FLAG_گهڻائي_LOCAL	(1 << 4)
#وصف_ڪريو BREAKPOINT_FLAG_گهڻائي_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#ضمن "kernel/proto.h"

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو
