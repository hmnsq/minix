/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#شامل <assert.h>
#شامل <string.h>

#شامل <machine/multiboot.h>

#شامل "kernel/kernel.h"

#شامل "archconst.h"
#شامل "arch_proto.h"

#شامل <sys/exec.h>
#شامل <libexec.h>

ساختار tss_s tss[CONFIG_MAX_CPUS];
بیرونی صحیح exc_vector_table;

صحیح prot_init_done = 0;

phys_bytes vir2phys(تهی *vir)
{
	/* defined in kernel.lds */
	بیرونی نویسه _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	بازگشت (phys_bytes)vir - offset;
}

صحیح tss_init(unsigned cpu, تهی * kernel_پشته)
{

	ساختار tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_پشته) - ARM_پشته_بالاترین_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * اندازه(reg_t))) = cpu;

	بازگشت 0;
}

multiboot_module_t *bootmod(صحیح pnr)
{
	صحیح i;

	أكد(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	برای(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		صحیح p;
		p = i - NR_TASKS;
		اگر(image[i].proc_nr == pnr) {
			أكد(p < MULTIBOOT_MAX_MODS);
			أكد(p < kinfo.mbi.mi_mods_شمار);
			بازگشت &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

صحیح booting_cpu = 0;

تهی prot_init(تهی)
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

ایستا صحیح alloc_for_vm = 0;

تهی arch_post_init(تهی)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	ساختار proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

ایستا صحیح libexec_pg_alloc(ساختار exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((نویسه *) vaddr, 0, len);
	alloc_for_vm += len;
	بازگشت OK;
}

تهی arch_boot_proc(ساختار boot_image *ip, ساختار proc *rp)
{
	multiboot_module_t *mod;
	ساختار ps_strings *psp;
	نویسه *sp;

	اگر(rp->p_nr < 0) بازگشت;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	اگر(rp->p_nr == VM_PROC_NR) {
		ساختار exec_info execi;

		memset(&execi, 0, اندازه(execi));

		/* exec parameters */
		execi.پشته_بالا = kinfo.user_sp;
		execi.پشته_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (نویسه *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, اندازه(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = پوچ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		اگر(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (نویسه *)execi.پشته_بالا;
		sp -= اندازه(ساختار ps_strings);
		psp = (ساختار ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (اندازه(تهی *) + اندازه(تهی *) + اندازه(صحیح));

		// linear address space, so it is available.
		psp->ps_argvstr = (نویسه **)(sp + اندازه(صحیح));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + اندازه(تهی *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.پشته_بالا - اندازه(ساختار ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
