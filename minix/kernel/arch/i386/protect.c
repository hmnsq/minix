/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <sys/exec.h>
#ಅನ್ತರ್ಭಾವ <libexec.h>

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
ವರ್ಣ *video_mem = (ವರ್ಣ *) MULTIBOOT_VIDEO_BUFFER;

/* Storage for gdt, idt and tss. */
ಸಂರಚನಾ segdesc_s gdt[GDT_SIZE] __aligned(DESC_SIZE);
ಸಂರಚನಾ gatedesc_s idt[IDT_SIZE] __aligned(DESC_SIZE);
ಸಂರಚನಾ tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

ಪೂರ್ಣಾಙ್ಕ prot_init_done = 0;

phys_bytes vir2phys(ಶೂನ್ಯ *vir)
{
	ಬಾಹ್ಯ ವರ್ಣ _kern_vir_base, _kern_phys_base;	/* in kernel.lds */
	u32_t offset = (vir_bytes) &_kern_vir_base -
		(vir_bytes) &_kern_phys_base;
	ಪ್ರತ್ಯಾಗಮ (phys_bytes)vir - offset;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
ಶೂನ್ಯ enable_iop(ಸಂರಚನಾ proc *pp)
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
 ಶೂನ್ಯ sdesc(ಸಂರಚನಾ segdesc_s *segdp, phys_bytes base, vir_bytes size)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->base_ಕೆಳಗಿನ = base;
  segdp->base_ಮಧ್ಯ = base >> BASE_ಮಧ್ಯ_SHIFT;
  segdp->base_ಮೇಲಿನ = base >> BASE_ಮೇಲಿನ_SHIFT;

  --size;			/* convert to a limit, 0 size means 4G */
  ಯದಿ (size > BYTE_GRAN_MAX) {
	segdp->limit_ಕೆಳಗಿನ = size >> PAGE_GRAN_SHIFT;
	segdp->granularity = GRANULAR | (size >>
			     (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT));
  } ಅನ್ಯಥಾ {
	segdp->limit_ಕೆಳಗಿನ = size;
	segdp->granularity = size >> GRANULARITY_SHIFT;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
ಶೂನ್ಯ init_param_dataseg(ಪಞ್ಜಿಕಾ ಸಂರಚನಾ segdesc_s *segdp,
	phys_bytes base, vir_bytes size, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, base, size);
	segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

ಶೂನ್ಯ init_dataseg(ಪೂರ್ಣಾಙ್ಕ index, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ privilege)
{
	init_param_dataseg(&gdt[index], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ init_codeseg(ಪೂರ್ಣಾಙ್ಕ index, ಪೂರ್ಣಾಙ್ಕ privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[index], 0, 0xFFFFFFFF);
	gdt[index].access = (privilege << DPL_SHIFT)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

ಸ್ಥಿರ ಸಂರಚನಾ gate_table_s gate_table_pic[] = {
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
	{ ರಿಕ್ತ, 0, 0}
};

ಸ್ಥಿರ ಸಂರಚನಾ gate_table_s gate_table_exceptions[] = {
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
	{ ರಾಶಿ_exception, ರಾಶಿ_FAULT_VECTOR, INTR_PRIVILEGE },
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
	{ ರಿಕ್ತ, 0, 0}
};

ಪೂರ್ಣಾಙ್ಕ tss_init(unsigned cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ)
{
	ಸಂರಚನಾ tss_s * t = &tss[cpu];
	ಪೂರ್ಣಾಙ್ಕ index = TSS_INDEX(cpu);
	ಸಂರಚನಾ segdesc_s *tssgdt;

	tssgdt = &gdt[index];
  
	init_param_dataseg(tssgdt, (phys_bytes) t,
			ಪರಿಮಾಣ(ಸಂರಚನಾ tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, ಪರಿಮಾಣ(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = ಪರಿಮಾಣ(ಸಂರಚನಾ tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((unsigned) kernel_ರಾಶಿ) - X86_ರಾಶಿ_ತುದಿ_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ಪರಿಮಾಣ(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	ಯದಿ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_entry_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	ಯದಿ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#define set_star_cpu(forcpu) ಯದಿ(cpu == forcpu) {				\
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
		ಪ್ರತಿಜ್ಞಾ(CONFIG_MAX_CPUS <= 8);
  	}

	ಪ್ರತ್ಯಾಗಮ SEG_SELECTOR(index);
}

phys_bytes init_segdesc(ಪೂರ್ಣಾಙ್ಕ gdt_index, ಶೂನ್ಯ *base, ಪೂರ್ಣಾಙ್ಕ size)
{
	ಸಂರಚನಾ desctableptr_s *dtp = (ಸಂರಚನಾ desctableptr_s *) &gdt[gdt_index];
	dtp->limit = size - 1;
	dtp->base = (phys_bytes) base;

	ಪ್ರತ್ಯಾಗಮ (phys_bytes) dtp;
}

ಶೂನ್ಯ int_gate(ಸಂರಚನಾ gatedesc_s *tab,
	unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
/* Build descriptor for an interrupt gate. */
  ಪಞ್ಜಿಕಾ ಸಂರಚನಾ gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->offset_ಕೆಳಗಿನ = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->offset_ಮೇಲಿನ = offset >> OFFSET_ಮೇಲಿನ_SHIFT;
}

ಶೂನ್ಯ int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
	int_gate(idt, vec_nr, offset, dpl_type);
}

ಶೂನ್ಯ idt_copy_vectors(ಸಂರಚನಾ gate_table_s * first)
{
	ಸಂರಚನಾ gate_table_s *gtp;
	ಪ್ರತಿ (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_bytes) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_SHIFT));
	}
}

ಶೂನ್ಯ idt_copy_vectors_pic(ಶೂನ್ಯ)
{
	idt_copy_vectors(gate_table_pic);
}

ಶೂನ್ಯ idt_init(ಶೂನ್ಯ)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_table_exceptions);
}

ಸಂರಚನಾ desctableptr_s gdt_desc, idt_desc;

ಶೂನ್ಯ idt_reload(ಶೂನ್ಯ)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(ಪೂರ್ಣಾಙ್ಕ pnr)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿಜ್ಞಾ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	ಪ್ರತಿ(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		ಪೂರ್ಣಾಙ್ಕ p;
		p = i - NR_TASKS;
		ಯದಿ(image[i].proc_nr == pnr) {
			ಪ್ರತಿಜ್ಞಾ(p < MULTIBOOT_MAX_MODS);
			ಪ್ರತಿಜ್ಞಾ(p < kinfo.mbi.mi_mods_ಎಣಿಕೆ);
			ಪ್ರತ್ಯಾಗಮ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

ಪೂರ್ಣಾಙ್ಕ booting_cpu = 0;

ಶೂನ್ಯ prot_load_selectors(ಶೂನ್ಯ)
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
ಶೂನ್ಯ prot_init(ಶೂನ್ಯ)
{
  ಬಾಹ್ಯ ವರ್ಣ k_boot_stktop;

  ಯದಿ(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  ಯದಿ(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, ಪರಿಮಾಣ(gdt));
  memset(idt, 0, ಪರಿಮಾಣ(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.base = (u32_t) gdt;
  gdt_desc.limit = ಪರಿಮಾಣ(gdt)-1;
  idt_desc.base = (u32_t) idt;
  idt_desc.limit = ಪರಿಮಾಣ(idt)-1;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ alloc_for_vm = 0;

ಶೂನ್ಯ arch_post_init(ಶೂನ್ಯ)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  ಸಂರಚನಾ proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ libexec_pg_alloc(ಸಂರಚನಾ exec_info *execi, vir_bytes vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_load();
        memset((ವರ್ಣ *) vaddr, 0, len);
	alloc_for_vm += len;
        ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ arch_boot_proc(ಸಂರಚನಾ boot_image *ip, ಸಂರಚನಾ proc *rp)
{
	multiboot_module_t *mod;
	ಸಂರಚನಾ ps_strings *psp;
	ವರ್ಣ *sp;

	ಯದಿ(rp->p_nr < 0) ಪ್ರತ್ಯಾಗಮ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ಯದಿ(rp->p_nr == VM_PROC_NR) {
		ಸಂರಚನಾ exec_info execi;

		memset(&execi, 0, ಪರಿಮಾಣ(execi));

		/* exec parameters */
		execi.ರಾಶಿ_ಮೇಲಿನ = kinfo.user_sp;
		execi.ರಾಶಿ_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ವರ್ಣ *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, ಪರಿಮಾಣ(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ರಿಕ್ತ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ಯದಿ(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ವರ್ಣ *)execi.ರಾಶಿ_ಮೇಲಿನ;
		sp -= ಪರಿಮಾಣ(ಸಂರಚನಾ ps_strings);
		psp = (ಸಂರಚನಾ ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (ಪರಿಮಾಣ(ಶೂನ್ಯ *) + ಪರಿಮಾಣ(ಶೂನ್ಯ *) + ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));

		// linear address space, so it is available.
		psp->ps_argvstr = (ವರ್ಣ **)(sp + ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + ಪರಿಮಾಣ(ಶೂನ್ಯ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ರಾಶಿ_ಮೇಲಿನ - ಪರಿಮಾಣ(ಸಂರಚನಾ ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
