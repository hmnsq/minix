/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ <minix/cpufeature.h>
#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ "kernel/kernel.h"

#അന്തര്ഭാവ "arch_proto.h"

#അന്തര്ഭാവ <sys/exec.h>
#അന്തര്ഭാവ <libexec.h>

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
അക്ഷരം *video_mem = (അക്ഷരം *) MULTIBOOT_VIDEO_BUFFER;

/* Storage for gdt, idt and tss. */
ഘടന segdesc_s gdt[GDT_SIZE] __aligned(DESC_SIZE);
ഘടന gatedesc_s idt[IDT_SIZE] __aligned(DESC_SIZE);
ഘടന tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

പൂർണ്ണസംഖ്യ prot_init_done = 0;

phys_bytes vir2phys(ശൂന്യം *vir)
{
	ബാഹ്യ അക്ഷരം _kern_vir_base, _kern_phys_base;	/* in kernel.lds */
	u32_t offset = (vir_bytes) &_kern_vir_base -
		(vir_bytes) &_kern_phys_base;
	തിരികെ (phys_bytes)vir - offset;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
ശൂന്യം enable_iop(ഘടന proc *pp)
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
 ശൂന്യം sdesc(ഘടന segdesc_s *segdp, phys_bytes base, vir_bytes size)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->base_താഴെയുള്ള = base;
  segdp->base_നടുവ് = base >> BASE_നടുവ്_SHIFT;
  segdp->base_മുകളിലെ = base >> BASE_മുകളിലെ_SHIFT;

  --size;			/* convert to a limit, 0 size means 4G */
  എങ്കിൽ (size > BYTE_GRAN_MAX) {
	segdp->limit_താഴെയുള്ള = size >> PAGE_GRAN_SHIFT;
	segdp->granularity = GRANULAR | (size >>
			     (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT));
  } അല്ലെങ്കിൽ {
	segdp->limit_താഴെയുള്ള = size;
	segdp->granularity = size >> GRANULARITY_SHIFT;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
ശൂന്യം init_param_dataseg(രജിസ്റ്റർ ഘടന segdesc_s *segdp,
	phys_bytes base, vir_bytes size, സ്ഥിരം പൂർണ്ണസംഖ്യ privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, base, size);
	segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

ശൂന്യം init_dataseg(പൂർണ്ണസംഖ്യ index, സ്ഥിരം പൂർണ്ണസംഖ്യ privilege)
{
	init_param_dataseg(&gdt[index], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം init_codeseg(പൂർണ്ണസംഖ്യ index, പൂർണ്ണസംഖ്യ privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[index], 0, 0xFFFFFFFF);
	gdt[index].access = (privilege << DPL_SHIFT)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

സ്ഥിതം ഘടന gate_table_s gate_table_pic[] = {
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
	{ ശൂന്യ, 0, 0}
};

സ്ഥിതം ഘടന gate_table_s gate_table_exceptions[] = {
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
	{ അട്ടി_exception, അട്ടി_FAULT_VECTOR, INTR_PRIVILEGE },
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
	{ ശൂന്യ, 0, 0}
};

പൂർണ്ണസംഖ്യ tss_init(unsigned cpu, ശൂന്യം * kernel_അട്ടി)
{
	ഘടന tss_s * t = &tss[cpu];
	പൂർണ്ണസംഖ്യ index = TSS_INDEX(cpu);
	ഘടന segdesc_s *tssgdt;

	tssgdt = &gdt[index];
  
	init_param_dataseg(tssgdt, (phys_bytes) t,
			വലുപ്പം(ഘടന tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, വലുപ്പം(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = വലുപ്പം(ഘടന tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((unsigned) kernel_അട്ടി) - X86_അട്ടി_മുകൾ_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * വലുപ്പം(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	എങ്കിൽ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_entry_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	എങ്കിൽ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#define set_star_cpu(forcpu) എങ്കിൽ(cpu == forcpu) {				\
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
		പ്രതിജ്ഞാ(CONFIG_MAX_CPUS <= 8);
  	}

	തിരികെ SEG_SELECTOR(index);
}

phys_bytes init_segdesc(പൂർണ്ണസംഖ്യ gdt_index, ശൂന്യം *base, പൂർണ്ണസംഖ്യ size)
{
	ഘടന desctableptr_s *dtp = (ഘടന desctableptr_s *) &gdt[gdt_index];
	dtp->limit = size - 1;
	dtp->base = (phys_bytes) base;

	തിരികെ (phys_bytes) dtp;
}

ശൂന്യം int_gate(ഘടന gatedesc_s *tab,
	unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
/* Build descriptor for an interrupt gate. */
  രജിസ്റ്റർ ഘടന gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->offset_താഴെയുള്ള = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->offset_മുകളിലെ = offset >> OFFSET_മുകളിലെ_SHIFT;
}

ശൂന്യം int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
	int_gate(idt, vec_nr, offset, dpl_type);
}

ശൂന്യം idt_copy_vectors(ഘടന gate_table_s * first)
{
	ഘടന gate_table_s *gtp;
	വേണ്ടി (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_bytes) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_SHIFT));
	}
}

ശൂന്യം idt_copy_vectors_pic(ശൂന്യം)
{
	idt_copy_vectors(gate_table_pic);
}

ശൂന്യം idt_init(ശൂന്യം)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_table_exceptions);
}

ഘടന desctableptr_s gdt_desc, idt_desc;

ശൂന്യം idt_reload(ശൂന്യം)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(പൂർണ്ണസംഖ്യ pnr)
{
	പൂർണ്ണസംഖ്യ i;

	പ്രതിജ്ഞാ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	വേണ്ടി(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		പൂർണ്ണസംഖ്യ p;
		p = i - NR_TASKS;
		എങ്കിൽ(image[i].proc_nr == pnr) {
			പ്രതിജ്ഞാ(p < MULTIBOOT_MAX_MODS);
			പ്രതിജ്ഞാ(p < kinfo.mbi.mi_mods_എണ്ണം);
			തിരികെ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

പൂർണ്ണസംഖ്യ booting_cpu = 0;

ശൂന്യം prot_load_selectors(ശൂന്യം)
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
ശൂന്യം prot_init(ശൂന്യം)
{
  ബാഹ്യ അക്ഷരം k_boot_stktop;

  എങ്കിൽ(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  എങ്കിൽ(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, വലുപ്പം(gdt));
  memset(idt, 0, വലുപ്പം(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.base = (u32_t) gdt;
  gdt_desc.limit = വലുപ്പം(gdt)-1;
  idt_desc.base = (u32_t) idt;
  idt_desc.limit = വലുപ്പം(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_INDEX],
    (phys_bytes) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_INDEX].access = PRESENT | LDT;
  init_codeseg(KERN_CS_INDEX, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_INDEX, INTR_PRIVILEGE);
  init_codeseg(USER_CS_INDEX, USER_PRIVILEGE);
  init_dataseg(USER_DS_INDEX, USER_PRIVILEGE);

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

സ്ഥിതം പൂർണ്ണസംഖ്യ alloc_for_vm = 0;

ശൂന്യം arch_post_init(ശൂന്യം)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  ഘടന proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ libexec_pg_alloc(ഘടന exec_info *execi, vir_bytes vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_load();
        memset((അക്ഷരം *) vaddr, 0, len);
	alloc_for_vm += len;
        തിരികെ OK;
}

ശൂന്യം arch_boot_proc(ഘടന boot_image *ip, ഘടന proc *rp)
{
	multiboot_module_t *mod;
	ഘടന ps_strings *psp;
	അക്ഷരം *sp;

	എങ്കിൽ(rp->p_nr < 0) തിരികെ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	എങ്കിൽ(rp->p_nr == VM_PROC_NR) {
		ഘടന exec_info execi;

		memset(&execi, 0, വലുപ്പം(execi));

		/* exec parameters */
		execi.അട്ടി_മുകളിലെ = kinfo.user_sp;
		execi.അട്ടി_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (അക്ഷരം *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, വലുപ്പം(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ശൂന്യ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		എങ്കിൽ(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (അക്ഷരം *)execi.അട്ടി_മുകളിലെ;
		sp -= വലുപ്പം(ഘടന ps_strings);
		psp = (ഘടന ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (വലുപ്പം(ശൂന്യം *) + വലുപ്പം(ശൂന്യം *) + വലുപ്പം(പൂർണ്ണസംഖ്യ));

		// linear address space, so it is available.
		psp->ps_argvstr = (അക്ഷരം **)(sp + വലുപ്പം(പൂർണ്ണസംഖ്യ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + വലുപ്പം(ശൂന്യം *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.അട്ടി_മുകളിലെ - വലുപ്പം(ഘടന ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
