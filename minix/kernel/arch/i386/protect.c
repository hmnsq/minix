/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#समावेश <assert.h>
#समावेश <string.h>

#समावेश <minix/cpufeature.h>
#समावेश <sys/types.h>
#समावेश "kernel/kernel.h"

#समावेश "arch_proto.h"

#समावेश <sys/exec.h>
#समावेश <libexec.h>

#घोषणा INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#घोषणा TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
अक्षर *video_mem = (अक्षर *) MULTIBOOT_VIDEO_बफर;

/* Storage for gdt, idt and tss. */
काष्ठा segdesc_s gdt[GDT_आकार] __aligned(DESC_आकार);
काष्ठा gatedesc_s idt[IDT_आकार] __aligned(DESC_आकार);
काष्ठा tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

पूर्णांक prot_init_done = 0;

phys_bytes vir2phys(व्योम *vir)
{
	बाह्य अक्षर _kern_vir_base, _kern_phys_base;	/* in kernel.lds */
	u32_t offset = (vir_bytes) &_kern_vir_base -
		(vir_bytes) &_kern_phys_base;
	वापस (phys_bytes)vir - offset;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
व्योम enable_iop(काष्ठा proc *pp)
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
 व्योम sdesc(काष्ठा segdesc_s *segdp, phys_bytes base, vir_bytes आकार)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->base_निचला = base;
  segdp->base_मध्य = base >> BASE_मध्य_SHIFT;
  segdp->base_ऊपरी = base >> BASE_ऊपरी_SHIFT;

  --आकार;			/* convert to a limit, 0 size means 4G */
  अगर (आकार > BYTE_GRAN_MAX) {
	segdp->limit_निचला = आकार >> PAGE_GRAN_SHIFT;
	segdp->granularity = GRANULAR | (आकार >>
			     (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT));
  } अन्यथा {
	segdp->limit_निचला = आकार;
	segdp->granularity = आकार >> GRANULARITY_SHIFT;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
व्योम init_param_dataseg(रेजिस्टर काष्ठा segdesc_s *segdp,
	phys_bytes base, vir_bytes आकार, स्थिर पूर्णांक privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, base, आकार);
	segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

व्योम init_dataseg(पूर्णांक सूचकांक, स्थिर पूर्णांक privilege)
{
	init_param_dataseg(&gdt[सूचकांक], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
अटल व्योम init_codeseg(पूर्णांक सूचकांक, पूर्णांक privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[सूचकांक], 0, 0xFFFFFFFF);
	gdt[सूचकांक].access = (privilege << DPL_SHIFT)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

अटल काष्ठा gate_table_s gate_table_pic[] = {
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
	{ शून्य, 0, 0}
};

अटल काष्ठा gate_table_s gate_table_exceptions[] = {
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
	{ ढेर_exception, ढेर_FAULT_VECTOR, INTR_PRIVILEGE },
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
	{ शून्य, 0, 0}
};

पूर्णांक tss_init(अचिन्हित cpu, व्योम * kernel_ढेर)
{
	काष्ठा tss_s * t = &tss[cpu];
	पूर्णांक सूचकांक = TSS_सूचकांक(cpu);
	काष्ठा segdesc_s *tssgdt;

	tssgdt = &gdt[सूचकांक];
  
	init_param_dataseg(tssgdt, (phys_bytes) t,
			माप(काष्ठा tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;

	/* Build TSS. */
	स_रखो(t, 0, माप(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = माप(काष्ठा tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((अचिन्हित) kernel_ढेर) - X86_ढेर_शीर्ष_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * माप(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	अगर(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_entry_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	अगर(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#घोषणा set_star_cpu(forcpu) अगर(cpu == forcpu) {				\
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
		निश्चित(CONFIG_MAX_CPUS <= 8);
  	}

	वापस SEG_SELECTOR(सूचकांक);
}

phys_bytes init_segdesc(पूर्णांक gdt_सूचकांक, व्योम *base, पूर्णांक आकार)
{
	काष्ठा desctableptr_s *dtp = (काष्ठा desctableptr_s *) &gdt[gdt_सूचकांक];
	dtp->limit = आकार - 1;
	dtp->base = (phys_bytes) base;

	वापस (phys_bytes) dtp;
}

व्योम int_gate(काष्ठा gatedesc_s *tab,
	अचिन्हित vec_nr, vir_bytes offset, अचिन्हित dpl_type)
{
/* Build descriptor for an interrupt gate. */
  रेजिस्टर काष्ठा gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->offset_निचला = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->offset_ऊपरी = offset >> OFFSET_ऊपरी_SHIFT;
}

व्योम int_gate_idt(अचिन्हित vec_nr, vir_bytes offset, अचिन्हित dpl_type)
{
	int_gate(idt, vec_nr, offset, dpl_type);
}

व्योम idt_copy_vectors(काष्ठा gate_table_s * first)
{
	काष्ठा gate_table_s *gtp;
	क्रम (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_bytes) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_SHIFT));
	}
}

व्योम idt_copy_vectors_pic(व्योम)
{
	idt_copy_vectors(gate_table_pic);
}

व्योम idt_init(व्योम)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_table_exceptions);
}

काष्ठा desctableptr_s gdt_desc, idt_desc;

व्योम idt_reload(व्योम)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(पूर्णांक pnr)
{
	पूर्णांक i;

	निश्चित(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	क्रम(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		पूर्णांक p;
		p = i - NR_TASKS;
		अगर(image[i].proc_nr == pnr) {
			निश्चित(p < MULTIBOOT_MAX_MODS);
			निश्चित(p < kinfo.mbi.mi_mods_गिनती);
			वापस &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

पूर्णांक booting_cpu = 0;

व्योम prot_load_selectors(व्योम)
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
व्योम prot_init(व्योम)
{
  बाह्य अक्षर k_boot_stktop;

  अगर(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  अगर(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  स_रखो(gdt, 0, माप(gdt));
  स_रखो(idt, 0, माप(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.base = (u32_t) gdt;
  gdt_desc.limit = माप(gdt)-1;
  idt_desc.base = (u32_t) idt;
  idt_desc.limit = माप(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_सूचकांक],
    (phys_bytes) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_सूचकांक].access = PRESENT | LDT;
  init_codeseg(KERN_CS_सूचकांक, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_सूचकांक, INTR_PRIVILEGE);
  init_codeseg(USER_CS_सूचकांक, USER_PRIVILEGE);
  init_dataseg(USER_DS_सूचकांक, USER_PRIVILEGE);

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

अटल पूर्णांक alloc_for_vm = 0;

व्योम arch_post_init(व्योम)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  काष्ठा proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

अटल पूर्णांक libexec_pg_alloc(काष्ठा exec_info *execi, vir_bytes vaddr, माप_प्रकार len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_load();
        स_रखो((अक्षर *) vaddr, 0, len);
	alloc_for_vm += len;
        वापस OK;
}

व्योम arch_boot_proc(काष्ठा boot_image *ip, काष्ठा proc *rp)
{
	multiboot_module_t *mod;
	काष्ठा ps_strings *psp;
	अक्षर *sp;

	अगर(rp->p_nr < 0) वापस;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	अगर(rp->p_nr == VM_PROC_NR) {
		काष्ठा exec_info execi;

		स_रखो(&execi, 0, माप(execi));

		/* exec parameters */
		execi.ढेर_ऊपरी = kinfo.user_sp;
		execi.ढेर_आकार = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (अक्षर *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, माप(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = शून्य;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		अगर(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (अक्षर *)execi.ढेर_ऊपरी;
		sp -= माप(काष्ठा ps_strings);
		psp = (काष्ठा ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (माप(व्योम *) + माप(व्योम *) + माप(पूर्णांक));

		// linear address space, so it is available.
		psp->ps_argvstr = (अक्षर **)(sp + माप(पूर्णांक));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + माप(व्योम *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ढेर_ऊपरी - माप(काष्ठा ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
