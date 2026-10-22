/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব <machine/multiboot.h>

#অন্তর্ভাব "kernel/kernel.h"

#অন্তর্ভাব "archconst.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব <sys/exec.h>
#অন্তর্ভাব <libexec.h>

কাঠামো tss_s tss[CONFIG_MAX_CPUS];
বাহ্যিক পূর্ণসংখ্যা exc_vector_table;

পূর্ণসংখ্যা prot_init_done = 0;

phys_bytes vir2phys(শূন্য *vir)
{
	/* defined in kernel.lds */
	বাহ্যিক অক্ষর _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	ফেরত (phys_bytes)vir - offset;
}

পূর্ণসংখ্যা tss_init(unsigned cpu, শূন্য * kernel_স্তূপ)
{

	কাঠামো tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_স্তূপ) - ARM_স্তূপ_শীর্ষ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * আকার(reg_t))) = cpu;

	ফেরত 0;
}

multiboot_module_t *bootmod(পূর্ণসংখ্যা pnr)
{
	পূর্ণসংখ্যা i;

	প্রতিজ্ঞা(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	জন্য(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		পূর্ণসংখ্যা p;
		p = i - NR_TASKS;
		যদি(image[i].proc_nr == pnr) {
			প্রতিজ্ঞা(p < MULTIBOOT_MAX_MODS);
			প্রতিজ্ঞা(p < kinfo.mbi.mi_mods_গুনতি);
			ফেরত &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

পূর্ণসংখ্যা booting_cpu = 0;

শূন্য prot_init(শূন্য)
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

স্থির পূর্ণসংখ্যা alloc_for_vm = 0;

শূন্য arch_post_init(শূন্য)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	কাঠামো proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

স্থির পূর্ণসংখ্যা libexec_pg_alloc(কাঠামো exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((অক্ষর *) vaddr, 0, len);
	alloc_for_vm += len;
	ফেরত OK;
}

শূন্য arch_boot_proc(কাঠামো boot_image *ip, কাঠামো proc *rp)
{
	multiboot_module_t *mod;
	কাঠামো ps_strings *psp;
	অক্ষর *sp;

	যদি(rp->p_nr < 0) ফেরত;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	যদি(rp->p_nr == VM_PROC_NR) {
		কাঠামো exec_info execi;

		memset(&execi, 0, আকার(execi));

		/* exec parameters */
		execi.স্তূপ_উপরের = kinfo.user_sp;
		execi.স্তূপ_আকার = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (অক্ষর *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, আকার(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ফাঁকা;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		যদি(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (অক্ষর *)execi.স্তূপ_উপরের;
		sp -= আকার(কাঠামো ps_strings);
		psp = (কাঠামো ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (আকার(শূন্য *) + আকার(শূন্য *) + আকার(পূর্ণসংখ্যা));

		// linear address space, so it is available.
		psp->ps_argvstr = (অক্ষর **)(sp + আকার(পূর্ণসংখ্যা));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + আকার(শূন্য *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.স্তূপ_উপরের - আকার(কাঠামো ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
