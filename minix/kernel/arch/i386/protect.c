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

#ವ್ಯಾಖ್ಯಾನಿಸು INT_GATE_TYPE	(INT_286_GATE | DESC_386_ಬಿಟ್)
#ವ್ಯಾಖ್ಯಾನಿಸು TSS_TYPE	(AVL_286_TSS  | DESC_386_ಬಿಟ್)

/* This is OK initially, when the 1:1 mapping is still there. */
ಅಕ್ಷರ *video_mem = (ಅಕ್ಷರ *) MULTIBOOT_VIDEO_ಬಫರ್;

/* Storage for gdt, idt and tss. */
ರಚನೆ segdesc_s gdt[GDT_ಗಾತ್ರ] __aligned(DESC_ಗಾತ್ರ);
ರಚನೆ gatedesc_s idt[IDT_ಗಾತ್ರ] __aligned(DESC_ಗಾತ್ರ);
ರಚನೆ tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

ಪೂರ್ಣಾಂಕ prot_init_ಮುಗಿದ = 0;

phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *vir)
{
	ಬಾಹ್ಯ ಅಕ್ಷರ kern_vir_ತಳ, kern_phys_ತಳ;	/* in kernel.lds */
	u32_t ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ -
		(vir_ಬೈಟುಗಳು) &kern_phys_ತಳ;
	ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು)vir - ಸ್ಥಾನಾಂತರ;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
ಶೂನ್ಯ enable_iop(ರಚನೆ proc *pp)
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
 ಶೂನ್ಯ sdesc(ರಚನೆ segdesc_s *segdp, phys_ಬೈಟುಗಳು ತಳ, vir_ಬೈಟುಗಳು ಗಾತ್ರ_2)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->ತಳ_ಕೆಳಗಿನ = ತಳ;
  segdp->ತಳ_ಮಧ್ಯ = ತಳ >> ತಳ_ಮಧ್ಯ_ಸರಿಸು;
  segdp->ತಳ_ಮೇಲಿನ = ತಳ >> ತಳ_ಮೇಲಿನ_ಸರಿಸು;

  --ಗಾತ್ರ_2;			/* convert to a limit, 0 size means 4G */
  ಆದರೆ (ಗಾತ್ರ_2 > BYTE_GRAN_MAX) {
	segdp->ಎಲ್ಲೆ_ಕೆಳಗಿನ = ಗಾತ್ರ_2 >> ಪುಟ_GRAN_ಸರಿಸು;
	segdp->granularity = GRANULAR | (ಗಾತ್ರ_2 >>
			     (ಪುಟ_GRAN_ಸರಿಸು + GRANULARITY_ಸರಿಸು));
  } ಇಲ್ಲದಿದ್ದರೆ {
	segdp->ಎಲ್ಲೆ_ಕೆಳಗಿನ = ಗಾತ್ರ_2;
	segdp->granularity = ಗಾತ್ರ_2 >> GRANULARITY_ಸರಿಸು;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
ಶೂನ್ಯ init_param_dataseg(ನೋಂದಣಿ ರಚನೆ segdesc_s *segdp,
	phys_ಬೈಟುಗಳು ತಳ, vir_ಬೈಟುಗಳು ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, ತಳ, ಗಾತ್ರ_2);
	segdp->access = (privilege << DPL_ಸರಿಸು) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

ಶೂನ್ಯ init_dataseg(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ privilege)
{
	init_param_dataseg(&gdt[ಸೂಚ್ಯಂಕ], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ init_codeseg(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[ಸೂಚ್ಯಂಕ], 0, 0xFFFFFFFF);
	gdt[ಸೂಚ್ಯಂಕ].access = (privilege << DPL_ಸರಿಸು)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

ಸ್ಥಿರ ರಚನೆ gate_ಕೋಷ್ಟಕ_s gate_ಕೋಷ್ಟಕ_pic[] = {
	{ hwint00, ಸದಿಶ_2( 0), INTR_PRIVILEGE },
	{ hwint01, ಸದಿಶ_2( 1), INTR_PRIVILEGE },
	{ hwint02, ಸದಿಶ_2( 2), INTR_PRIVILEGE },
	{ hwint03, ಸದಿಶ_2( 3), INTR_PRIVILEGE },
	{ hwint04, ಸದಿಶ_2( 4), INTR_PRIVILEGE },
	{ hwint05, ಸದಿಶ_2( 5), INTR_PRIVILEGE },
	{ hwint06, ಸದಿಶ_2( 6), INTR_PRIVILEGE },
	{ hwint07, ಸದಿಶ_2( 7), INTR_PRIVILEGE },
	{ hwint08, ಸದಿಶ_2( 8), INTR_PRIVILEGE },
	{ hwint09, ಸದಿಶ_2( 9), INTR_PRIVILEGE },
	{ hwint10, ಸದಿಶ_2(10), INTR_PRIVILEGE },
	{ hwint11, ಸದಿಶ_2(11), INTR_PRIVILEGE },
	{ hwint12, ಸದಿಶ_2(12), INTR_PRIVILEGE },
	{ hwint13, ಸದಿಶ_2(13), INTR_PRIVILEGE },
	{ hwint14, ಸದಿಶ_2(14), INTR_PRIVILEGE },
	{ hwint15, ಸದಿಶ_2(15), INTR_PRIVILEGE },
	{ ಖಾಲಿ, 0, 0}
};

ಸ್ಥಿರ ರಚನೆ gate_ಕೋಷ್ಟಕ_s gate_ಕೋಷ್ಟಕ_exceptions[] = {
	{ divide_error, DIVIDE_ಸದಿಶ, INTR_PRIVILEGE },
	{ single_ಹೆಜ್ಜೆ_exception, DEBUG_ಸದಿಶ, INTR_PRIVILEGE },
	{ nmi, NMI_ಸದಿಶ, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_ಸದಿಶ, USER_PRIVILEGE },
	{ ಅತಿಪ್ರವಾಹ, ಅತಿಪ್ರವಾಹ_ಸದಿಶ, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_ಸದಿಶ, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_ಸದಿಶ, INTR_PRIVILEGE },
	{ copr_not_ಲಭ್ಯ, COPROC_NOT_ಸದಿಶ, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_ಸದಿಶ, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_ಸದಿಶ, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_ಸದಿಶ, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_ಸದಿಶ, INTR_PRIVILEGE },
	{ ರಾಶಿ_exception, ರಾಶಿ_FAULT_ಸದಿಶ, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_ಸದಿಶ, INTR_PRIVILEGE },
	{ ಪುಟ_fault, ಪುಟ_FAULT_ಸದಿಶ, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_ಸದಿಶ, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_ಸದಿಶ, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_ಸದಿಶ, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_ಸದಿಶ, INTR_PRIVILEGE },
	{ ipc_ನಮೂದು_softint_orig, IPC_ಸದಿಶ_ORIG, USER_PRIVILEGE },
	{ kernel_call_ನಮೂದು_orig, KERN_CALL_ಸದಿಶ_ORIG, USER_PRIVILEGE },
	{ ipc_ನಮೂದು_softint_um, IPC_ಸದಿಶ_UM, USER_PRIVILEGE },
	{ kernel_call_ನಮೂದು_um, KERN_CALL_ಸದಿಶ_UM, USER_PRIVILEGE },
	{ ಖಾಲಿ, 0, 0}
};

ಪೂರ್ಣಾಂಕ tss_init(ಚಿಹ್ನೆರಹಿತ cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ)
{
	ರಚನೆ tss_s * t = &tss[cpu];
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = TSS_ಸೂಚ್ಯಂಕ(cpu);
	ರಚನೆ segdesc_s *tssgdt;

	tssgdt = &gdt[ಸೂಚ್ಯಂಕ];
  
	init_param_dataseg(tssgdt, (phys_ಬೈಟುಗಳು) t,
			ಗಾತ್ರ(ರಚನೆ tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_ಸರಿಸು) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, ಗಾತ್ರ(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = ಗಾತ್ರ(ರಚನೆ tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((ಚಿಹ್ನೆರಹಿತ) kernel_ರಾಶಿ) - X86_ರಾಶಿ_ತುದಿ_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ಗಾತ್ರ(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	ಆದರೆ(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_ನಮೂದು_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	ಆದರೆ(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ನಕ್ಷತ್ರ_cpu(forcpu) ಆದರೆ(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_ನಕ್ಷತ್ರ,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_ನಮೂದು_syscall_cpu ## forcpu); }
		ಗಣ_ನಕ್ಷತ್ರ_cpu(0);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(1);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(2);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(3);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(4);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(5);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(6);
		ಗಣ_ನಕ್ಷತ್ರ_cpu(7);
		ಪ್ರತಿಜ್ಞಾ(CONFIG_MAX_CPUS <= 8);
  	}

	ಹಿಂತಿರುಗಿ SEG_SELECTOR(ಸೂಚ್ಯಂಕ);
}

phys_ಬೈಟುಗಳು init_segdesc(ಪೂರ್ಣಾಂಕ gdt_ಸೂಚ್ಯಂಕ, ಶೂನ್ಯ *ತಳ, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
	ರಚನೆ desctableptr_s *dtp = (ರಚನೆ desctableptr_s *) &gdt[gdt_ಸೂಚ್ಯಂಕ];
	dtp->ಎಲ್ಲೆ_2 = ಗಾತ್ರ_2 - 1;
	dtp->ತಳ = (phys_ಬೈಟುಗಳು) ತಳ;

	ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು) dtp;
}

ಶೂನ್ಯ int_gate(ರಚನೆ gatedesc_s *tab,
	ಚಿಹ್ನೆರಹಿತ vec_nr, vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ dpl_type)
{
/* Build descriptor for an interrupt gate. */
  ನೋಂದಣಿ ರಚನೆ gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->ಸ್ಥಾನಾಂತರ_ಕೆಳಗಿನ = ಸ್ಥಾನಾಂತರ;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ = ಸ್ಥಾನಾಂತರ >> ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ_ಸರಿಸು;
}

ಶೂನ್ಯ int_gate_idt(ಚಿಹ್ನೆರಹಿತ vec_nr, vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ dpl_type)
{
	int_gate(idt, vec_nr, ಸ್ಥಾನಾಂತರ, dpl_type);
}

ಶೂನ್ಯ idt_copy_vectors(ರಚನೆ gate_ಕೋಷ್ಟಕ_s * ಮೊದಲ)
{
	ರಚನೆ gate_ಕೋಷ್ಟಕ_s *gtp;
	ಗಾಗಿ (gtp = ಮೊದಲ; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_ಬೈಟುಗಳು) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_ಸರಿಸು));
	}
}

ಶೂನ್ಯ idt_copy_vectors_pic(ಶೂನ್ಯ)
{
	idt_copy_vectors(gate_ಕೋಷ್ಟಕ_pic);
}

ಶೂನ್ಯ idt_init(ಶೂನ್ಯ)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_ಕೋಷ್ಟಕ_exceptions);
}

ರಚನೆ desctableptr_s gdt_desc, idt_desc;

ಶೂನ್ಯ idt_reload(ಶೂನ್ಯ)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(ಪೂರ್ಣಾಂಕ pnr)
{
	ಪೂರ್ಣಾಂಕ i;

	ಪ್ರತಿಜ್ಞಾ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	ಗಾಗಿ(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		ಪೂರ್ಣಾಂಕ p;
		p = i - NR_TASKS;
		ಆದರೆ(image[i].proc_nr == pnr) {
			ಪ್ರತಿಜ್ಞಾ(p < MULTIBOOT_MAX_MODS);
			ಪ್ರತಿಜ್ಞಾ(p < kinfo.mbi.mi_mods_ಎಣಿಕೆ);
			ಹಿಂತಿರುಗಿ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

ಪೂರ್ಣಾಂಕ booting_cpu = 0;

ಶೂನ್ಯ prot_ತುಂಬು_selectors(ಶೂನ್ಯ)
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

  x86_ತುಂಬು_kerncs();
  x86_ತುಂಬು_ds(KERN_DS_SELECTOR);
  x86_ತುಂಬು_es(KERN_DS_SELECTOR);
  x86_ತುಂಬು_fs(KERN_DS_SELECTOR);
  x86_ತುಂಬು_gs(KERN_DS_SELECTOR);
  x86_ತುಂಬು_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
ಶೂನ್ಯ prot_init(ಶೂನ್ಯ)
{
  ಬಾಹ್ಯ ಅಕ್ಷರ k_boot_stktop;

  ಆದರೆ(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  ಆದರೆ(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, ಗಾತ್ರ(gdt));
  memset(idt, 0, ಗಾತ್ರ(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.ತಳ = (u32_t) gdt;
  gdt_desc.ಎಲ್ಲೆ_2 = ಗಾತ್ರ(gdt)-1;
  idt_desc.ತಳ = (u32_t) idt;
  idt_desc.ಎಲ್ಲೆ_2 = ಗಾತ್ರ(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_ಸೂಚ್ಯಂಕ],
    (phys_ಬೈಟುಗಳು) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_ಸೂಚ್ಯಂಕ].access = PRESENT | LDT;
  init_codeseg(KERN_CS_ಸೂಚ್ಯಂಕ, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_ಸೂಚ್ಯಂಕ, INTR_PRIVILEGE);
  init_codeseg(USER_CS_ಸೂಚ್ಯಂಕ, USER_PRIVILEGE);
  init_dataseg(USER_DS_ಸೂಚ್ಯಂಕ, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_ತುಂಬು_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_ತಟಸ್ಥ(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_ತುಂಬು();

  prot_init_ಮುಗಿದ = 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ alloc_for_vm = 0;

ಶೂನ್ಯ arch_post_init(ಶೂನ್ಯ)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  ರಚನೆ proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ libexec_pg_alloc(ರಚನೆ exec_info *execi, vir_ಬೈಟುಗಳು vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_ತುಂಬು();
        memset((ಅಕ್ಷರ *) vaddr, 0, len);
	alloc_for_vm += len;
        ಹಿಂತಿರುಗಿ OK;
}

ಶೂನ್ಯ arch_boot_proc(ರಚನೆ boot_image *ip, ರಚನೆ proc *rp)
{
	multiboot_module_t *mod;
	ರಚನೆ ps_ಅಕ್ಷರಸರಣಿಗಳು *psp;
	ಅಕ್ಷರ *sp;

	ಆದರೆ(rp->p_nr < 0) ಹಿಂತಿರುಗಿ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ಆದರೆ(rp->p_nr == VM_PROC_NR) {
		ರಚನೆ exec_info execi;

		memset(&execi, 0, ಗಾತ್ರ(execi));

		/* exec parameters */
		execi.ರಾಶಿ_ಮೇಲಿನ = kinfo.user_sp;
		execi.ರಾಶಿ_ಗಾತ್ರ = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ಅಕ್ಷರ *) mod->mod_ಆರಂಭ; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_ಕೊನೆ - mod->mod_ಆರಂಭ;
		strlcpy(execi.progname, ip->proc_name, ಗಾತ್ರ(execi.progname));
		execi.ಚೌಕಟ್ಟು_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ಖಾಲಿ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ಆದರೆ(libexec_ತುಂಬು_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ಅಕ್ಷರ *)execi.ರಾಶಿ_ಮೇಲಿನ;
		sp -= ಗಾತ್ರ(ರಚನೆ ps_ಅಕ್ಷರಸರಣಿಗಳು);
		psp = (ರಚನೆ ps_ಅಕ್ಷರಸರಣಿಗಳು *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (ಗಾತ್ರ(ಶೂನ್ಯ *) + ಗಾತ್ರ(ಶೂನ್ಯ *) + ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));

		// linear address space, so it is available.
		psp->ps_argvstr = (ಅಕ್ಷರ **)(sp + ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + ಗಾತ್ರ(ಶೂನ್ಯ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_ಬೈಟುಗಳು)sp,
			execi.ರಾಶಿ_ಮೇಲಿನ - ಗಾತ್ರ(ರಚನೆ ps_ಅಕ್ಷರಸರಣಿಗಳು),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		ಕೂಡಿಸು_memmap(&kinfo, mod->mod_ಆರಂಭ, mod->mod_ಕೊನೆ-mod->mod_ಆರಂಭ);
		mod->mod_ಕೊನೆ = mod->mod_ಆರಂಭ = 0;

		/* Remember them */
		kinfo.vm_allocated_ಬೈಟುಗಳು = alloc_for_vm;
	}
}
