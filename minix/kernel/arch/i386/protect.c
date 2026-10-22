/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ <minix/cpufeature.h>
#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ "kernel/kernel.h"

#அந்தர்பாவ "arch_proto.h"

#அந்தர்பாவ <sys/exec.h>
#அந்தர்பாவ <libexec.h>

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	(AVL_286_TSS  | DESC_386_BIT)

/* This is OK initially, when the 1:1 mapping is still there. */
எழுத்து *video_mem = (எழுத்து *) MULTIBOOT_VIDEO_BUFFER;

/* Storage for gdt, idt and tss. */
கட்டமைப்பு segdesc_s gdt[GDT_SIZE] __aligned(DESC_SIZE);
கட்டமைப்பு gatedesc_s idt[IDT_SIZE] __aligned(DESC_SIZE);
கட்டமைப்பு tss_s tss[CONFIG_MAX_CPUS];

u32_t k_percpu_stacks[CONFIG_MAX_CPUS];

முழு prot_init_done = 0;

phys_bytes vir2phys(வெறுமை *vir)
{
	வெளி எழுத்து _kern_vir_base, _kern_phys_base;	/* in kernel.lds */
	u32_t offset = (vir_bytes) &_kern_vir_base -
		(vir_bytes) &_kern_phys_base;
	திரும்பு (phys_bytes)vir - offset;
}

/*===========================================================================*
 *				enable_iop				     * 
 *===========================================================================*/
வெறுமை enable_iop(கட்டமைப்பு proc *pp)
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
 வெறுமை sdesc(கட்டமைப்பு segdesc_s *segdp, phys_bytes base, vir_bytes size)
{
/* Fill in the size fields (base, limit and granularity) of a descriptor. */
  segdp->base_கீழ் = base;
  segdp->base_நடு = base >> BASE_நடு_SHIFT;
  segdp->base_உயர்வு = base >> BASE_உயர்வு_SHIFT;

  --size;			/* convert to a limit, 0 size means 4G */
  எனில் (size > BYTE_GRAN_MAX) {
	segdp->limit_கீழ் = size >> PAGE_GRAN_SHIFT;
	segdp->granularity = GRANULAR | (size >>
			     (PAGE_GRAN_SHIFT + GRANULARITY_SHIFT));
  } இல்லையெனில் {
	segdp->limit_கீழ் = size;
	segdp->granularity = size >> GRANULARITY_SHIFT;
  }
  segdp->granularity |= DEFAULT;	/* means BIG for data seg */
}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
வெறுமை init_param_dataseg(பதிவு கட்டமைப்பு segdesc_s *segdp,
	phys_bytes base, vir_bytes size, மாறிலி முழு privilege)
{
	/* Build descriptor for a data segment. */
	sdesc(segdp, base, size);
	segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT |
		WRITEABLE | ACCESSED);
		/* EXECUTABLE = 0, EXPAND_DOWN = 0, ACCESSED = 0 */
}

வெறுமை init_dataseg(முழு index, மாறிலி முழு privilege)
{
	init_param_dataseg(&gdt[index], 0, 0xFFFFFFFF, privilege);
}

/*===========================================================================*
 *				init_codeseg				     *
 *===========================================================================*/
நிலையான வெறுமை init_codeseg(முழு index, முழு privilege)
{
	/* Build descriptor for a code segment. */
	sdesc(&gdt[index], 0, 0xFFFFFFFF);
	gdt[index].access = (privilege << DPL_SHIFT)
	        | (PRESENT | SEGMENT | EXECUTABLE | READABLE);
		/* CONFORMING = 0, ACCESSED = 0 */
}

நிலையான கட்டமைப்பு gate_table_s gate_table_pic[] = {
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
	{ வெற்று, 0, 0}
};

நிலையான கட்டமைப்பு gate_table_s gate_table_exceptions[] = {
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
	{ அடுக்கு_exception, அடுக்கு_FAULT_VECTOR, INTR_PRIVILEGE },
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
	{ வெற்று, 0, 0}
};

முழு tss_init(unsigned cpu, வெறுமை * kernel_அடுக்கு)
{
	கட்டமைப்பு tss_s * t = &tss[cpu];
	முழு index = TSS_INDEX(cpu);
	கட்டமைப்பு segdesc_s *tssgdt;

	tssgdt = &gdt[index];
  
	init_param_dataseg(tssgdt, (phys_bytes) t,
			அளவு(கட்டமைப்பு tss_s), INTR_PRIVILEGE);
	tssgdt->access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;

	/* Build TSS. */
	memset(t, 0, அளவு(*t));
	t->ds = t->es = t->fs = t->gs = t->ss0 = KERN_DS_SELECTOR;
	t->cs = KERN_CS_SELECTOR;
	t->iobase = அளவு(கட்டமைப்பு tss_s);	/* empty i/o permissions map */

	/* 
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	k_percpu_stacks[cpu] = t->sp0 = ((unsigned) kernel_அடுக்கு) - X86_அடுக்கு_உச்சி_RESERVED;
	/* 
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * அளவு(reg_t))) = cpu;

	/* Set up Intel SYSENTER support if available. */
	எனில்(minix_feature_flags & MKF_I386_INTEL_SYSENTER) {
	  ia32_msr_write(INTEL_MSR_SYSENTER_CS, 0, KERN_CS_SELECTOR);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_ESP, 0, t->sp0);
  	  ia32_msr_write(INTEL_MSR_SYSENTER_EIP, 0, (u32_t) ipc_entry_sysenter);
  	}

	/* Set up AMD SYSCALL support if available. */
	எனில்(minix_feature_flags & MKF_I386_AMD_SYSCALL) {
		u32_t msr_lo, msr_hi;

		/* set SYSCALL ENABLE bit in EFER MSR */
		ia32_msr_read(AMD_MSR_EFER, &msr_hi, &msr_lo);
		msr_lo |= AMD_EFER_SCE;
		ia32_msr_write(AMD_MSR_EFER, msr_hi, msr_lo);

		/* set STAR register value */
#define set_star_cpu(forcpu) எனில்(cpu == forcpu) {				\
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
		ப்ரதிஜ்ஞா(CONFIG_MAX_CPUS <= 8);
  	}

	திரும்பு SEG_SELECTOR(index);
}

phys_bytes init_segdesc(முழு gdt_index, வெறுமை *base, முழு size)
{
	கட்டமைப்பு desctableptr_s *dtp = (கட்டமைப்பு desctableptr_s *) &gdt[gdt_index];
	dtp->limit = size - 1;
	dtp->base = (phys_bytes) base;

	திரும்பு (phys_bytes) dtp;
}

வெறுமை int_gate(கட்டமைப்பு gatedesc_s *tab,
	unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
/* Build descriptor for an interrupt gate. */
  பதிவு கட்டமைப்பு gatedesc_s *idp;

  idp = &tab[vec_nr];
  idp->offset_கீழ் = offset;
  idp->selector = KERN_CS_SELECTOR;
  idp->p_dpl_type = dpl_type;
  idp->offset_உயர்வு = offset >> OFFSET_உயர்வு_SHIFT;
}

வெறுமை int_gate_idt(unsigned vec_nr, vir_bytes offset, unsigned dpl_type)
{
	int_gate(idt, vec_nr, offset, dpl_type);
}

வெறுமை idt_copy_vectors(கட்டமைப்பு gate_table_s * first)
{
	கட்டமைப்பு gate_table_s *gtp;
	க்கு (gtp = first; gtp->gate; gtp++) {
		int_gate(idt, gtp->vec_nr, (vir_bytes) gtp->gate,
				PRESENT | INT_GATE_TYPE |
				(gtp->privilege << DPL_SHIFT));
	}
}

வெறுமை idt_copy_vectors_pic(வெறுமை)
{
	idt_copy_vectors(gate_table_pic);
}

வெறுமை idt_init(வெறுமை)
{
	idt_copy_vectors_pic();
	idt_copy_vectors(gate_table_exceptions);
}

கட்டமைப்பு desctableptr_s gdt_desc, idt_desc;

வெறுமை idt_reload(வெறுமை)
{
	x86_lidt(&idt_desc);
}

multiboot_module_t *bootmod(முழு pnr)
{
	முழு i;

	ப்ரதிஜ்ஞா(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	க்கு(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		முழு p;
		p = i - NR_TASKS;
		எனில்(image[i].proc_nr == pnr) {
			ப்ரதிஜ்ஞா(p < MULTIBOOT_MAX_MODS);
			ப்ரதிஜ்ஞா(p < kinfo.mbi.mi_mods_மொத்தம்);
			திரும்பு &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

முழு booting_cpu = 0;

வெறுமை prot_load_selectors(வெறுமை)
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
வெறுமை prot_init(வெறுமை)
{
  வெளி எழுத்து k_boot_stktop;

  எனில்(_cpufeature(_CPUF_I386_SYSENTER))
	minix_feature_flags |= MKF_I386_INTEL_SYSENTER;
  எனில்(_cpufeature(_CPUF_I386_SYSCALL))
	minix_feature_flags |= MKF_I386_AMD_SYSCALL;

  memset(gdt, 0, அளவு(gdt));
  memset(idt, 0, அளவு(idt));

  /* Build GDT, IDT, IDT descriptors. */
  gdt_desc.base = (u32_t) gdt;
  gdt_desc.limit = அளவு(gdt)-1;
  idt_desc.base = (u32_t) idt;
  idt_desc.limit = அளவு(idt)-1;
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

நிலையான முழு alloc_for_vm = 0;

வெறுமை arch_post_init(வெறுமை)
{
  /* Let memory mapping code know what's going on at bootstrap time */
  கட்டமைப்பு proc *vm;
  vm = proc_addr(VM_PROC_NR);
  get_cpulocal_var(ptproc) = vm;
  pg_info(&vm->p_seg.p_cr3, &vm->p_seg.p_cr3_v);
}

நிலையான முழு libexec_pg_alloc(கட்டமைப்பு exec_info *execi, vir_bytes vaddr, size_t len)
{
        pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
  	pg_load();
        memset((எழுத்து *) vaddr, 0, len);
	alloc_for_vm += len;
        திரும்பு OK;
}

வெறுமை arch_boot_proc(கட்டமைப்பு boot_image *ip, கட்டமைப்பு proc *rp)
{
	multiboot_module_t *mod;
	கட்டமைப்பு ps_strings *psp;
	எழுத்து *sp;

	எனில்(rp->p_nr < 0) திரும்பு;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	எனில்(rp->p_nr == VM_PROC_NR) {
		கட்டமைப்பு exec_info execi;

		memset(&execi, 0, அளவு(execi));

		/* exec parameters */
		execi.அடுக்கு_உயர்வு = kinfo.user_sp;
		execi.அடுக்கு_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (எழுத்து *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, அளவு(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = வெற்று;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		எனில்(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (எழுத்து *)execi.அடுக்கு_உயர்வு;
		sp -= அளவு(கட்டமைப்பு ps_strings);
		psp = (கட்டமைப்பு ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (அளவு(வெறுமை *) + அளவு(வெறுமை *) + அளவு(முழு));

		// linear address space, so it is available.
		psp->ps_argvstr = (எழுத்து **)(sp + அளவு(முழு));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + அளவு(வெறுமை *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.அடுக்கு_உயர்வு - அளவு(கட்டமைப்பு ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
