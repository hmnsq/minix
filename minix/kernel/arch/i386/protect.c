/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#شامل <assert.h>
#شامل <string.h>

#شامل <minix/cpufeature.h>
#شامل <sys/types.h>
#شامل "kernel/kernel.h"

#شامل "arch_proto.h"

#شامل <sys/exec.h>
#شامل <libexec.h>

#تعریف_کرو INT_GATE_TYPE	(INT_286_GATE | DESC_386_ثنائی_ہندسہ)
#تعریف_کرو TSS_TYPE	(AVL_286_TSS  | DESC_386_ثنائی_ہندسہ)

/* This is OK initially, when the 1:1 mapping is still there. */
حرف *video_mem = (حرف *) MULTIBOOT_VIDEO_بفر;

/* Storage for gdt, idt and tss. */
ساخت segdesc_s gdt[GDT_سائز] __aligned(DESC_سائز);
ساخت gatedesc_s idt[IDT_سائز] __aligned(DESC_سائز);
ساخت tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

صحیح prot_init_مکمل = 0;

phys_بائٹیں vir2phys(خالی *vir)
{
	خارجی حرف kern_vir_بنیاد, kern_phys_بنیاد;	/* in kernel.lds */
	u32_t انحراف = (vir_بائٹیں) &kern_vir_بنیاد -
		(vir_بائٹیں) &kern_phys_بنیاد;
	واپس (phys_بائٹیں)vir - انحراف;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
خالی enable_iop(ساخت proc *pp)
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
 خالی sdesc(ساخت segdesc_s *segdp, phys_بائٹیں بنیاد, vir_بائٹیں سائز)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->بنیاد_کم = بنیاد;
  segdp->بنیاد_درمیان = بنیاد >> بنیاد_درمیان_سرکاؤ;
  segdp->بنیاد_بلند = بنیاد >> بنیاد_بلند_سرکاؤ;

  --سائز;			/* convert to a limit, 0 size means 4G */
  اگر (سائز > BYTE_GRAN_MAX) {
	segdp->حد_بندی_کم = سائز >> صفحہ_GRAN_سرکاؤ;
	segdp->granularity = GRANULAR | (سائز >>
			     (صفحہ_GRAN_سرکاؤ + GRANULARITY_سرکاؤ));
  } ورنہ {
	segdp->حد_بندی_کم = سائز;
	segdp->granularity = سائز >> GRANULARITY_سرکاؤ;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
خالی init_param_dataseg(اندراج ساخت segdesc_s *segdp,
	phys_بائٹیں بنیاد, vir_بائٹیں سائز, ثابت صحیح privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, بنیاد, سائز);
	segdp->access = (privilege << DPL_سرکاؤ) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

خالی init_dataseg(صحیح اشاریہ, ثابت صحیح privilege)
{
	init_param_dataseg(&gdt[اشاریہ], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
ساکن خالی init_codeseg(صحیح اشاریہ, صحیح privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[اشاریہ], 0, 0xFFFFFFFF);
	gdt[اشاریہ].access = (privilege << DPL_سرکاؤ)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

ساکن ساخت gate_جدول_s gate_جدول_pic[] = {
	{ hwint00, سمتیہ_2( 0), INTR_PRIVILEGE },
	{ hwint01, سمتیہ_2( 1), INTR_PRIVILEGE },
	{ hwint02, سمتیہ_2( 2), INTR_PRIVILEGE },
	{ hwint03, سمتیہ_2( 3), INTR_PRIVILEGE },
	{ hwint04, سمتیہ_2( 4), INTR_PRIVILEGE },
	{ hwint05, سمتیہ_2( 5), INTR_PRIVILEGE },
	{ hwint06, سمتیہ_2( 6), INTR_PRIVILEGE },
	{ hwint07, سمتیہ_2( 7), INTR_PRIVILEGE },
	{ hwint08, سمتیہ_2( 8), INTR_PRIVILEGE },
	{ hwint09, سمتیہ_2( 9), INTR_PRIVILEGE },
	{ hwint10, سمتیہ_2(10), INTR_PRIVILEGE },
	{ hwint11, سمتیہ_2(11), INTR_PRIVILEGE },
	{ hwint12, سمتیہ_2(12), INTR_PRIVILEGE },
	{ hwint13, سمتیہ_2(13), INTR_PRIVILEGE },
	{ hwint14, سمتیہ_2(14), INTR_PRIVILEGE },
	{ hwint15, سمتیہ_2(15), INTR_PRIVILEGE },
	{ تہی, 0, 0}
};

ساکن ساخت gate_جدول_s gate_جدول_exceptions[] = {
	{ divide_error, DIVIDE_سمتیہ, INTR_PRIVILEGE },
	{ single_قدم_exception, DEBUG_سمتیہ, INTR_PRIVILEGE },
	{ nmi, NMI_سمتیہ, INTR_PRIVILEGE },
	{ breakpoint_exception, BREAKPOINT_سمتیہ, USER_PRIVILEGE },
	{ حد_سے_تجاوز, حد_سے_تجاوز_سمتیہ, USER_PRIVILEGE },
	{ bounds_check, BOUNDS_سمتیہ, INTR_PRIVILEGE },
	{ inval_opcode, INVAL_OP_سمتیہ, INTR_PRIVILEGE },
	{ copr_not_دستیاب, COPROC_NOT_سمتیہ, INTR_PRIVILEGE },
	{ double_fault, DOUBLE_FAULT_سمتیہ, INTR_PRIVILEGE },
	{ copr_seg_overrun, COPROC_SEG_سمتیہ, INTR_PRIVILEGE },
	{ inval_tss, INVAL_TSS_سمتیہ, INTR_PRIVILEGE },
	{ segment_not_present, SEG_NOT_سمتیہ, INTR_PRIVILEGE },
	{ ڈھیر_exception, ڈھیر_FAULT_سمتیہ, INTR_PRIVILEGE },
	{ general_protection, PROTECTION_سمتیہ, INTR_PRIVILEGE },
	{ صفحہ_fault, صفحہ_FAULT_سمتیہ, INTR_PRIVILEGE },
	{ copr_error, COPROC_ERR_سمتیہ, INTR_PRIVILEGE },
	{ alignment_check, ALIGNMENT_CHECK_سمتیہ, INTR_PRIVILEGE },
	{ machine_check, MACHINE_CHECK_سمتیہ, INTR_PRIVILEGE },
	{ simd_exception, SIMD_EXCEPTION_سمتیہ, INTR_PRIVILEGE },
	{ ipc_اندراج_softint_orig, IPC_سمتیہ_ORIG, USER_PRIVILEGE },
	{ kernel_call_اندراج_orig, KERN_CALL_سمتیہ_ORIG, USER_PRIVILEGE },
	{ ipc_اندراج_softint_um, IPC_سمتیہ_UM, USER_PRIVILEGE },
	{ kernel_call_اندراج_um, KERN_CALL_سمتیہ_UM, USER_PRIVILEGE },
	{ تہی, 0, 0}
};

صحیح tss_init(غیرعلامتی cpu, خالی * kernel_ڈھیر)
{
	ساخت tss_s * t = &tss[cpu];
	صحیح اشاریہ = TSS_اشاریہ(cpu);
	ساخت segdesc_s *tssgdt;

	tssgdt = &gdt[اشاریہ];
  
	init_param_dataseg(tssgdt, (phys_بائٹیں) t,
			حجم(ساخت tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_سرکاؤ) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, حجم(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = حجم(ساخت tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((غیرعلامتی) kernel_ڈھیر) - X86_ڈھیر_چوٹی_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * حجم(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	اگر(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_اندراج_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	اگر(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#تعریف_کرو سیٹ_ستارہ_cpu(forcpu) اگر(cpu == forcpu) {				\
		ia32_msr_write(AMD_MSR_ستارہ,					\
		  ((u32_t)USER_CS_SELECTOR << 16) | (u32_t)KERN_CS_SELECTOR,	\
		  (u32_t) ipc_اندراج_syscall_cpu ## forcpu); }
		سیٹ_ستارہ_cpu(0);
		سیٹ_ستارہ_cpu(1);
		سیٹ_ستارہ_cpu(2);
		سیٹ_ستارہ_cpu(3);
		سیٹ_ستارہ_cpu(4);
		سیٹ_ستارہ_cpu(5);
		سیٹ_ستارہ_cpu(6);
		سیٹ_ستارہ_cpu(7);
		أكد(CONFIG_MAX_CPUS <= 8);
  	}

	واپس SEG_SELECTOR(اشاریہ);
}

phys_بائٹیں init_segdesc(صحیح gdt_اشاریہ, خالی *بنیاد, صحیح سائز)
{
	ساخت desctableptr_s *dtp = (ساخت desctableptr_s *) &gdt[gdt_اشاریہ];
	dtp->حد_بندی_2 = سائز - 1;
	dtp->بنیاد = (phys_بائٹیں) بنیاد;

	واپس (phys_بائٹیں) dtp;
}

خالی int_gate(ساخت gatedesc_s *tab,
	غیرعلامتی vec_nr, vir_بائٹیں انحراف, غیرعلامتی dpl_type)
{
/* Build descriptor for an interrupt gate. */
  اندراج ساخت gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->انحراف_کم = انحراف;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->انحراف_بلند = انحراف >> انحراف_بلند_سرکاؤ;
}

خالی int_gate_idt(غیرعلامتی vec_nr, vir_بائٹیں انحراف, غیرعلامتی dpl_type)
{
	int_gate(idt, vec_nr, انحراف, dpl_type);
}

خالی idt_copy_vectors(ساخت gate_جدول_s * پہلا)
{
	ساخت gate_جدول_s *gtp;
	برائے (gtp = پہلا; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_بائٹیں) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_سرکاؤ));
	}
}

خالی idt_copy_vectors_pic(خالی)
{
	idt_copy_vectors(gate_جدول_pic);
}

خالی idt_init(خالی)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_جدول_exceptions);
}

ساخت desctableptr_s gdt_desc, idt_desc;

خالی idt_reload(خالی)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(صحیح pnr)
{
	صحیح i;

	أكد(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	برائے(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		صحیح p;
		p = i - NR_TASKS;
		اگر(image[i].proc_nr == pnr) {
			أكد(p < MULTIBOOT_MAX_MODS);
			أكد(p < kinfo.mbi.mi_mods_گنتی);
			واپس &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

صحیح booting_cpu = 0;

خالی prot_بھرو_selectors(خالی)
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

  x86_بھرو_kerncs();
  x86_بھرو_ds(KERN_DS_SELECTOR);
  x86_بھرو_es(KERN_DS_SELECTOR);
  x86_بھرو_fs(KERN_DS_SELECTOR);
  x86_بھرو_gs(KERN_DS_SELECTOR);
  x86_بھرو_ss(KERN_DS_SELECTOR);
}

/*===========================================================================*
 *				prot_init				     *
 *===========================================================================*/
خالی prot_init(خالی)
{
  خارجی حرف k_boot_stktop;

  اگر(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  اگر(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, حجم(gdt));
  memset(idt, 0, حجم(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.بنیاد = (u32_t) gdt;
  gdt_desc.حد_بندی_2 = حجم(gdt)-1;
  idt_desc.بنیاد = (u32_t) idt;
  idt_desc.حد_بندی_2 = حجم(idt)-1;
  tss_init(0, &k_boot_stktop);

  /* Build GDT */
  init_param_dataseg(&gdt[LDT_اشاریہ],
    (phys_بائٹیں) 0, 0, INTR_PRIVILEGE); /* unusable LDT */
  gdt[LDT_اشاریہ].access = PRESENT | LDT;
  init_codeseg(KERN_CS_اشاریہ, INTR_PRIVILEGE);
  init_dataseg(KERN_DS_اشاریہ, INTR_PRIVILEGE);
  init_codeseg(USER_CS_اشاریہ, USER_PRIVILEGE);
  init_dataseg(USER_DS_اشاریہ, USER_PRIVILEGE);

  /* Currently the multiboot segments are loaded; which is fine, but
   * let's replace them with the ones from our own GDT so we test
   * right away whether they work as expected.
   */
  prot_بھرو_selectors();

  /* Set up a new post-relocate bootstrap pagetable so that
   * we can map in VM, and we no longer rely on pre-relocated
   * data.
   */

  pg_clear();
  pg_شناختی_عنصر(&kinfo); /* Still need 1:1 for lapic and video mem and such. */
  pg_mapkernel();
  pg_بھرو();

  prot_init_مکمل = 1;
}

ساکن صحیح alloc_for_vm = 0;

خالی arch_post_init(خالی)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  ساخت proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

ساکن صحیح libexec_pg_alloc(ساخت exec_info *execi, vir_بائٹیں vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_بھرو();
        memset((حرف *) vaddr, 0, len);
	alloc_for_vm += len;
        واپس OK;
}

خالی arch_boot_proc(ساخت boot_image *ip, ساخت proc *rp)
{
	multiboot_module_t *mod;
	ساخت ps_لڑیاں *psp;
	حرف *sp;

	اگر(rp->p_nr < 0) واپس;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	اگر(rp->p_nr == VM_PROC_NR) {
		ساخت exec_info execi;

		memset(&execi, 0, حجم(execi));

		/* exec parameters */
		execi.ڈھیر_بلند = kinfo.user_sp;
		execi.ڈھیر_سائز = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (حرف *) mod->mod_آغاز; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_انتہا - mod->mod_آغاز;
		strlcpy(execi.progname, ip->proc_name, حجم(execi.progname));
		execi.چوکھٹا_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = تہی;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		اگر(libexec_بھرو_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (حرف *)execi.ڈھیر_بلند;
		sp -= حجم(ساخت ps_لڑیاں);
		psp = (ساخت ps_لڑیاں *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (حجم(خالی *) + حجم(خالی *) + حجم(صحیح));

		// linear address space, so it is available.
		psp->ps_argvstr = (حرف **)(sp + حجم(صحیح));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + حجم(خالی *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_بائٹیں)sp,
			execi.ڈھیر_بلند - حجم(ساخت ps_لڑیاں),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		جوڑو_memmap(&kinfo, mod->mod_آغاز, mod->mod_انتہا-mod->mod_آغاز);
		mod->mod_انتہا = mod->mod_آغاز = 0;

		/* Remember them */
		kinfo.vm_allocated_بائٹیں = alloc_for_vm;
	}
}
