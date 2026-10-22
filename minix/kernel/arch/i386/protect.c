/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব <sys/exec.h>
#অন্তর্ভাব <libexec.h>

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
অক্ষর *video_mem = (অক্ষর *) MULTIBOOT_VIDEO_বাফার;

/* Storage for gdt, idt and tss. */
কাঠামো segdesc_s gdt[GDT_আকার] __aligned(DESC_আকার);
কাঠামো gatedesc_s idt[IDT_আকার] __aligned(DESC_আকার);
কাঠামো tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

পূর্ণসংখ্যা prot_init_done = 0;

phys_bytes vir2phys(শূন্য *vir)
{
	বাহ্যিক অক্ষর _kern_vir_base, _kern_phys_base;	/* in kernel.lds */
	u32_t offset = (vir_bytes) &_kern_vir_base -
		(vir_bytes) &_kern_phys_base;
	ফেরত (phys_bytes)vir - offset;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
শূন্য enable_iop(কাঠামো proc *pp)
{
/* Allow a user process to use I/O instructions.  Change the I/O Permission
 * Level bits in the psw. These specify least-privileged Current Permission
 * Level allowed to execute I/O instructions. Users and servers have CPL 3. 
 * You can't have less privilege than that. Kernel has CPL 0, tasks CPL 1.
 */
  pp->p_reg.psw |= 0x3000;
}


/*===========================================================================*
 *				sdesc					     *
 *===========================================================================*/
 শূন্য sdesc(কাঠামো segdesc_s *segdp, phys_bytes base, vir_bytes আকার_2)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->base_নিচের = base;
  segdp->base_মধ্য = base >> BASE_মধ্য_SHIFT;
  segdp->base_উপরের = base >> BASE_উপরের_SHIFT;

  --আকার_2;			/* convert to a limit, 0 size means 4G */
  যদি (আকার_2 > BYTE_GRAN_MAX) {
	segdp->limit_নিচের = আকার_2 >> PAGE_GRAN_SHIFT;
	segdp->granularity = GRANULAR | (আকার_2 >>
			     (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT));
  } অন্যথা {
	segdp->limit_নিচের = আকার_2;
	segdp->granularity = আকার_2 >> GRANULARITY_SHIFT;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
শূন্য init_param_dataseg(নিবন্ধ কাঠামো segdesc_s *segdp,
	phys_bytes base, vir_bytes আকার_2, ধ্রুবক পূর্ণসংখ্যা privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, base, আকার_2);
	segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

শূন্য init_dataseg(পূর্ণসংখ্যা সূচক, ধ্রুবক পূর্ণসংখ্যা privilege)
{
	init_param_dataseg(&gdt[সূচক], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
স্থির শূন্য init_codeseg(পূর্ণসংখ্যা সূচক, পূর্ণসংখ্যা privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[সূচক], 0, 0xFFFFFFFF);
	gdt[সূচক].access = (privilege << DPL_SHIFT)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

স্থির কাঠামো gate_table_s gate_table_pic[] = {
	{ hwint00, VECTOR( 0), INTR_PRIVILEGE },
	{ hwint01, VECTOR( 1), INTR_PRIVILEGE },
	{ hwint02, VECTOR( 2), INTR_PRIVILEGE },
	{ hwint03, VECTOR( 3), INTR_PRIVILEGE },
	{ hwint04, VECTOR( 4), INTR_PRIVILEGE },
	{ hwint05, VECTOR( 5), INTR_PRIVILEGE },
	{ hwint06, VECTOR( 6), INTR_PRIVILEGE },
	{ hwint07, VECTOR( 7), INTR_PRIVILEGE },
	{ hwint08, VECTOR( 8), INTR_PRIVILEGE },
	{ hwint09, VECTOR( 9), INTR_PRIVILEGE },
	{ hwint10, VECTOR(10), INTR_PRIVILEGE },
	{ hwint11, VECTOR(11), INTR_PRIVILEGE },
	{ hwint12, VECTOR(12), INTR_PRIVILEGE },
	{ hwint13, VECTOR(13), INTR_PRIVILEGE },
	{ hwint14, VECTOR(14), INTR_PRIVILEGE },
	{ hwint15, VECTOR(15), INTR_PRIVILEGE },
	{ ফাঁকা, 0, 0}
};

স্থির কাঠামো gate_table_s gate_table_exceptions[] = {
	{ divide_error, DIVIDE_VECTOR, INTR_PRIVILEGE },
	{ single_step_exception, DEBUG_VECTOR, INTR_PRIVILEGE },
	{ nmi, NMI_VECTOR, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_VECTOR, USER_PRIVILEGE },
	{ overflow, OVERFLOW_VECTOR, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_VECTOR, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_VECTOR, INTR_PRIVILEGE },
	{ copr_not_available, COPROC_NOT_VECTOR, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_VECTOR, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_VECTOR, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_VECTOR, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_VECTOR, INTR_PRIVILEGE },
	{ স্তূপ_exception, স্তূপ_FAULT_VECTOR, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_VECTOR, INTR_PRIVILEGE },
	{ page_fault, PAGE_FAULT_VECTOR, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_VECTOR, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_VECTOR, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_VECTOR, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_VECTOR, INTR_PRIVILEGE },
	{ ipc_entry_softint_orig, IPC_VECTOR_ORIG, USER_PRIVILEGE },
	{ kernel_call_entry_orig, KERN_CALL_VECTOR_ORIG, USER_PRIVILEGE },
	{ ipc_entry_softint_um, IPC_VECTOR_UM, USER_PRIVILEGE },
	{ kernel_call_entry_um, KERN_CALL_VECTOR_UM, USER_PRIVILEGE },
	{ ফাঁকা, 0, 0}
};

পূর্ণসংখ্যা tss_init(unsigned cpu, শূন্য * kernel_স্তূপ)
{
	কাঠামো tss_s * t = &tss[cpu];
	পূর্ণসংখ্যা সূচক = TSS_সূচক(cpu);
	কাঠামো segdesc_s *tssgdt;

	tssgdt = &gdt[সূচক];
  
	init_param_dataseg(tssgdt, (phys_bytes) t,
			আকার(কাঠামো tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, আকার(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = আকার(কাঠামো tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((unsigned) kernel_স্তূপ) - X86_স্তূপ_শীর্ষ_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * আকার(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	যদি(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_entry_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	যদি(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#define set_star_cpu(forcpu) যদি(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_STAR,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_entry_syscall_cpu ## forcpu); }
		set_star_cpu(0);
		set_star_cpu(1);
		set_star_cpu(2);
		set_star_cpu(3);
		set_star_cpu(4);
		set_star_cpu(5);
		set_star_cpu(6);
		set_star_cpu(7);
		প্রতিজ্ঞা(CONFIG_MAX_CPUS <= 8);
  	}

	ফেরত SEG_SELECTOR(সূচক);
}

phys_bytes init_segdesc(পূর্ণসংখ্যা gdt_সূচক, শূন্য *base, পূর্ণসংখ্যা আকার_2)
{
	কাঠামো desctableptr_s *dtp = (কাঠামো desctableptr_s *) &gdt[gdt_সূচক];
	dtp->limit = আকার_2 - 1;
	dtp->base = (phys_bytes) base;

	ফেরত (phys_bytes) dtp;
}

শূন্য int_gate(কাঠামো gatedesc_s *tab,
	unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
/* Build descriptor for an interrupt gate. */
  নিবন্ধ কাঠামো gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->offset_নিচের = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->offset_উপরের = offset >> OFFSET_উপরের_SHIFT;
}

শূন্য int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
	int_gate(idt, vec_nr, offset, dpl_type);
}

শূন্য idt_copy_vectors(কাঠামো gate_table_s * first)
{
	কাঠামো gate_table_s *gtp;
	জন্য (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_bytes) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_SHIFT));
	}
}

শূন্য idt_copy_vectors_pic(শূন্য)
{
	idt_copy_vectors(gate_table_pic);
}

শূন্য idt_init(শূন্য)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_table_exceptions);
}

কাঠামো desctableptr_s gdt_desc, idt_desc;

শূন্য idt_reload(শূন্য)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(পূর্ণসংখ্যা pnr)
{
	পূর্ণসংখ্যা i;

	প্রতিজ্ঞা(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	জন্য(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		পূর্ণসংখ্যা p;
		p = i - NR_TASKS;
		যদি(image[i].proc_nr == pnr) {
			প্রতিজ্ঞা(p < MULTIBOOT_MAX_MODS);
			প্রতিজ্ঞা(p < kinfo.mbi.mi_mods_গুনতি);
			ফেরত &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

পূর্ণসংখ্যা booting_cpu = 0;

শূন্য prot_load_selectors(শূন্য)
{
  /* this function is called by both prot_init by the BSP and
   * the early AP booting code in mpx.S by secondary CPU's.
   * everything is set up the same except for the TSS that is per-CPU.
   */
  x86_lgdt(&gdt_desc);	/* Load gdt */ 
  idt_init();
  idt_reload();
  x86_lldt(LDT_SELECTOR); 	/* Load bogus ldt */
  x86_ltr(TSS_SELECTOR(booting_cpu));

  x86_load_kerncs();
  x86_load_ds(KERN_DS_SELECTOR);
  x86_load_es(KERN_DS_SELECTOR);
  x86_load_fs(KERN_DS_SELECTOR);
  x86_load_gs(KERN_DS_SELECTOR);
  x86_load_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
শূন্য prot_init(শূন্য)
{
  বাহ্যিক অক্ষর k_boot_stktop;

  যদি(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  যদি(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, আকার(gdt));
  memset(idt, 0, আকার(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.base = (u32_t) gdt;
  gdt_desc.limit = আকার(gdt)-1;
  idt_desc.base = (u32_t) idt;
  idt_desc.limit = আকার(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_সূচক],
    (phys_bytes) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_সূচক].access = PRESENT | LDT;
  init_codeseg(KERN_CS_সূচক, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_সূচক, INTR_PRIVILEGE);
  init_codeseg(USER_CS_সূচক, USER_PRIVILEGE);
  init_dataseg(USER_DS_সূচক, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_load_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_identity(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_load();

  prot_init_done = 1;
}

স্থির পূর্ণসংখ্যা alloc_for_vm = 0;

শূন্য arch_post_init(শূন্য)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  কাঠামো proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

স্থির পূর্ণসংখ্যা libexec_pg_alloc(কাঠামো exec_info *execi, vir_bytes vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_load();
        memset((অক্ষর *) vaddr, 0, len);
	alloc_for_vm += len;
        ফেরত OK;
}

শূন্য arch_boot_proc(কাঠামো boot_image *ip, কাঠামো proc *rp)
{
	multiboot_module_t *mod;
	কাঠামো ps_strings *psp;
	অক্ষর *sp;

	যদি(rp->p_nr < 0) ফেরত;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	যদি(rp->p_nr == VM_PROC_NR) {
		কাঠামো exec_info execi;

		memset(&execi, 0, আকার(execi));

		/* exec parameters */
		execi.স্তূপ_উপরের = kinfo.user_sp;
		execi.স্তূপ_আকার = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (অক্ষর *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, আকার(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ফাঁকা;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		যদি(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (অক্ষর *)execi.স্তূপ_উপরের;
		sp -= আকার(কাঠামো ps_strings);
		psp = (কাঠামো ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (আকার(শূন্য *) + আকার(শূন্য *) + আকার(পূর্ণসংখ্যা));

		// linear address space, so it is available.
		psp->ps_argvstr = (অক্ষর **)(sp + আকার(পূর্ণসংখ্যা));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + আকার(শূন্য *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.স্তূপ_উপরের - আকার(কাঠামো ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
