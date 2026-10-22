
#اگر_غیر_تعریف_شدہ _I386_PROTO_H
#تعریف_کرو _I386_PROTO_H

#شامل <machine/vm.h>

#تعریف_کرو K_ڈھیر_سائز	I386_صفحہ_سائز

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

/* Hardware interrupt handlers. */
خالی hwint00(خالی);
خالی hwint01(خالی);
خالی hwint02(خالی);
خالی hwint03(خالی);
خالی hwint04(خالی);
خالی hwint05(خالی);
خالی hwint06(خالی);
خالی hwint07(خالی);
خالی hwint08(خالی);
خالی hwint09(خالی);
خالی hwint10(خالی);
خالی hwint11(خالی);
خالی hwint12(خالی);
خالی hwint13(خالی);
خالی hwint14(خالی);
خالی hwint15(خالی);

/* Exception handlers (real or protected mode), in numerical order. */
خالی int00(خالی), divide_error (خالی);
خالی int01(خالی), single_قدم_exception (خالی);
خالی int02(خالی), nmi (خالی);
خالی int03(خالی), breakpoint_exception (خالی);
خالی int04(خالی), حد_سے_تجاوز (خالی);
خالی int05(خالی), bounds_check (خالی);
خالی int06(خالی), inval_opcode (خالی);
خالی int07(خالی), copr_not_دستیاب (خالی);
خالی double_fault(خالی);
خالی copr_seg_overrun(خالی);
خالی inval_tss(خالی);
خالی segment_not_present(خالی);
خالی ڈھیر_exception(خالی);
خالی general_protection(خالی);
خالی صفحہ_fault(خالی);
خالی copr_error(خالی);
خالی alignment_check(خالی);
خالی machine_check(خالی);
خالی simd_exception(خالی);

خالی restore_user_context_int(ساخت proc *);
خالی restore_user_context_sysenter(ساخت proc *);
خالی restore_user_context_syscall(ساخت proc *);

/* Software interrupt handlers, in numerical order. */
خالی trp(خالی);
خالی ipc_اندراج_softint_orig(خالی);
خالی ipc_اندراج_softint_um(خالی);
خالی ipc_اندراج_sysenter(خالی);
خالی ipc_اندراج_syscall_cpu0(خالی);
خالی ipc_اندراج_syscall_cpu1(خالی);
خالی ipc_اندراج_syscall_cpu2(خالی);
خالی ipc_اندراج_syscall_cpu3(خالی);
خالی ipc_اندراج_syscall_cpu4(خالی);
خالی ipc_اندراج_syscall_cpu5(خالی);
خالی ipc_اندراج_syscall_cpu6(خالی);
خالی ipc_اندراج_syscall_cpu7(خالی);
خالی kernel_call_اندراج_orig(خالی);
خالی kernel_call_اندراج_um(خالی);
خالی level0_call(خالی);

