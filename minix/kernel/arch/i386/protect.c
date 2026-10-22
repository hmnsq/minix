/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ضمن <assert.h>
#ضمن <string.h>

#ضمن <minix/cpufeature.h>
#ضمن <sys/types.h>
#ضمن "kernel/kernel.h"

#ضمن "arch_proto.h"

#ضمن <sys/exec.h>
#ضمن <libexec.h>

#تعریف_کړه INT_GATE_TYPE	(INT_286_GATE | DESC_386_بټ)
#تعریف_کړه TSS_TYPE	(AVL_286_TSS  | DESC_386_بټ)

/* This is OK initially, when the 1:1 mapping is still there. */
حرف *video_mem = (حرف *) MULTIBOOT_VIDEO_بفر;

/* Storage for gdt, idt and tss. */
بنية segdesc_s gdt[GDT_اندازه] __aligned(DESC_اندازه);
بنية gatedesc_s idt[IDT_اندازه] __aligned(DESC_اندازه);
بنية tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

صحيح prot_init_بشپړ = 0;

phys_بایټونه vir2phys(فراغ *vir)
{
	خارجي حرف kern_vir_بنسټ, kern_phys_بنسټ;	/* in kernel.lds */
	u32_t دننه_واټن = (vir_بایټونه) &kern_vir_بنسټ -
		(vir_بایټونه) &kern_phys_بنسټ;
	أرجع (phys_بایټونه)vir - دننه_واټن;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
فراغ enable_iop(بنية proc *pp)
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
 فراغ sdesc(بنية segdesc_s *segdp, phys_بایټونه بنسټ, vir_بایټونه اندازه)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->بنسټ_ټیټ = بنسټ;
  segdp->بنسټ_منځ = بنسټ >> بنسټ_منځ_ښويدنه;
  segdp->بنسټ_لوړ = بنسټ >> بنسټ_لوړ_ښويدنه;

  --اندازه;			/* convert to a limit, 0 size means 4G */
  إذا (اندازه > BYTE_GRAN_MAX) {
	segdp->حد_ټیټ = اندازه >> پاڼه_GRAN_ښويدنه;
	segdp->granularity = GRANULAR | (اندازه >>
			     (پاڼه_GRAN_ښويدنه + GRANULARITY_ښويدنه));
  } وإلا {
	segdp->حد_ټیټ = اندازه;
	segdp->granularity = اندازه >> GRANULARITY_ښويدنه;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
فراغ init_param_dataseg(سجل بنية segdesc_s *segdp,
	phys_بایټونه بنسټ, vir_بایټونه اندازه, ثابت صحيح privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, بنسټ, اندازه);
	segdp->access = (privilege << DPL_ښويدنه) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

فراغ init_dataseg(صحيح شاخص, ثابت صحيح privilege)
{
	init_param_dataseg(&gdt[شاخص], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
ساكن فراغ init_codeseg(صحيح شاخص, صحيح privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[شاخص], 0, 0xFFFFFFFF);
	gdt[شاخص].access = (privilege << DPL_ښويدنه)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

ساكن بنية gate_جدول_s gate_جدول_pic[] = {
	{ hwint00, ویکټور_2( 0), INTR_PRIVILEGE },
	{ hwint01, ویکټور_2( 1), INTR_PRIVILEGE },
	{ hwint02, ویکټور_2( 2), INTR_PRIVILEGE },
	{ hwint03, ویکټور_2( 3), INTR_PRIVILEGE },
	{ hwint04, ویکټور_2( 4), INTR_PRIVILEGE },
	{ hwint05, ویکټور_2( 5), INTR_PRIVILEGE },
	{ hwint06, ویکټور_2( 6), INTR_PRIVILEGE },
	{ hwint07, ویکټور_2( 7), INTR_PRIVILEGE },
	{ hwint08, ویکټور_2( 8), INTR_PRIVILEGE },
	{ hwint09, ویکټور_2( 9), INTR_PRIVILEGE },
	{ hwint10, ویکټور_2(10), INTR_PRIVILEGE },
	{ hwint11, ویکټور_2(11), INTR_PRIVILEGE },
	{ hwint12, ویکټور_2(12), INTR_PRIVILEGE },
	{ hwint13, ویکټور_2(13), INTR_PRIVILEGE },
	{ hwint14, ویکټور_2(14), INTR_PRIVILEGE },
	{ hwint15, ویکټور_2(15), INTR_PRIVILEGE },
	{ عدم, 0, 0}
};

ساكن بنية gate_جدول_s gate_جدول_exceptions[] = {
	{ divide_error, DIVIDE_ویکټور, INTR_PRIVILEGE },
	{ single_ګام_exception, DEBUG_ویکټور, INTR_PRIVILEGE },
	{ nmi, NMI_ویکټور, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_ویکټور, USER_PRIVILEGE },
	{ ډکېدنه, ډکېدنه_ویکټور, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_ویکټور, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_ویکټور, INTR_PRIVILEGE },
	{ copr_not_شته, COPROC_NOT_ویکټور, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_ویکټور, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_ویکټور, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_ویکټور, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_ویکټور, INTR_PRIVILEGE },
	{ ډېری_exception, ډېری_FAULT_ویکټور, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_ویکټور, INTR_PRIVILEGE },
	{ پاڼه_fault, پاڼه_FAULT_ویکټور, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_ویکټور, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_ویکټور, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_ویکټور, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_ویکټور, INTR_PRIVILEGE },
	{ ipc_ثبت_softint_orig, IPC_ویکټور_ORIG, USER_PRIVILEGE },
	{ kernel_call_ثبت_orig, KERN_CALL_ویکټور_ORIG, USER_PRIVILEGE },
	{ ipc_ثبت_softint_um, IPC_ویکټور_UM, USER_PRIVILEGE },
	{ kernel_call_ثبت_um, KERN_CALL_ویکټور_UM, USER_PRIVILEGE },
	{ عدم, 0, 0}
};

صحيح tss_init(بینښه cpu, فراغ * kernel_ډېری)
{
	بنية tss_s * t = &tss[cpu];
	صحيح شاخص = TSS_شاخص(cpu);
	بنية segdesc_s *tssgdt;

	tssgdt = &gdt[شاخص];
  
	init_param_dataseg(tssgdt, (phys_بایټونه) t,
			حجم(بنية tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_ښويدنه) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, حجم(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = حجم(بنية tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((بینښه) kernel_ډېری) - X86_ډېری_سر_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * حجم(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	إذا(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_ثبت_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	إذا(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#تعریف_کړه سېټ_ستوری_cpu(forcpu) إذا(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_ستوری,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_ثبت_syscall_cpu ## forcpu); }
		سېټ_ستوری_cpu(0);
		سېټ_ستوری_cpu(1);
		سېټ_ستوری_cpu(2);
		سېټ_ستوری_cpu(3);
		سېټ_ستوری_cpu(4);
		سېټ_ستوری_cpu(5);
		سېټ_ستوری_cpu(6);
		سېټ_ستوری_cpu(7);
		أكد(CONFIG_MAX_CPUS <= 8);
  	}

	أرجع SEG_SELECTOR(شاخص);
}

phys_بایټونه init_segdesc(صحيح gdt_شاخص, فراغ *بنسټ, صحيح اندازه)
{
	بنية desctableptr_s *dtp = (بنية desctableptr_s *) &gdt[gdt_شاخص];
	dtp->حد_2 = اندازه - 1;
	dtp->بنسټ = (phys_بایټونه) بنسټ;

	أرجع (phys_بایټونه) dtp;
}

فراغ int_gate(بنية gatedesc_s *tab,
	بینښه vec_nr, vir_بایټونه دننه_واټن, بینښه dpl_type)
{
/* Build descriptor for an interrupt gate. */
  سجل بنية gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->دننه_واټن_ټیټ = دننه_واټن;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->دننه_واټن_لوړ = دننه_واټن >> دننه_واټن_لوړ_ښويدنه;
}

فراغ int_gate_idt(بینښه vec_nr, vir_بایټونه دننه_واټن, بینښه dpl_type)
{
	int_gate(idt, vec_nr, دننه_واټن, dpl_type);
}

فراغ idt_copy_vectors(بنية gate_جدول_s * لومړی)
{
	بنية gate_جدول_s *gtp;
	لكل (gtp = لومړی; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_بایټونه) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_ښويدنه));
	}
}

فراغ idt_copy_vectors_pic(فراغ)
{
	idt_copy_vectors(gate_جدول_pic);
}

فراغ idt_init(فراغ)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_جدول_exceptions);
}

بنية desctableptr_s gdt_desc, idt_desc;

فراغ idt_reload(فراغ)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(صحيح pnr)
{
	صحيح i;

	أكد(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	لكل(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		صحيح p;
		p = i - NR_TASKS;
		إذا(image[i].proc_nr == pnr) {
			أكد(p < MULTIBOOT_MAX_MODS);
			أكد(p < kinfo.mbi.mi_mods_شمېر);
			أرجع &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

صحيح booting_cpu = 0;

فراغ prot_بارول_selectors(فراغ)
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

  x86_بارول_kerncs();
  x86_بارول_ds(KERN_DS_SELECTOR);
  x86_بارول_es(KERN_DS_SELECTOR);
  x86_بارول_fs(KERN_DS_SELECTOR);
  x86_بارول_gs(KERN_DS_SELECTOR);
  x86_بارول_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
فراغ prot_init(فراغ)
{
  خارجي حرف k_boot_stktop;

  إذا(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  إذا(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, حجم(gdt));
  memset(idt, 0, حجم(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.بنسټ = (u32_t) gdt;
  gdt_desc.حد_2 = حجم(gdt)-1;
  idt_desc.بنسټ = (u32_t) idt;
  idt_desc.حد_2 = حجم(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_شاخص],
    (phys_بایټونه) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_شاخص].access = PRESENT | LDT;
  init_codeseg(KERN_CS_شاخص, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_شاخص, INTR_PRIVILEGE);
  init_codeseg(USER_CS_شاخص, USER_PRIVILEGE);
  init_dataseg(USER_DS_شاخص, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_بارول_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_بې_اغېزه_عنصر(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_بارول();

  prot_init_بشپړ = 1;
}

ساكن صحيح alloc_for_vm = 0;

فراغ arch_post_init(فراغ)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  بنية proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

ساكن صحيح libexec_pg_alloc(بنية exec_info *execi, vir_بایټونه vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_بارول();
        memset((حرف *) vaddr, 0, len);
	alloc_for_vm += len;
        أرجع OK;
}

فراغ arch_boot_proc(بنية boot_image *ip, بنية proc *rp)
{
	multiboot_module_t *mod;
	بنية ps_مزي_ګانې *psp;
	حرف *sp;

	إذا(rp->p_nr < 0) أرجع;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	إذا(rp->p_nr == VM_PROC_NR) {
		بنية exec_info execi;

		memset(&execi, 0, حجم(execi));

		/* exec parameters */
		execi.ډېری_لوړ = kinfo.user_sp;
		execi.ډېری_اندازه = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (حرف *) mod->mod_پیل; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_پای_ټکی - mod->mod_پیل;
		strlcpy(execi.progname, ip->proc_name, حجم(execi.progname));
		execi.چوکاټ_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = عدم;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		إذا(libexec_بارول_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (حرف *)execi.ډېری_لوړ;
		sp -= حجم(بنية ps_مزي_ګانې);
		psp = (بنية ps_مزي_ګانې *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (حجم(فراغ *) + حجم(فراغ *) + حجم(صحيح));

		// linear address space, so it is available.
		psp->ps_argvstr = (حرف **)(sp + حجم(صحيح));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + حجم(فراغ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_بایټونه)sp,
			execi.ډېری_لوړ - حجم(بنية ps_مزي_ګانې),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		جمع_کړه_memmap(&kinfo, mod->mod_پیل, mod->mod_پای_ټکی-mod->mod_پیل);
		mod->mod_پای_ټکی = mod->mod_پیل = 0;

		/* Remember them */
		kinfo.vm_allocated_بایټونه = alloc_for_vm;
	}
}
