/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ <sys/exec.h>
#అన్తర్భావ <libexec.h>

#నిర్వచించు INT_GATE_TYPE	(INT_286_GATE | DESC_386_బిట్టు)
#నిర్వచించు TSS_TYPE	(AVL_286_TSS  | DESC_386_బిట్టు)

/* This is OK initially, when the 1:1 mapping is still there. */
అక్షరం *video_mem = (అక్షరం *) MULTIBOOT_VIDEO_బఫరు;

/* Storage for gdt, idt and tss. */
నిర్మాణం segdesc_s gdt[GDT_కొలత] __aligned(DESC_కొలత);
నిర్మాణం gatedesc_s idt[IDT_కొలత] __aligned(DESC_కొలత);
నిర్మాణం tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

పూర్ణాంకం prot_init_పూర్తి = 0;

phys_బైట్లు vir2phys(శూన్య *vir)
{
	బాహ్య అక్షరం kern_vir_పునాది, kern_phys_పునాది;	/* in kernel.lds */
	u32_t స్థానభ్రంశం = (vir_బైట్లు) &kern_vir_పునాది -
		(vir_బైట్లు) &kern_phys_పునాది;
	తిరిగి (phys_బైట్లు)vir - స్థానభ్రంశం;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
శూన్య enable_iop(నిర్మాణం proc *pp)
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
 శూన్య sdesc(నిర్మాణం segdesc_s *segdp, phys_బైట్లు పునాది, vir_బైట్లు కొలత)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->పునాది_దిగువ = పునాది;
  segdp->పునాది_మధ్య = పునాది >> పునాది_మధ్య_జరుపు;
  segdp->పునాది_ఎగువ = పునాది >> పునాది_ఎగువ_జరుపు;

  --కొలత;			/* convert to a limit, 0 size means 4G */
  ఒకవేళ (కొలత > BYTE_GRAN_MAX) {
	segdp->హద్దు_దిగువ = కొలత >> పుట_GRAN_జరుపు;
	segdp->granularity = GRANULAR | (కొలత >>
			     (పుట_GRAN_జరుపు + GRANULARITY_జరుపు));
  } లేకపోతే {
	segdp->హద్దు_దిగువ = కొలత;
	segdp->granularity = కొలత >> GRANULARITY_జరుపు;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
శూన్య init_param_dataseg(నమోదు నిర్మాణం segdesc_s *segdp,
	phys_బైట్లు పునాది, vir_బైట్లు కొలత, స్థిరరాశి పూర్ణాంకం privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, పునాది, కొలత);
	segdp->access = (privilege << DPL_జరుపు) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

శూన్య init_dataseg(పూర్ణాంకం సూచీ, స్థిరరాశి పూర్ణాంకం privilege)
{
	init_param_dataseg(&gdt[సూచీ], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
స్థిర శూన్య init_codeseg(పూర్ణాంకం సూచీ, పూర్ణాంకం privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[సూచీ], 0, 0xFFFFFFFF);
	gdt[సూచీ].access = (privilege << DPL_జరుపు)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

స్థిర నిర్మాణం gate_పట్టిక_s gate_పట్టిక_pic[] = {
	{ hwint00, సదిశ_2( 0), INTR_PRIVILEGE },
	{ hwint01, సదిశ_2( 1), INTR_PRIVILEGE },
	{ hwint02, సదిశ_2( 2), INTR_PRIVILEGE },
	{ hwint03, సదిశ_2( 3), INTR_PRIVILEGE },
	{ hwint04, సదిశ_2( 4), INTR_PRIVILEGE },
	{ hwint05, సదిశ_2( 5), INTR_PRIVILEGE },
	{ hwint06, సదిశ_2( 6), INTR_PRIVILEGE },
	{ hwint07, సదిశ_2( 7), INTR_PRIVILEGE },
	{ hwint08, సదిశ_2( 8), INTR_PRIVILEGE },
	{ hwint09, సదిశ_2( 9), INTR_PRIVILEGE },
	{ hwint10, సదిశ_2(10), INTR_PRIVILEGE },
	{ hwint11, సదిశ_2(11), INTR_PRIVILEGE },
	{ hwint12, సదిశ_2(12), INTR_PRIVILEGE },
	{ hwint13, సదిశ_2(13), INTR_PRIVILEGE },
	{ hwint14, సదిశ_2(14), INTR_PRIVILEGE },
	{ hwint15, సదిశ_2(15), INTR_PRIVILEGE },
	{ ఖాళీ, 0, 0}
};

స్థిర నిర్మాణం gate_పట్టిక_s gate_పట్టిక_exceptions[] = {
	{ divide_error, DIVIDE_సదిశ, INTR_PRIVILEGE },
	{ single_అడుగు_exception, DEBUG_సదిశ, INTR_PRIVILEGE },
	{ nmi, NMI_సదిశ, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_సదిశ, USER_PRIVILEGE },
	{ అధికప్రవాహం, అధికప్రవాహం_సదిశ, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_సదిశ, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_సదిశ, INTR_PRIVILEGE },
	{ copr_not_అందుబాటు, COPROC_NOT_సదిశ, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_సదిశ, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_సదిశ, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_సదిశ, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_సదిశ, INTR_PRIVILEGE },
	{ దొంతర_exception, దొంతర_FAULT_సదిశ, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_సదిశ, INTR_PRIVILEGE },
	{ పుట_fault, పుట_FAULT_సదిశ, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_సదిశ, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_సదిశ, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_సదిశ, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_సదిశ, INTR_PRIVILEGE },
	{ ipc_నమోదు_softint_orig, IPC_సదిశ_ORIG, USER_PRIVILEGE },
	{ kernel_call_నమోదు_orig, KERN_CALL_సదిశ_ORIG, USER_PRIVILEGE },
	{ ipc_నమోదు_softint_um, IPC_సదిశ_UM, USER_PRIVILEGE },
	{ kernel_call_నమోదు_um, KERN_CALL_సదిశ_UM, USER_PRIVILEGE },
	{ ఖాళీ, 0, 0}
};

పూర్ణాంకం tss_init(గుర్తులేని cpu, శూన్య * kernel_దొంతర)
{
	నిర్మాణం tss_s * t = &tss[cpu];
	పూర్ణాంకం సూచీ = TSS_సూచీ(cpu);
	నిర్మాణం segdesc_s *tssgdt;

	tssgdt = &gdt[సూచీ];
  
	init_param_dataseg(tssgdt, (phys_బైట్లు) t,
			పరిమాణం(నిర్మాణం tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_జరుపు) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, పరిమాణం(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = పరిమాణం(నిర్మాణం tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((గుర్తులేని) kernel_దొంతర) - X86_దొంతర_శిఖరం_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * పరిమాణం(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	ఒకవేళ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_నమోదు_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	ఒకవేళ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#నిర్వచించు సమితి_నక్షత్రం_cpu(forcpu) ఒకవేళ(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_నక్షత్రం,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_నమోదు_syscall_cpu ## forcpu); }
		సమితి_నక్షత్రం_cpu(0);
		సమితి_నక్షత్రం_cpu(1);
		సమితి_నక్షత్రం_cpu(2);
		సమితి_నక్షత్రం_cpu(3);
		సమితి_నక్షత్రం_cpu(4);
		సమితి_నక్షత్రం_cpu(5);
		సమితి_నక్షత్రం_cpu(6);
		సమితి_నక్షత్రం_cpu(7);
		ప్రతిజ్ఞా(CONFIG_MAX_CPUS <= 8);
  	}

	తిరిగి SEG_SELECTOR(సూచీ);
}

phys_బైట్లు init_segdesc(పూర్ణాంకం gdt_సూచీ, శూన్య *పునాది, పూర్ణాంకం కొలత)
{
	నిర్మాణం desctableptr_s *dtp = (నిర్మాణం desctableptr_s *) &gdt[gdt_సూచీ];
	dtp->హద్దు_2 = కొలత - 1;
	dtp->పునాది = (phys_బైట్లు) పునాది;

	తిరిగి (phys_బైట్లు) dtp;
}

శూన్య int_gate(నిర్మాణం gatedesc_s *tab,
	గుర్తులేని vec_nr, vir_బైట్లు స్థానభ్రంశం, గుర్తులేని dpl_type)
{
/* Build descriptor for an interrupt gate. */
  నమోదు నిర్మాణం gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->స్థానభ్రంశం_దిగువ = స్థానభ్రంశం;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->స్థానభ్రంశం_ఎగువ = స్థానభ్రంశం >> స్థానభ్రంశం_ఎగువ_జరుపు;
}

శూన్య int_gate_idt(గుర్తులేని vec_nr, vir_బైట్లు స్థానభ్రంశం, గుర్తులేని dpl_type)
{
	int_gate(idt, vec_nr, స్థానభ్రంశం, dpl_type);
}

శూన్య idt_copy_vectors(నిర్మాణం gate_పట్టిక_s * మొదటి)
{
	నిర్మాణం gate_పట్టిక_s *gtp;
	కొరకు (gtp = మొదటి; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_బైట్లు) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_జరుపు));
	}
}

శూన్య idt_copy_vectors_pic(శూన్య)
{
	idt_copy_vectors(gate_పట్టిక_pic);
}

శూన్య idt_init(శూన్య)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_పట్టిక_exceptions);
}

నిర్మాణం desctableptr_s gdt_desc, idt_desc;

శూన్య idt_reload(శూన్య)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(పూర్ణాంకం pnr)
{
	పూర్ణాంకం i;

	ప్రతిజ్ఞా(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	కొరకు(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		పూర్ణాంకం p;
		p = i - NR_TASKS;
		ఒకవేళ(image[i].proc_nr == pnr) {
			ప్రతిజ్ఞా(p < MULTIBOOT_MAX_MODS);
			ప్రతిజ్ఞా(p < kinfo.mbi.mi_mods_లెక్క);
			తిరిగి &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

పూర్ణాంకం booting_cpu = 0;

శూన్య prot_ఎక్కించు_selectors(శూన్య)
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

  x86_ఎక్కించు_kerncs();
  x86_ఎక్కించు_ds(KERN_DS_SELECTOR);
  x86_ఎక్కించు_es(KERN_DS_SELECTOR);
  x86_ఎక్కించు_fs(KERN_DS_SELECTOR);
  x86_ఎక్కించు_gs(KERN_DS_SELECTOR);
  x86_ఎక్కించు_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
శూన్య prot_init(శూన్య)
{
  బాహ్య అక్షరం k_boot_stktop;

  ఒకవేళ(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  ఒకవేళ(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, పరిమాణం(gdt));
  memset(idt, 0, పరిమాణం(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.పునాది = (u32_t) gdt;
  gdt_desc.హద్దు_2 = పరిమాణం(gdt)-1;
  idt_desc.పునాది = (u32_t) idt;
  idt_desc.హద్దు_2 = పరిమాణం(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_సూచీ],
    (phys_బైట్లు) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_సూచీ].access = PRESENT | LDT;
  init_codeseg(KERN_CS_సూచీ, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_సూచీ, INTR_PRIVILEGE);
  init_codeseg(USER_CS_సూచీ, USER_PRIVILEGE);
  init_dataseg(USER_DS_సూచీ, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_ఎక్కించు_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_తటస్థం(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_ఎక్కించు();

  prot_init_పూర్తి = 1;
}

స్థిర పూర్ణాంకం alloc_for_vm = 0;

శూన్య arch_post_init(శూన్య)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  నిర్మాణం proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

స్థిర పూర్ణాంకం libexec_pg_alloc(నిర్మాణం exec_info *execi, vir_బైట్లు vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_ఎక్కించు();
        memset((అక్షరం *) vaddr, 0, len);
	alloc_for_vm += len;
        తిరిగి OK;
}

శూన్య arch_boot_proc(నిర్మాణం boot_image *ip, నిర్మాణం proc *rp)
{
	multiboot_module_t *mod;
	నిర్మాణం ps_అక్షరశ్రేణులు *psp;
	అక్షరం *sp;

	ఒకవేళ(rp->p_nr < 0) తిరిగి;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ఒకవేళ(rp->p_nr == VM_PROC_NR) {
		నిర్మాణం exec_info execi;

		memset(&execi, 0, పరిమాణం(execi));

		/* exec parameters */
		execi.దొంతర_ఎగువ = kinfo.user_sp;
		execi.దొంతర_కొలత = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (అక్షరం *) mod->mod_ప్రారంభం; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_చివర - mod->mod_ప్రారంభం;
		strlcpy(execi.progname, ip->proc_name, పరిమాణం(execi.progname));
		execi.చట్రం_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ఖాళీ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ఒకవేళ(libexec_ఎక్కించు_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (అక్షరం *)execi.దొంతర_ఎగువ;
		sp -= పరిమాణం(నిర్మాణం ps_అక్షరశ్రేణులు);
		psp = (నిర్మాణం ps_అక్షరశ్రేణులు *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (పరిమాణం(శూన్య *) + పరిమాణం(శూన్య *) + పరిమాణం(పూర్ణాంకం));

		// linear address space, so it is available.
		psp->ps_argvstr = (అక్షరం **)(sp + పరిమాణం(పూర్ణాంకం));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + పరిమాణం(శూన్య *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_బైట్లు)sp,
			execi.దొంతర_ఎగువ - పరిమాణం(నిర్మాణం ps_అక్షరశ్రేణులు),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		కూడు_memmap(&kinfo, mod->mod_ప్రారంభం, mod->mod_చివర-mod->mod_ప్రారంభం);
		mod->mod_చివర = mod->mod_ప్రారంభం = 0;

		/* Remember them */
		kinfo.vm_allocated_బైట్లు = alloc_for_vm;
	}
}