/* exception.c */
ساخت exception_چوکھٹا {
	reg_t	سمتیہ;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

خالی exception(ساخت exception_چوکھٹا * چوکھٹا);

/* klib.S */
__dead خالی monitor(خالی);
__dead خالی reset(خالی);
__dead خالی poweroff_vmware_clihlt(خالی);
__dead خالی x86_triplefault(خالی);
reg_t read_cr0(خالی);
reg_t read_cr2(خالی);
خالی write_cr0(غیرعلامتی طویل قدر);
غیرعلامتی طویل read_cr4(خالی);
خالی write_cr4(غیرعلامتی طویل قدر);
خالی write_cr3(غیرعلامتی طویل قدر);
غیرعلامتی طویل read_cpu_flags(خالی);
phys_بائٹیں vir2phys(خالی *);
خالی phys_insb(u16_t پورٹ, phys_بائٹیں buf, size_t گنتی);
خالی phys_insw(u16_t پورٹ, phys_بائٹیں buf, size_t گنتی);
خالی phys_outsb(u16_t پورٹ, phys_بائٹیں buf, size_t گنتی);
خالی phys_outsw(u16_t پورٹ, phys_بائٹیں buf, size_t گنتی);
u32_t read_cr3(خالی);
خالی reload_cr3(خالی);
خالی i386_invlpg(phys_بائٹیں linaddr);
vir_بائٹیں phys_memset(phys_بائٹیں ph, u32_t c, phys_بائٹیں بائٹیں);
خالی reload_ds(خالی);
خالی ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
خالی ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
خالی fninit(خالی);
خالی clts(خالی);
خالی fxsave(خالی *);
خالی fnsave(خالی *);
صحیح fxrstor(خالی *);
صحیح __fxrstor_end(خالی *);
صحیح frstor(خالی *);
صحیح __frstor_end(خالی *);
صحیح __frstor_failure(خالی *);
غیرعلامتی مختصر fnstsw(خالی);
خالی fnstcw(غیرعلامتی مختصر* cw);
خالی x86_lgdt(خالی *);
خالی x86_lldt(u32_t);
خالی x86_ltr(u32_t);
خالی x86_lidt(خالی *);
خالی x86_بھرو_kerncs(خالی);
خالی x86_بھرو_ds(u32_t);
خالی x86_بھرو_ss(u32_t);
خالی x86_بھرو_es(u32_t);
خالی x86_بھرو_fs(u32_t);
خالی x86_بھرو_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
صحیح usermapped_send_softint(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_receive_softint(endpoint_t src, پیغام *m_ptr, صحیح *حالت_ptr);
صحیح usermapped_sendrec_softint(endpoint_t src_dest, پیغام *m_ptr);
صحیح usermapped_sendnb_softint(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_notify_softint(endpoint_t dest);
صحیح usermapped_do_kernel_call_softint(پیغام *m_ptr);
صحیح usermapped_senda_softint(asynmsg_t *جدول, size_t گنتی);

صحیح usermapped_send_syscall(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_receive_syscall(endpoint_t src, پیغام *m_ptr, صحیح *حالت_ptr);
صحیح usermapped_sendrec_syscall(endpoint_t src_dest, پیغام *m_ptr);
صحیح usermapped_sendnb_syscall(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_notify_syscall(endpoint_t dest);
صحیح usermapped_do_kernel_call_syscall(پیغام *m_ptr);
صحیح usermapped_senda_syscall(asynmsg_t *جدول, size_t گنتی);

صحیح usermapped_send_sysenter(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_receive_sysenter(endpoint_t src, پیغام *m_ptr, صحیح *حالت_ptr);
صحیح usermapped_sendrec_sysenter(endpoint_t src_dest, پیغام *m_ptr);
صحیح usermapped_sendnb_sysenter(endpoint_t dest, پیغام *m_ptr);
صحیح usermapped_notify_sysenter(endpoint_t dest);
صحیح usermapped_do_kernel_call_sysenter(پیغام *m_ptr);
صحیح usermapped_senda_sysenter(asynmsg_t *جدول, size_t گنتی);

خالی switch_k_ڈھیر(خالی * esp, خالی (* continuation)(خالی));

خالی __switch_address_space(ساخت proc * p, ساخت proc ** __ptproc);
#تعریف_کرو switch_پتہ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

خالی refresh_tlb(خالی);

/* multiboot.c */
خالی multiboot_init(خالی);

/* protect.c */
ساخت tss_s {
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

خالی enable_iop(ساخت proc *pp);
u32_t read_cs(خالی);
u32_t read_ds(خالی);
u32_t read_ss(خالی);

خالی جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بائٹیں alloc_سب_سے_نیچا(kinfo_t *cbi, phys_بائٹیں len);
خالی vm_enable_paging(خالی);
خالی cut_memmap(kinfo_t *cbi, phys_بائٹیں آغاز, phys_بائٹیں انتہا);
phys_بائٹیں pg_roundup(phys_بائٹیں b);
خالی pg_info(reg_t *, u32_t **);
خالی pg_clear(خالی);
خالی pg_شناختی_عنصر(kinfo_t *);
phys_بائٹیں pg_بھرو(خالی);
خالی pg_map(phys_بائٹیں phys, vir_بائٹیں vaddr, vir_بائٹیں vaddr_انتہا, kinfo_t *cbi);
صحیح pg_mapkernel(خالی);
خالی pg_mapproc(ساخت proc *p, ساخت boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
ساخت gate_جدول_s {
 خالی(*gate) (خالی);
	غیرعلامتی حرف vec_nr;
	غیرعلامتی حرف privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
خالی idt_copy_vectors(ساخت gate_جدول_s * پہلا);
خالی idt_copy_vectors_pic(خالی);
خالی idt_reload(خالی);

EXTERN خالی * k_stacks_آغاز;
خارجی خالی * k_stacks;

#تعریف_کرو get_k_ڈھیر_چوٹی(cpu)	((خالی *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ڈھیر_سائز))

خالی mfence(خالی);
#تعریف_کرو barrier()	کرو { mfence(); } جب(0)


#اگر_غیر_تعریف_شدہ __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(خالی);
#تعریف_کرو get_ڈھیر_چوکھٹا(__X)	((reg_t)read_ebp())
#ورنہ
/* read %ebp directly */
#تعریف_کرو get_ڈھیر_چوکھٹا(__X)	((reg_t)__builtin_frame_address(0))
#اگر_ختم

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
صحیح tss_init(غیرعلامتی cpu, خالی * kernel_ڈھیر);

خالی int_gate_idt(غیرعلامتی vec_nr, vir_بائٹیں انحراف, غیرعلامتی dpl_type);

خالی __copy_msg_from_user_end(خالی);
خالی __copy_msg_to_user_end(خالی);
خالی __user_copy_msg_pointer_failure(خالی);

صحیح platform_tbl_checksum_ok(خالی *ptr, غیرعلامتی صحیح لمبائی);
صحیح platform_tbl_ptr(phys_بائٹیں آغاز, phys_بائٹیں انتہا, غیرعلامتی
	increment, خالی * buff, غیرعلامتی سائز, phys_بائٹیں * phys_addr, صحیح ((*
	cmp_f)(خالی *)));

/* breakpoints.c */
صحیح breakpoint_سیٹ(phys_بائٹیں linaddr, صحیح bp, ثابت صحیح flags);
#تعریف_کرو BREAKPOINT_گنتی		4
#تعریف_کرو BREAKPOINT_FLAG_RW_نقاب		(3 << 0)
#تعریف_کرو BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#تعریف_کرو BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#تعریف_کرو BREAKPOINT_FLAG_RW_RW		(2 << 0)
#تعریف_کرو BREAKPOINT_FLAG_LEN_نقاب	(3 << 2)
#تعریف_کرو BREAKPOINT_FLAG_LEN_1		(0 << 2)
#تعریف_کرو BREAKPOINT_FLAG_LEN_2		(1 << 2)
#تعریف_کرو BREAKPOINT_FLAG_LEN_4		(2 << 2)
#تعریف_کرو BREAKPOINT_FLAG_منوال_نقاب	(3 << 4)
#تعریف_کرو BREAKPOINT_FLAG_منوال_OFF	(0 << 4)
#تعریف_کرو BREAKPOINT_FLAG_منوال_LOCAL	(1 << 4)
#تعریف_کرو BREAKPOINT_FLAG_منوال_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#شامل "kernel/proto.h"

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم
