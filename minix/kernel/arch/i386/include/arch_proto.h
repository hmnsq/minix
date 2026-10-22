
#अगर_अघोषित _I386_PROTO_H
#घोषणा _I386_PROTO_H

#समावेश <machine/vm.h>

#घोषणा K_ढेर_आकार	I386_PAGE_आकार

#अगर_अघोषित __ASSEMBLY__

/* Hardware interrupt handlers. */
व्योम hwint00(व्योम);
व्योम hwint01(व्योम);
व्योम hwint02(व्योम);
व्योम hwint03(व्योम);
व्योम hwint04(व्योम);
व्योम hwint05(व्योम);
व्योम hwint06(व्योम);
व्योम hwint07(व्योम);
व्योम hwint08(व्योम);
व्योम hwint09(व्योम);
व्योम hwint10(व्योम);
व्योम hwint11(व्योम);
व्योम hwint12(व्योम);
व्योम hwint13(व्योम);
व्योम hwint14(व्योम);
व्योम hwint15(व्योम);

/* Exception handlers (real or protected mode), in numerical order. */
व्योम int00(व्योम), divide_error (व्योम);
व्योम int01(व्योम), single_step_exception (व्योम);
व्योम int02(व्योम), nmi (व्योम);
व्योम int03(व्योम), breakpoint_exception (व्योम);
व्योम int04(व्योम), overflow (व्योम);
व्योम int05(व्योम), bounds_check (व्योम);
व्योम int06(व्योम), inval_opcode (व्योम);
व्योम int07(व्योम), copr_not_available (व्योम);
व्योम double_fault(व्योम);
व्योम copr_seg_overrun(व्योम);
व्योम inval_tss(व्योम);
व्योम segment_not_present(व्योम);
व्योम ढेर_exception(व्योम);
व्योम general_protection(व्योम);
व्योम page_fault(व्योम);
व्योम copr_error(व्योम);
व्योम alignment_check(व्योम);
व्योम machine_check(व्योम);
व्योम simd_exception(व्योम);

व्योम restore_user_context_int(काष्ठा proc *);
व्योम restore_user_context_sysenter(काष्ठा proc *);
व्योम restore_user_context_syscall(काष्ठा proc *);

/* Software interrupt handlers, in numerical order. */
व्योम trp(व्योम);
व्योम ipc_entry_softint_orig(व्योम);
व्योम ipc_entry_softint_um(व्योम);
व्योम ipc_entry_sysenter(व्योम);
व्योम ipc_entry_syscall_cpu0(व्योम);
व्योम ipc_entry_syscall_cpu1(व्योम);
व्योम ipc_entry_syscall_cpu2(व्योम);
व्योम ipc_entry_syscall_cpu3(व्योम);
व्योम ipc_entry_syscall_cpu4(व्योम);
व्योम ipc_entry_syscall_cpu5(व्योम);
व्योम ipc_entry_syscall_cpu6(व्योम);
व्योम ipc_entry_syscall_cpu7(व्योम);
व्योम kernel_call_entry_orig(व्योम);
व्योम kernel_call_entry_um(व्योम);
व्योम level0_call(व्योम);

/* exception.c */
काष्ठा exception_frame {
	reg_t	vector;		/* which interrupt vector was triggered */
	reg_t	errcode;	/* zero if no exception does not push err code */
	reg_t	eip;
	reg_t	cs;
	reg_t	eflags;
	reg_t	esp;		/* undefined if trap is nested */
	reg_t	ss;		/* undefined if trap is nested */
};

व्योम exception(काष्ठा exception_frame * frame);

/* klib.S */
__dead व्योम monitor(व्योम);
__dead व्योम reset(व्योम);
__dead व्योम poweroff_vmware_clihlt(व्योम);
__dead व्योम x86_triplefault(व्योम);
reg_t read_cr0(व्योम);
reg_t read_cr2(व्योम);
व्योम write_cr0(अचिन्हित दीर्घ मूल्य);
अचिन्हित दीर्घ read_cr4(व्योम);
व्योम write_cr4(अचिन्हित दीर्घ मूल्य);
व्योम write_cr3(अचिन्हित दीर्घ मूल्य);
अचिन्हित दीर्घ read_cpu_flags(व्योम);
phys_bytes vir2phys(व्योम *);
व्योम phys_insb(u16_t port, phys_bytes buf, माप_प्रकार गिनती);
व्योम phys_insw(u16_t port, phys_bytes buf, माप_प्रकार गिनती);
व्योम phys_outsb(u16_t port, phys_bytes buf, माप_प्रकार गिनती);
व्योम phys_outsw(u16_t port, phys_bytes buf, माप_प्रकार गिनती);
u32_t read_cr3(व्योम);
व्योम reload_cr3(व्योम);
व्योम i386_invlpg(phys_bytes linaddr);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);
व्योम reload_ds(व्योम);
व्योम ia32_msr_read(u32_t reg, u32_t * hi, u32_t * lo);
व्योम ia32_msr_write(u32_t reg, u32_t hi, u32_t lo);
व्योम fninit(व्योम);
व्योम clts(व्योम);
व्योम fxsave(व्योम *);
व्योम fnsave(व्योम *);
पूर्णांक fxrstor(व्योम *);
पूर्णांक __fxrstor_end(व्योम *);
पूर्णांक frstor(व्योम *);
पूर्णांक __frstor_end(व्योम *);
पूर्णांक __frstor_failure(व्योम *);
अचिन्हित लघु fnstsw(व्योम);
व्योम fnstcw(अचिन्हित लघु* cw);
व्योम x86_lgdt(व्योम *);
व्योम x86_lldt(u32_t);
व्योम x86_ltr(u32_t);
व्योम x86_lidt(व्योम *);
व्योम x86_load_kerncs(व्योम);
व्योम x86_load_ds(u32_t);
व्योम x86_load_ss(u32_t);
व्योम x86_load_es(u32_t);
व्योम x86_load_fs(u32_t);
व्योम x86_load_gs(u32_t);

