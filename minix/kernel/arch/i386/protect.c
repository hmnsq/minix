/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <sys/exec.h>
#अन्तर्भाव <libexec.h>

#परिभाषय INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#परिभाषय TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
वर्ण *video_mem = (वर्ण *) MULTIBOOT_VIDEO_बफर;

/* Storage for gdt, idt and tss. */
संरचना segdesc_s gdt[GDT_आकार] __aligned(DESC_आकार);
संरचना gatedesc_s idt[IDT_आकार] __aligned(DESC_आकार);
संरचना tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

पूर्णाङ्क prot_init_समाप्तम् = 0;

phys_अष्टकानि vir2phys(शून्य *vir)
{
	बाह्य वर्ण kern_vir_आधारः, kern_phys_आधारः;	/* in kernel.lds */
	u32_t विस्थापनम् = (vir_अष्टकानि) &kern_vir_आधारः -
		(vir_अष्टकानि) &kern_phys_आधारः;
	प्रत्यागम (phys_अष्टकानि)vir - विस्थापनम्;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
शून्य enable_iop(संरचना proc *pp)
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
 शून्य sdesc(संरचना segdesc_s *segdp, phys_अष्टकानि आधारः, vir_अष्टकानि आकार)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->आधारः_नीचः = आधारः;
  segdp->आधारः_मध्यः = आधारः >> आधारः_मध्यः_सरणम्;
  segdp->आधारः_उच्चः = आधारः >> आधारः_उच्चः_सरणम्;

  --आकार;			/* convert to a limit, 0 size means 4G */
  यदि (आकार > BYTE_GRAN_MAX) {
	segdp->सीमा_नीचः = आकार >> PAGE_GRAN_सरणम्;
	segdp->granularity = GRANULAR | (आकार >>
			     (PAGE_GRAN_सरणम् + GRANULARITY_सरणम्));
  } अन्यथा {
	segdp->सीमा_नीचः = आकार;
	segdp->granularity = आकार >> GRANULARITY_सरणम्;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
शून्य init_param_dataseg(पञ्जिका संरचना segdesc_s *segdp,
	phys_अष्टकानि आधारः, vir_अष्टकानि आकार, ध्रुव पूर्णाङ्क privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, आधारः, आकार);
	segdp->access = (privilege << DPL_सरणम्) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

शून्य init_dataseg(पूर्णाङ्क सूचकांक, ध्रुव पूर्णाङ्क privilege)
{
	init_param_dataseg(&gdt[सूचकांक], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
स्थिर शून्य init_codeseg(पूर्णाङ्क सूचकांक, पूर्णाङ्क privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[सूचकांक], 0, 0xFFFFFFFF);
	gdt[सूचकांक].access = (privilege << DPL_सरणम्)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

स्थिर संरचना gate_सारणी_s gate_सारणी_pic[] = {
	{ hwint00, सदिशम्_2( 0), INTR_PRIVILEGE },
	{ hwint01, सदिशम्_2( 1), INTR_PRIVILEGE },
	{ hwint02, सदिशम्_2( 2), INTR_PRIVILEGE },
	{ hwint03, सदिशम्_2( 3), INTR_PRIVILEGE },
	{ hwint04, सदिशम्_2( 4), INTR_PRIVILEGE },
	{ hwint05, सदिशम्_2( 5), INTR_PRIVILEGE },
	{ hwint06, सदिशम्_2( 6), INTR_PRIVILEGE },
	{ hwint07, सदिशम्_2( 7), INTR_PRIVILEGE },
	{ hwint08, सदिशम्_2( 8), INTR_PRIVILEGE },
	{ hwint09, सदिशम्_2( 9), INTR_PRIVILEGE },
	{ hwint10, सदिशम्_2(10), INTR_PRIVILEGE },
	{ hwint11, सदिशम्_2(11), INTR_PRIVILEGE },
	{ hwint12, सदिशम्_2(12), INTR_PRIVILEGE },
	{ hwint13, सदिशम्_2(13), INTR_PRIVILEGE },
	{ hwint14, सदिशम्_2(14), INTR_PRIVILEGE },
	{ hwint15, सदिशम्_2(15), INTR_PRIVILEGE },
	{ रिक्त, 0, 0}
};

स्थिर संरचना gate_सारणी_s gate_सारणी_exceptions[] = {
	{ divide_error, DIVIDE_सदिशम्, INTR_PRIVILEGE },
	{ single_चरणः_exception, DEBUG_सदिशम्, INTR_PRIVILEGE },
	{ nmi, NMI_सदिशम्, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_सदिशम्, USER_PRIVILEGE },
	{ अतिप्रवाहः, अतिप्रवाहः_सदिशम्, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_सदिशम्, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_सदिशम्, INTR_PRIVILEGE },
	{ copr_not_उपलब्धम्, COPROC_NOT_सदिशम्, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_सदिशम्, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_सदिशम्, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_सदिशम्, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_सदिशम्, INTR_PRIVILEGE },
	{ राशिः_exception, राशिः_FAULT_सदिशम्, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_सदिशम्, INTR_PRIVILEGE },
	{ page_fault, PAGE_FAULT_सदिशम्, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_सदिशम्, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_सदिशम्, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_सदिशम्, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_सदिशम्, INTR_PRIVILEGE },
	{ ipc_प्रविष्टिः_softint_orig, IPC_सदिशम्_ORIG, USER_PRIVILEGE },
	{ kernel_call_प्रविष्टिः_orig, KERN_CALL_सदिशम्_ORIG, USER_PRIVILEGE },
	{ ipc_प्रविष्टिः_softint_um, IPC_सदिशम्_UM, USER_PRIVILEGE },
	{ kernel_call_प्रविष्टिः_um, KERN_CALL_सदिशम्_UM, USER_PRIVILEGE },
	{ रिक्त, 0, 0}
};

पूर्णाङ्क tss_init(अचिह्नित cpu, शून्य * kernel_राशिः)
{
	संरचना tss_s * t = &tss[cpu];
	पूर्णाङ्क सूचकांक = TSS_सूचकांक(cpu);
	संरचना segdesc_s *tssgdt;

	tssgdt = &gdt[सूचकांक];
  
	init_param_dataseg(tssgdt, (phys_अष्टकानि) t,
			परिमाण(संरचना tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_सरणम्) | TSS_TYPE;

	/* Build TSS. */
	स्मृतिं_पूरय(t, 0, परिमाण(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = परिमाण(संरचना tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((अचिह्नित) kernel_राशिः) - X86_राशिः_शिखरम्_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * परिमाण(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	यदि(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_प्रविष्टिः_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	यदि(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#परिभाषय समुच्चयः_तारकम्_cpu(forcpu) यदि(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_तारकम्,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_प्रविष्टिः_syscall_cpu ## forcpu); }
		समुच्चयः_तारकम्_cpu(0);
		समुच्चयः_तारकम्_cpu(1);
		समुच्चयः_तारकम्_cpu(2);
		समुच्चयः_तारकम्_cpu(3);
		समुच्चयः_तारकम्_cpu(4);
		समुच्चयः_तारकम्_cpu(5);
		समुच्चयः_तारकम्_cpu(6);
		समुच्चयः_तारकम्_cpu(7);
		प्रतिज्ञा(CONFIG_MAX_CPUS <= 8);
  	}

	प्रत्यागम SEG_SELECTOR(सूचकांक);
}

phys_अष्टकानि init_segdesc(पूर्णाङ्क gdt_सूचकांक, शून्य *आधारः, पूर्णाङ्क आकार)
{
	संरचना desctableptr_s *dtp = (संरचना desctableptr_s *) &gdt[gdt_सूचकांक];
	dtp->सीमा_2 = आकार - 1;
	dtp->आधारः = (phys_अष्टकानि) आधारः;

	प्रत्यागम (phys_अष्टकानि) dtp;
}

शून्य int_gate(संरचना gatedesc_s *tab,
	अचिह्नित vec_nr, vir_अष्टकानि विस्थापनम्, अचिह्नित dpl_type)
{
/* Build descriptor for an interrupt gate. */
  पञ्जिका संरचना gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->विस्थापनम्_नीचः = विस्थापनम्;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->विस्थापनम्_उच्चः = विस्थापनम् >> विस्थापनम्_उच्चः_सरणम्;
}

शून्य int_gate_idt(अचिह्नित vec_nr, vir_अष्टकानि विस्थापनम्, अचिह्नित dpl_type)
{
	int_gate(idt, vec_nr, विस्थापनम्, dpl_type);
}

शून्य idt_copy_vectors(संरचना gate_सारणी_s * first)
{
	संरचना gate_सारणी_s *gtp;
	प्रति (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_अष्टकानि) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_सरणम्));
	}
}

शून्य idt_copy_vectors_pic(शून्य)
{
	idt_copy_vectors(gate_सारणी_pic);
}

शून्य idt_init(शून्य)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_सारणी_exceptions);
}

संरचना desctableptr_s gdt_desc, idt_desc;

शून्य idt_reload(शून्य)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(पूर्णाङ्क pnr)
{
	पूर्णाङ्क i;

	प्रतिज्ञा(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	प्रति(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		पूर्णाङ्क p;
		p = i - NR_TASKS;
		यदि(image[i].proc_nr == pnr) {
			प्रतिज्ञा(p < MULTIBOOT_MAX_MODS);
			प्रतिज्ञा(p < kinfo.mbi.mi_mods_गणनम्);
			प्रत्यागम &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

पूर्णाङ्क booting_cpu = 0;

शून्य prot_आरोपय_selectors(शून्य)
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

  x86_आरोपय_kerncs();
  x86_आरोपय_ds(KERN_DS_SELECTOR);
  x86_आरोपय_es(KERN_DS_SELECTOR);
  x86_आरोपय_fs(KERN_DS_SELECTOR);
  x86_आरोपय_gs(KERN_DS_SELECTOR);
  x86_आरोपय_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
शून्य prot_init(शून्य)
{
  बाह्य वर्ण k_boot_stktop;

  यदि(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  यदि(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  स्मृतिं_पूरय(gdt, 0, परिमाण(gdt));
  स्मृतिं_पूरय(idt, 0, परिमाण(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.आधारः = (u32_t) gdt;
  gdt_desc.सीमा_2 = परिमाण(gdt)-1;
  idt_desc.आधारः = (u32_t) idt;
  idt_desc.सीमा_2 = परिमाण(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_सूचकांक],
    (phys_अष्टकानि) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_सूचकांक].access = PRESENT | LDT;
  init_codeseg(KERN_CS_सूचकांक, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_सूचकांक, INTR_PRIVILEGE);
  init_codeseg(USER_CS_सूचकांक, USER_PRIVILEGE);
  init_dataseg(USER_DS_सूचकांक, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_आरोपय_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_तटस्थः(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_आरोपय();

  prot_init_समाप्तम् = 1;
}

स्थिर पूर्णाङ्क alloc_for_vm = 0;

शून्य arch_post_init(शून्य)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  संरचना proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

स्थिर पूर्णाङ्क libexec_pg_alloc(संरचना exec_info *execi, vir_अष्टकानि vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_आरोपय();
        स्मृतिं_पूरय((वर्ण *) vaddr, 0, len);
	alloc_for_vm += len;
        प्रत्यागम OK;
}

शून्य arch_boot_proc(संरचना boot_image *ip, संरचना proc *rp)
{
	multiboot_module_t *mod;
	संरचना ps_वाक्यानि *psp;
	वर्ण *sp;

	यदि(rp->p_nr < 0) प्रत्यागम;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	यदि(rp->p_nr == VM_PROC_NR) {
		संरचना exec_info execi;

		स्मृतिं_पूरय(&execi, 0, परिमाण(execi));

		/* exec parameters */
		execi.राशिः_उच्चः = kinfo.user_sp;
		execi.राशिः_आकार = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (वर्ण *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_अन्तः - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, परिमाण(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = रिक्त;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		यदि(libexec_आरोपय_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (वर्ण *)execi.राशिः_उच्चः;
		sp -= परिमाण(संरचना ps_वाक्यानि);
		psp = (संरचना ps_वाक्यानि *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (परिमाण(शून्य *) + परिमाण(शून्य *) + परिमाण(पूर्णाङ्क));

		// linear address space, so it is available.
		psp->ps_argvstr = (वर्ण **)(sp + परिमाण(पूर्णाङ्क));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + परिमाण(शून्य *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_अष्टकानि)sp,
			execi.राशिः_उच्चः - परिमाण(संरचना ps_वाक्यानि),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		सङ्कलय_memmap(&kinfo, mod->mod_start, mod->mod_अन्तः-mod->mod_start);
		mod->mod_अन्तः = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_अष्टकानि = alloc_for_vm;
	}
}
