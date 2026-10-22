/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/exec.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libexec.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_GATE_TYPE	(INT_286_GATE | DESC_386_ਦੋਆਧਾਰੀ_ਅੰਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSS_TYPE	(AVL_286_TSS  | DESC_386_ਦੋਆਧਾਰੀ_ਅੰਕ)

/* This is OK initially, when the 1:1 mapping is still there. */
ਅੱਖਰ *video_mem = (ਅੱਖਰ *) MULTIBOOT_VIDEO_ਬਫਰ;

/* Storage for gdt, idt and tss. */
ਬਣਤਰ segdesc_s gdt[GDT_ਮਾਪ] __aligned(DESC_ਮਾਪ);
ਬਣਤਰ gatedesc_s idt[IDT_ਮਾਪ] __aligned(DESC_ਮਾਪ);
ਬਣਤਰ tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

ਪੂਰਨ prot_init_ਪੂਰਾ = 0;

phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *vir)
{
	ਬਾਹਰੀ ਅੱਖਰ kern_vir_ਅਧਾਰ, kern_phys_ਅਧਾਰ;	/* in kernel.lds */
	u32_t ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ -
		(vir_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ;
	ਵਾਪਸ (phys_ਬਾਈਟਾਂ)vir - ਵਿੱਥ;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
ਖਾਲੀ enable_iop(ਬਣਤਰ proc *pp)
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
 ਖਾਲੀ sdesc(ਬਣਤਰ segdesc_s *segdp, phys_ਬਾਈਟਾਂ ਅਧਾਰ, vir_ਬਾਈਟਾਂ ਮਾਪ)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->ਅਧਾਰ_ਨੀਵਾਂ = ਅਧਾਰ;
  segdp->ਅਧਾਰ_ਵਿਚਕਾਰ = ਅਧਾਰ >> ਅਧਾਰ_ਵਿਚਕਾਰ_ਖਿਸਕਾਓ;
  segdp->ਅਧਾਰ_ਉੱਚਾ = ਅਧਾਰ >> ਅਧਾਰ_ਉੱਚਾ_ਖਿਸਕਾਓ;

  --ਮਾਪ;			/* convert to a limit, 0 size means 4G */
  ਜੇ (ਮਾਪ > BYTE_GRAN_MAX) {
	segdp->ਹੱਦਬੰਦੀ_ਨੀਵਾਂ = ਮਾਪ >> ਪੰਨਾ_GRAN_ਖਿਸਕਾਓ;
	segdp->granularity = GRANULAR | (ਮਾਪ >>
			     (ਪੰਨਾ_GRAN_ਖਿਸਕਾਓ + GRANULARITY_ਖਿਸਕਾਓ));
  } ਨਹੀਂਤਾਂ {
	segdp->ਹੱਦਬੰਦੀ_ਨੀਵਾਂ = ਮਾਪ;
	segdp->granularity = ਮਾਪ >> GRANULARITY_ਖਿਸਕਾਓ;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
ਖਾਲੀ init_param_dataseg(ਰਜਿਸਟਰ ਬਣਤਰ segdesc_s *segdp,
	phys_ਬਾਈਟਾਂ ਅਧਾਰ, vir_ਬਾਈਟਾਂ ਮਾਪ, ਅਚਲ ਪੂਰਨ privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, ਅਧਾਰ, ਮਾਪ);
	segdp->access = (privilege << DPL_ਖਿਸਕਾਓ) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

ਖਾਲੀ init_dataseg(ਪੂਰਨ ਸੂਚਕਾਂਕ, ਅਚਲ ਪੂਰਨ privilege)
{
	init_param_dataseg(&gdt[ਸੂਚਕਾਂਕ], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ init_codeseg(ਪੂਰਨ ਸੂਚਕਾਂਕ, ਪੂਰਨ privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[ਸੂਚਕਾਂਕ], 0, 0xFFFFFFFF);
	gdt[ਸੂਚਕਾਂਕ].access = (privilege << DPL_ਖਿਸਕਾਓ)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

ਸਥਿਰ ਬਣਤਰ gate_ਸਾਰਣੀ_s gate_ਸਾਰਣੀ_pic[] = {
	{ hwint00, ਸਦਿਸ਼_2( 0), INTR_PRIVILEGE },
	{ hwint01, ਸਦਿਸ਼_2( 1), INTR_PRIVILEGE },
	{ hwint02, ਸਦਿਸ਼_2( 2), INTR_PRIVILEGE },
	{ hwint03, ਸਦਿਸ਼_2( 3), INTR_PRIVILEGE },
	{ hwint04, ਸਦਿਸ਼_2( 4), INTR_PRIVILEGE },
	{ hwint05, ਸਦਿਸ਼_2( 5), INTR_PRIVILEGE },
	{ hwint06, ਸਦਿਸ਼_2( 6), INTR_PRIVILEGE },
	{ hwint07, ਸਦਿਸ਼_2( 7), INTR_PRIVILEGE },
	{ hwint08, ਸਦਿਸ਼_2( 8), INTR_PRIVILEGE },
	{ hwint09, ਸਦਿਸ਼_2( 9), INTR_PRIVILEGE },
	{ hwint10, ਸਦਿਸ਼_2(10), INTR_PRIVILEGE },
	{ hwint11, ਸਦਿਸ਼_2(11), INTR_PRIVILEGE },
	{ hwint12, ਸਦਿਸ਼_2(12), INTR_PRIVILEGE },
	{ hwint13, ਸਦਿਸ਼_2(13), INTR_PRIVILEGE },
	{ hwint14, ਸਦਿਸ਼_2(14), INTR_PRIVILEGE },
	{ hwint15, ਸਦਿਸ਼_2(15), INTR_PRIVILEGE },
	{ ਨਿਰਾ, 0, 0}
};

ਸਥਿਰ ਬਣਤਰ gate_ਸਾਰਣੀ_s gate_ਸਾਰਣੀ_exceptions[] = {
	{ divide_error, DIVIDE_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ single_ਕਦਮ_exception, DEBUG_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ nmi, NMI_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_ਸਦਿਸ਼, USER_PRIVILEGE },
	{ ਉਛਾਲ, ਉਛਾਲ_ਸਦਿਸ਼, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ copr_not_ਉਪਲਬਧ, COPROC_NOT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ ਢੇਰ_exception, ਢੇਰ_FAULT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ ਪੰਨਾ_fault, ਪੰਨਾ_FAULT_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_ਸਦਿਸ਼, INTR_PRIVILEGE },
	{ ipc_ਇੰਦਰਾਜ_softint_orig, IPC_ਸਦਿਸ਼_ORIG, USER_PRIVILEGE },
	{ kernel_call_ਇੰਦਰਾਜ_orig, KERN_CALL_ਸਦਿਸ਼_ORIG, USER_PRIVILEGE },
	{ ipc_ਇੰਦਰਾਜ_softint_um, IPC_ਸਦਿਸ਼_UM, USER_PRIVILEGE },
	{ kernel_call_ਇੰਦਰਾਜ_um, KERN_CALL_ਸਦਿਸ਼_UM, USER_PRIVILEGE },
	{ ਨਿਰਾ, 0, 0}
};

ਪੂਰਨ tss_init(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਖਾਲੀ * kernel_ਢੇਰ)
{
	ਬਣਤਰ tss_s * t = &tss[cpu];
	ਪੂਰਨ ਸੂਚਕਾਂਕ = TSS_ਸੂਚਕਾਂਕ(cpu);
	ਬਣਤਰ segdesc_s *tssgdt;

	tssgdt = &gdt[ਸੂਚਕਾਂਕ];
  
	init_param_dataseg(tssgdt, (phys_ਬਾਈਟਾਂ) t,
			ਆਕਾਰ(ਬਣਤਰ tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_ਖਿਸਕਾਓ) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, ਆਕਾਰ(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = ਆਕਾਰ(ਬਣਤਰ tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((ਬਿਨਾਨਿਸ਼ਾਨੀ) kernel_ਢੇਰ) - X86_ਢੇਰ_ਸਿਖਰ_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ਆਕਾਰ(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	ਜੇ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_ਇੰਦਰਾਜ_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	ਜੇ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਤਾਰਾ_cpu(forcpu) ਜੇ(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_ਤਾਰਾ,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_ਇੰਦਰਾਜ_syscall_cpu ## forcpu); }
		ਸੈੱਟ_ਤਾਰਾ_cpu(0);
		ਸੈੱਟ_ਤਾਰਾ_cpu(1);
		ਸੈੱਟ_ਤਾਰਾ_cpu(2);
		ਸੈੱਟ_ਤਾਰਾ_cpu(3);
		ਸੈੱਟ_ਤਾਰਾ_cpu(4);
		ਸੈੱਟ_ਤਾਰਾ_cpu(5);
		ਸੈੱਟ_ਤਾਰਾ_cpu(6);
		ਸੈੱਟ_ਤਾਰਾ_cpu(7);
		ਪ੍ਰਤਿਜ੍ਞਾ(CONFIG_MAX_CPUS <= 8);
  	}

	ਵਾਪਸ SEG_SELECTOR(ਸੂਚਕਾਂਕ);
}

phys_ਬਾਈਟਾਂ init_segdesc(ਪੂਰਨ gdt_ਸੂਚਕਾਂਕ, ਖਾਲੀ *ਅਧਾਰ, ਪੂਰਨ ਮਾਪ)
{
	ਬਣਤਰ desctableptr_s *dtp = (ਬਣਤਰ desctableptr_s *) &gdt[gdt_ਸੂਚਕਾਂਕ];
	dtp->ਹੱਦਬੰਦੀ_2 = ਮਾਪ - 1;
	dtp->ਅਧਾਰ = (phys_ਬਾਈਟਾਂ) ਅਧਾਰ;

	ਵਾਪਸ (phys_ਬਾਈਟਾਂ) dtp;
}

ਖਾਲੀ int_gate(ਬਣਤਰ gatedesc_s *tab,
	ਬਿਨਾਨਿਸ਼ਾਨੀ vec_nr, vir_ਬਾਈਟਾਂ ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ dpl_type)
{
/* Build descriptor for an interrupt gate. */
  ਰਜਿਸਟਰ ਬਣਤਰ gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->ਵਿੱਥ_ਨੀਵਾਂ = ਵਿੱਥ;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->ਵਿੱਥ_ਉੱਚਾ = ਵਿੱਥ >> ਵਿੱਥ_ਉੱਚਾ_ਖਿਸਕਾਓ;
}

ਖਾਲੀ int_gate_idt(ਬਿਨਾਨਿਸ਼ਾਨੀ vec_nr, vir_ਬਾਈਟਾਂ ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ dpl_type)
{
	int_gate(idt, vec_nr, ਵਿੱਥ, dpl_type);
}

ਖਾਲੀ idt_copy_vectors(ਬਣਤਰ gate_ਸਾਰਣੀ_s * ਪਹਿਲਾ)
{
	ਬਣਤਰ gate_ਸਾਰਣੀ_s *gtp;
	ਲਈ (gtp = ਪਹਿਲਾ; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_ਬਾਈਟਾਂ) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_ਖਿਸਕਾਓ));
	}
}

ਖਾਲੀ idt_copy_vectors_pic(ਖਾਲੀ)
{
	idt_copy_vectors(gate_ਸਾਰਣੀ_pic);
}

ਖਾਲੀ idt_init(ਖਾਲੀ)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_ਸਾਰਣੀ_exceptions);
}

ਬਣਤਰ desctableptr_s gdt_desc, idt_desc;

ਖਾਲੀ idt_reload(ਖਾਲੀ)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(ਪੂਰਨ pnr)
{
	ਪੂਰਨ i;

	ਪ੍ਰਤਿਜ੍ਞਾ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	ਲਈ(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		ਪੂਰਨ p;
		p = i - NR_TASKS;
		ਜੇ(image[i].proc_nr == pnr) {
			ਪ੍ਰਤਿਜ੍ਞਾ(p < MULTIBOOT_MAX_MODS);
			ਪ੍ਰਤਿਜ੍ਞਾ(p < kinfo.mbi.mi_mods_ਗਿਣਤੀ);
			ਵਾਪਸ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

ਪੂਰਨ booting_cpu = 0;

ਖਾਲੀ prot_ਲੱਦੋ_selectors(ਖਾਲੀ)
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

  x86_ਲੱਦੋ_kerncs();
  x86_ਲੱਦੋ_ds(KERN_DS_SELECTOR);
  x86_ਲੱਦੋ_es(KERN_DS_SELECTOR);
  x86_ਲੱਦੋ_fs(KERN_DS_SELECTOR);
  x86_ਲੱਦੋ_gs(KERN_DS_SELECTOR);
  x86_ਲੱਦੋ_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
ਖਾਲੀ prot_init(ਖਾਲੀ)
{
  ਬਾਹਰੀ ਅੱਖਰ k_boot_stktop;

  ਜੇ(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  ਜੇ(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, ਆਕਾਰ(gdt));
  memset(idt, 0, ਆਕਾਰ(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.ਅਧਾਰ = (u32_t) gdt;
  gdt_desc.ਹੱਦਬੰਦੀ_2 = ਆਕਾਰ(gdt)-1;
  idt_desc.ਅਧਾਰ = (u32_t) idt;
  idt_desc.ਹੱਦਬੰਦੀ_2 = ਆਕਾਰ(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_ਸੂਚਕਾਂਕ],
    (phys_ਬਾਈਟਾਂ) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_ਸੂਚਕਾਂਕ].access = PRESENT | LDT;
  init_codeseg(KERN_CS_ਸੂਚਕਾਂਕ, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_ਸੂਚਕਾਂਕ, INTR_PRIVILEGE);
  init_codeseg(USER_CS_ਸੂਚਕਾਂਕ, USER_PRIVILEGE);
  init_dataseg(USER_DS_ਸੂਚਕਾਂਕ, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_ਲੱਦੋ_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_ਪਛਾਣ_ਤੱਤ(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_ਲੱਦੋ();

  prot_init_ਪੂਰਾ = 1;
}

ਸਥਿਰ ਪੂਰਨ alloc_for_vm = 0;

ਖਾਲੀ arch_post_init(ਖਾਲੀ)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  ਬਣਤਰ proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

ਸਥਿਰ ਪੂਰਨ libexec_pg_alloc(ਬਣਤਰ exec_info *execi, vir_ਬਾਈਟਾਂ vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_ਲੱਦੋ();
        memset((ਅੱਖਰ *) vaddr, 0, len);
	alloc_for_vm += len;
        ਵਾਪਸ OK;
}

ਖਾਲੀ arch_boot_proc(ਬਣਤਰ boot_image *ip, ਬਣਤਰ proc *rp)
{
	multiboot_module_t *mod;
	ਬਣਤਰ ps_ਲੜੀਆਂ *psp;
	ਅੱਖਰ *sp;

	ਜੇ(rp->p_nr < 0) ਵਾਪਸ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ਜੇ(rp->p_nr == VM_PROC_NR) {
		ਬਣਤਰ exec_info execi;

		memset(&execi, 0, ਆਕਾਰ(execi));

		/* exec parameters */
		execi.ਢੇਰ_ਉੱਚਾ = kinfo.user_sp;
		execi.ਢੇਰ_ਮਾਪ = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ਅੱਖਰ *) mod->mod_ਸ਼ੁਰੂ; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_ਸਿਰਾ - mod->mod_ਸ਼ੁਰੂ;
		strlcpy(execi.progname, ip->proc_name, ਆਕਾਰ(execi.progname));
		execi.ਚੌਖਟਾ_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ਨਿਰਾ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ਜੇ(libexec_ਲੱਦੋ_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ਅੱਖਰ *)execi.ਢੇਰ_ਉੱਚਾ;
		sp -= ਆਕਾਰ(ਬਣਤਰ ps_ਲੜੀਆਂ);
		psp = (ਬਣਤਰ ps_ਲੜੀਆਂ *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (ਆਕਾਰ(ਖਾਲੀ *) + ਆਕਾਰ(ਖਾਲੀ *) + ਆਕਾਰ(ਪੂਰਨ));

		// linear address space, so it is available.
		psp->ps_argvstr = (ਅੱਖਰ **)(sp + ਆਕਾਰ(ਪੂਰਨ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + ਆਕਾਰ(ਖਾਲੀ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_ਬਾਈਟਾਂ)sp,
			execi.ਢੇਰ_ਉੱਚਾ - ਆਕਾਰ(ਬਣਤਰ ps_ਲੜੀਆਂ),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		ਜੋੜੋ_memmap(&kinfo, mod->mod_ਸ਼ੁਰੂ, mod->mod_ਸਿਰਾ-mod->mod_ਸ਼ੁਰੂ);
		mod->mod_ਸਿਰਾ = mod->mod_ਸ਼ੁਰੂ = 0;

		/* Remember them */
		kinfo.vm_allocated_ਬਾਈਟਾਂ = alloc_for_vm;
	}
}