/* ipc functions in usermapped_ipc.S */
पूर्णांक usermapped_send_softint(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_receive_softint(endpoint_t src, message *m_ptr, पूर्णांक *status_ptr);
पूर्णांक usermapped_sendrec_softint(endpoint_t src_dest, message *m_ptr);
पूर्णांक usermapped_sendnb_softint(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_notify_softint(endpoint_t dest);
पूर्णांक usermapped_do_kernel_call_softint(message *m_ptr);
पूर्णांक usermapped_senda_softint(asynmsg_t *table, माप_प्रकार गिनती);

पूर्णांक usermapped_send_syscall(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_receive_syscall(endpoint_t src, message *m_ptr, पूर्णांक *status_ptr);
पूर्णांक usermapped_sendrec_syscall(endpoint_t src_dest, message *m_ptr);
पूर्णांक usermapped_sendnb_syscall(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_notify_syscall(endpoint_t dest);
पूर्णांक usermapped_do_kernel_call_syscall(message *m_ptr);
पूर्णांक usermapped_senda_syscall(asynmsg_t *table, माप_प्रकार गिनती);

पूर्णांक usermapped_send_sysenter(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_receive_sysenter(endpoint_t src, message *m_ptr, पूर्णांक *status_ptr);
पूर्णांक usermapped_sendrec_sysenter(endpoint_t src_dest, message *m_ptr);
पूर्णांक usermapped_sendnb_sysenter(endpoint_t dest, message *m_ptr);
पूर्णांक usermapped_notify_sysenter(endpoint_t dest);
पूर्णांक usermapped_do_kernel_call_sysenter(message *m_ptr);
पूर्णांक usermapped_senda_sysenter(asynmsg_t *table, माप_प्रकार गिनती);

व्योम switch_k_ढेर(व्योम * esp, व्योम (* continuation)(व्योम));

व्योम __switch_address_space(काष्ठा proc * p, काष्ठा proc ** __ptproc);
#घोषणा switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

व्योम refresh_tlb(व्योम);

/* multiboot.c */
व्योम multiboot_init(व्योम);

/* protect.c */
काष्ठा tss_s {
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

व्योम enable_iop(काष्ठा proc *pp);
u32_t read_cs(व्योम);
u32_t read_ds(व्योम);
u32_t read_ss(व्योम);

व्योम add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
व्योम vm_enable_paging(व्योम);
व्योम cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
व्योम pg_info(reg_t *, u32_t **);
व्योम pg_clear(व्योम);
व्योम pg_identity(kinfo_t *);
phys_bytes pg_load(व्योम);
व्योम pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
पूर्णांक pg_mapkernel(व्योम);
व्योम pg_mapproc(काष्ठा proc *p, काष्ठा boot_image *ip, kinfo_t *cbi);

/* prototype of an interrupt vector table entry */
काष्ठा gate_table_s {
 व्योम(*gate) (व्योम);
	अचिन्हित अक्षर vec_nr;
	अचिन्हित अक्षर privilege;
};

/* copies an array of vectors to the IDT. The last vector must be zero filled */
व्योम idt_copy_vectors(काष्ठा gate_table_s * first);
व्योम idt_copy_vectors_pic(व्योम);
व्योम idt_reload(व्योम);

EXTERN व्योम * k_stacks_start;
बाह्य व्योम * k_stacks;

#घोषणा get_k_ढेर_शीर्ष(cpu)	((व्योम *)(((अक्षर*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ढेर_आकार))

व्योम mfence(व्योम);
#घोषणा barrier()	करो { mfence(); } जबतक(0)


#अगर_अघोषित __GNUC__
/* call a function to read the stack fram pointer (%ebp) */
reg_t read_ebp(व्योम);
#घोषणा get_ढेर_frame(__X)	((reg_t)read_ebp())
#अन्यथा
/* read %ebp directly */
#घोषणा get_ढेर_frame(__X)	((reg_t)__builtin_frame_address(0))
#पूर्ण_अगर

/*
 * sets up TSS for a cpu and assigns kernel stack and cpu id
 */
पूर्णांक tss_init(अचिन्हित cpu, व्योम * kernel_ढेर);

व्योम int_gate_idt(अचिन्हित vec_nr, vir_bytes offset, अचिन्हित dpl_type);

व्योम __copy_msg_from_user_end(व्योम);
व्योम __copy_msg_to_user_end(व्योम);
व्योम __user_copy_msg_pointer_failure(व्योम);

पूर्णांक platform_tbl_checksum_ok(व्योम *ptr, अचिन्हित पूर्णांक लंबाई);
पूर्णांक platform_tbl_ptr(phys_bytes start, phys_bytes end, अचिन्हित
	increment, व्योम * buff, अचिन्हित आकार, phys_bytes * phys_addr, पूर्णांक ((*
	cmp_f)(व्योम *)));

/* breakpoints.c */
पूर्णांक breakpoint_set(phys_bytes linaddr, पूर्णांक bp, स्थिर पूर्णांक flags);
#घोषणा BREAKPOINT_गिनती		4
#घोषणा BREAKPOINT_FLAG_RW_MASK		(3 << 0)
#घोषणा BREAKPOINT_FLAG_RW_EXEC		(0 << 0)
#घोषणा BREAKPOINT_FLAG_RW_WRITE	(1 << 0)
#घोषणा BREAKPOINT_FLAG_RW_RW		(2 << 0)
#घोषणा BREAKPOINT_FLAG_LEN_MASK	(3 << 2)
#घोषणा BREAKPOINT_FLAG_LEN_1		(0 << 2)
#घोषणा BREAKPOINT_FLAG_LEN_2		(1 << 2)
#घोषणा BREAKPOINT_FLAG_LEN_4		(2 << 2)
#घोषणा BREAKPOINT_FLAG_MODE_MASK	(3 << 4)
#घोषणा BREAKPOINT_FLAG_MODE_OFF	(0 << 4)
#घोषणा BREAKPOINT_FLAG_MODE_LOCAL	(1 << 4)
#घोषणा BREAKPOINT_FLAG_MODE_GLOBAL	(2 << 4)

/* functions defined in architecture-independent kernel source. */
#समावेश "kernel/proto.h"

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर
