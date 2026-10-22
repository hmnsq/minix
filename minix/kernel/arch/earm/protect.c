/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ <machine/multiboot.h>

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଅନ୍ତର୍ଭାଵ "archconst.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ଅନ୍ତର୍ଭାଵ <sys/exec.h>
#ଅନ୍ତର୍ଭାଵ <libexec.h>

ସଂରଚନା tss_s tss[CONFIG_MAX_CPUS];
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା exc_vector_table;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prot_init_done = 0;

phys_bytes vir2phys(ଶୂନ୍ୟ *vir)
{
	/* defined in kernel.lds */
	ବାହ୍ୟ ଅକ୍ଷର _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	ଫେରନ୍ତୁ (phys_bytes)vir - offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tss_init(unsigned cpu, ଶୂନ୍ୟ * kernel_ଗଦା)
{

	ସଂରଚନା tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_ଗଦା) - ARM_ଗଦା_ଶୀର୍ଷ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ଆକାର(reg_t))) = cpu;

	ଫେରନ୍ତୁ 0;
}

multiboot_module_t *bootmod(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pnr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପ୍ରତିଜ୍ଞା(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	ପାଇଁ(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା p;
		p = i - NR_TASKS;
		ଯଦି(image[i].proc_nr == pnr) {
			ପ୍ରତିଜ୍ଞା(p < MULTIBOOT_MAX_MODS);
			ପ୍ରତିଜ୍ଞା(p < kinfo.mbi.mi_mods_ଗଣନା);
			ଫେରନ୍ତୁ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା booting_cpu = 0;

ଶୂନ୍ୟ prot_init(ଶୂନ୍ୟ)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_vector_table);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_identity(&kinfo); /* Still need 1:1 for device memory . */
	pg_mapkernel();
	pg_load();

	prot_init_done = 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା alloc_for_vm = 0;

ଶୂନ୍ୟ arch_post_init(ଶୂନ୍ୟ)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	ସଂରଚନା proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା libexec_pg_alloc(ସଂରଚନା exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((ଅକ୍ଷର *) vaddr, 0, len);
	alloc_for_vm += len;
	ଫେରନ୍ତୁ OK;
}

ଶୂନ୍ୟ arch_boot_proc(ସଂରଚନା boot_image *ip, ସଂରଚନା proc *rp)
{
	multiboot_module_t *mod;
	ସଂରଚନା ps_strings *psp;
	ଅକ୍ଷର *sp;

	ଯଦି(rp->p_nr < 0) ଫେରନ୍ତୁ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ଯଦି(rp->p_nr == VM_PROC_NR) {
		ସଂରଚନା exec_info execi;

		memset(&execi, 0, ଆକାର(execi));

		/* exec parameters */
		execi.ଗଦା_ଉପର = kinfo.user_sp;
		execi.ଗଦା_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ଅକ୍ଷର *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, ଆକାର(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ଖାଲି;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ଯଦି(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ଅକ୍ଷର *)execi.ଗଦା_ଉପର;
		sp -= ଆକାର(ସଂରଚନା ps_strings);
		psp = (ସଂରଚନା ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (ଆକାର(ଶୂନ୍ୟ *) + ଆକାର(ଶୂନ୍ୟ *) + ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

		// linear address space, so it is available.
		psp->ps_argvstr = (ଅକ୍ଷର **)(sp + ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + ଆକାର(ଶୂନ୍ୟ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ଗଦା_ଉପର - ଆକାର(ସଂରଚନା ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
