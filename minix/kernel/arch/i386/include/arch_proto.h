
#اگر_تعریف_نشده _I386_PROTO_H
#تعریف_کن _I386_PROTO_H

#ضمن <machine/vm.h>

#تعریف_کن K_پشته_اندازه	I386_صفحه_اندازه

#اگر_تعریف_نشده __ASSEMBLY__

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
فراغ int01(فراغ), single_گام_exception (فراغ);
فراغ int02(فراغ), nmi (فراغ);
فراغ int03(فراغ), breakpoint_exception (فراغ);
فراغ int04(فراغ), سرریز (فراغ);
فراغ int05(فراغ), bounds_check (فراغ);
فراغ int06(فراغ), inval_opcode (فراغ);
فراغ int07(فراغ), copr_not_موجود (فراغ);
فراغ double_fault(فراغ);
فراغ copr_seg_overrun(فراغ);
فراغ inval_tss(فراغ);
فراغ segment_not_present(فراغ);
فراغ پشته_exception(فراغ);
فراغ general_protection(فراغ);
فراغ صفحه_fault(فراغ);
فراغ copr_error(فراغ);
فراغ alignment_check(فراغ);
فراغ machine_check(فراغ);
فراغ simd_exception(فراغ);

فراغ restore_user_context_int(بنية proc *);
فراغ restore_user_context_sysenter(بنية proc *);
فراغ restore_user_context_syscall(بنية proc *);

/* Software interrupt handlers, in numerical order. */
فراغ trp(فراغ);
فراغ ipc_مدخل_softint_orig(فراغ);
فراغ ipc_مدخل_softint_um(فراغ);
فراغ ipc_مدخل_sysenter(فراغ);
فراغ ipc_مدخل_syscall_cpu0(فراغ);
فراغ ipc_مدخل_syscall_cpu1(فراغ);
فراغ ipc_مدخل_syscall_cpu2(فراغ);
فراغ ipc_مدخل_syscall_cpu3(فراغ);
فراغ ipc_مدخل_syscall_cpu4(فراغ);
فراغ ipc_مدخل_syscall_cpu5(فراغ);
فراغ ipc_مدخل_syscall_cpu6(فراغ);
فراغ ipc_مدخل_syscall_cpu7(فراغ);
فراغ kernel_call_مدخل_orig(فراغ);
فراغ kernel_call_مدخل_um(فراغ);
فراغ level0_call(فراغ);

/* exception.c */
بنية exception_قاب {
	reg_t	وکتور;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

فراغ exception(بنية exception_قاب * قاب);

/* klib.S */
__dead فراغ monitor(فراغ);
__dead فراغ reset(فراغ);
__dead فراغ poweroff_vmware_clihlt(فراغ);
__dead فراغ x86_triplefault(فراغ);
reg_t read_cr0(فراغ);
reg_t read_cr2(فراغ);
فراغ write_cr0(بیعلامت دراز مقدار);
بیعلامت دراز read_cr4(فراغ);
فراغ write_cr4(بیعلامت دراز مقدار);
فراغ write_cr3(بیعلامت دراز مقدار);
بیعلامت دراز read_cpu_flags(فراغ);
phys_بایتها vir2phys(فراغ *);
فراغ phys_insb(u16_t پورت, phys_بایتها buf, size_t شمار);
فراغ phys_insw(u16_t پورت, phys_بایتها buf, size_t شمار);
فراغ phys_outsb(u16_t پورت, phys_بایتها buf, size_t شمار);
فراغ phys_outsw(u16_t پورت, phys_بایتها buf, size_t شمار);
u32_t read_cr3(فراغ);
فراغ reload_cr3(فراغ);
فراغ i386_invlpg(phys_بایتها linaddr);
vir_بایتها phys_memset(phys_بایتها ph, u32_t c, phys_بایتها بایتها);
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
بیعلامت کوتاه fnstsw(فراغ);
فراغ fnstcw(بیعلامت کوتاه* cw);
فراغ x86_lgdt(فراغ *);
فراغ x86_lldt(u32_t);
فراغ x86_ltr(u32_t);
فراغ x86_lidt(فراغ *);
فراغ x86_بارگیری_kerncs(فراغ);
فراغ x86_بارگیری_ds(u32_t);
فراغ x86_بارگیری_ss(u32_t);
فراغ x86_بارگیری_es(u32_t);
فراغ x86_بارگیری_fs(u32_t);
فراغ x86_بارگیری_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
صحيح usermapped_send_softint(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_receive_softint(endpoint_t src, پیام *m_ptr, صحيح *وضعیت_ptr);
صحيح usermapped_sendrec_softint(endpoint_t src_dest, پیام *m_ptr);
صحيح usermapped_sendnb_softint(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_notify_softint(endpoint_t dest);
صحيح usermapped_do_kernel_call_softint(پیام *m_ptr);
صحيح usermapped_senda_softint(asynmsg_t *جدول, size_t شمار);

صحيح usermapped_send_syscall(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_receive_syscall(endpoint_t src, پیام *m_ptr, صحيح *وضعیت_ptr);
صحيح usermapped_sendrec_syscall(endpoint_t src_dest, پیام *m_ptr);
صحيح usermapped_sendnb_syscall(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_notify_syscall(endpoint_t dest);
صحيح usermapped_do_kernel_call_syscall(پیام *m_ptr);
صحيح usermapped_senda_syscall(asynmsg_t *جدول, size_t شمار);

صحيح usermapped_send_sysenter(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_receive_sysenter(endpoint_t src, پیام *m_ptr, صحيح *وضعیت_ptr);
صحيح usermapped_sendrec_sysenter(endpoint_t src_dest, پیام *m_ptr);
صحيح usermapped_sendnb_sysenter(endpoint_t dest, پیام *m_ptr);
صحيح usermapped_notify_sysenter(endpoint_t dest);
صحيح usermapped_do_kernel_call_sysenter(پیام *m_ptr);
صحيح usermapped_senda_sysenter(asynmsg_t *جدول, size_t شمار);

فراغ switch_k_پشته(فراغ * esp, فراغ (* continuation)(فراغ));

فراغ __switch_address_space(بنية proc * p, بنية proc ** __ptproc);
#تعریف_کن switch_آدرس_space(proc)	\
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

فراغ جمع_کن_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بایتها alloc_پایینترین(kinfo_t *cbi, phys_بایتها len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_بایتها شروع, phys_بایتها انتها);
phys_بایتها pg_roundup(phys_بایتها b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_عنصر_خنثی(kinfo_t *);
phys_بایتها pg_بارگیری(فراغ);
فراغ pg_map(phys_بایتها phys, vir_بایتها vaddr, vir_بایتها vaddr_انتها, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
بنية gate_جدول_s {
 فراغ(*gate) (فراغ);
	بیعلامت حرف vec_nr;
	بیعلامت حرف privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
فراغ idt_copy_vectors(بنية gate_جدول_s * اولی);
فراغ idt_copy_vectors_pic(فراغ);
فراغ idt_reload(فراغ);

EXTERN فراغ * k_stacks_شروع;
خارجي فراغ * k_stacks;

#تعریف_کن get_k_پشته_بالاترین(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_پشته_اندازه))

فراغ mfence(فراغ);
#تعریف_کن barrier()	افعل { mfence(); } طالما(0)


#اگر_تعریف_نشده __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(فراغ);
#تعریف_کن get_پشته_قاب(__X)	((reg_t)read_ebp())
#وإلا
/* read %ebp directly */
#تعریف_کن get_پشته_قاب(__X)	((reg_t)__builtin_frame_address(0))
#پایان_اگر

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
صحيح tss_init(بیعلامت cpu, فراغ * kernel_پشته);

فراغ int_gate_idt(بیعلامت vec_nr, vir_بایتها جابه_جایی, بیعلامت dpl_type);

فراغ __copy_msg_from_user_end(فراغ);
فراغ __copy_msg_to_user_end(فراغ);
فراغ __user_copy_msg_pointer_failure(فراغ);

صحيح platform_tbl_checksum_ok(فراغ *ptr, بیعلامت صحيح درازی);
صحيح platform_tbl_ptr(phys_بایتها شروع, phys_بایتها انتها, بیعلامت
	increment, فراغ * buff, بیعلامت اندازه, phys_بایتها * phys_addr, صحيح ((*
	cmp_f)(فراغ *)));

/* breakpoints.c */
صحيح breakpoint_مجموعه(phys_بایتها linaddr, صحيح bp, ثابت صحيح flags);
#تعریف_کن BREAKPOINT_شمار		4
#تعریف_کن BREAKPOINT_FLAG_RW_ماسک		(3 << 0)
#تعریف_کن BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#تعریف_کن BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#تعریف_کن BREAKPOINT_FLAG_RW_RW		(2 << 0)
#تعریف_کن BREAKPOINT_FLAG_LEN_ماسک	(3 << 2)
#تعریف_کن BREAKPOINT_FLAG_LEN_1		(0 << 2)
#تعریف_کن BREAKPOINT_FLAG_LEN_2		(1 << 2)
#تعریف_کن BREAKPOINT_FLAG_LEN_4		(2 << 2)
#تعریف_کن BREAKPOINT_FLAG_نما_ماسک	(3 << 4)
#تعریف_کن BREAKPOINT_FLAG_نما_OFF	(0 << 4)
#تعریف_کن BREAKPOINT_FLAG_نما_LOCAL	(1 << 4)
#تعریف_کن BREAKPOINT_FLAG_نما_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#ضمن "kernel/proto.h"

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر
