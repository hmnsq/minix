/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ <machine/multiboot.h>

#অন্তর্ভাৱ "kernel/kernel.h"

#অন্তর্ভাৱ "archconst.h"
#অন্তর্ভাৱ "arch_proto.h"

#অন্তর্ভাৱ <sys/exec.h>
#অন্তর্ভাৱ <libexec.h>

সংরচনা tss_s tss[CONFIG_MAX_CPUS];
বাহ্য পূর্ণাঙ্ক exc_vector_table;

পূর্ণাঙ্ক prot_init_done = 0;

phys_bytes vir2phys(শূন্য *vir)
{
	/* defined in kernel.lds */
	বাহ্য ৱর্ণ _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	ঘূৰাই (phys_bytes)vir - offset;
}

পূর্ণাঙ্ক tss_init(unsigned cpu, শূন্য * kernel_স্তূপ)
{

	সংরচনা tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_স্তূপ) - ARM_স্তূপ_শীৰ্ষ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * পরিমাণ(reg_t))) = cpu;

	ঘূৰাই 0;
}

multiboot_module_t *bootmod(পূর্ণাঙ্ক pnr)
{
	পূর্ণাঙ্ক i;

	প্রতিজ্ঞা(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	বাবে(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		পূর্ণাঙ্ক p;
		p = i - NR_TASKS;
		যদি(image[i].proc_nr == pnr) {
			প্রতিজ্ঞা(p < MULTIBOOT_MAX_MODS);
			প্রতিজ্ঞা(p < kinfo.mbi.mi_mods_গুনতি);
			ঘূৰাই &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

পূর্ণাঙ্ক booting_cpu = 0;

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

স্থির পূর্ণাঙ্ক alloc_for_vm = 0;

শূন্য arch_post_init(শূন্য)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	সংরচনা proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

স্থির পূর্ণাঙ্ক libexec_pg_alloc(সংরচনা exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((ৱর্ণ *) vaddr, 0, len);
	alloc_for_vm += len;
	ঘূৰাই OK;
}

শূন্য arch_boot_proc(সংরচনা boot_image *ip, সংরচনা proc *rp)
{
	multiboot_module_t *mod;
	সংরচনা ps_strings *psp;
	ৱর্ণ *sp;

	যদি(rp->p_nr < 0) ঘূৰাই;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	যদি(rp->p_nr == VM_PROC_NR) {
		সংরচনা exec_info execi;

		memset(&execi, 0, পরিমাণ(execi));

		/* exec parameters */
		execi.স্তূপ_উপৰেৰ = kinfo.user_sp;
		execi.স্তূপ_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ৱর্ণ *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, পরিমাণ(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = রিক্ত;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		যদি(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ৱর্ণ *)execi.স্তূপ_উপৰেৰ;
		sp -= পরিমাণ(সংরচনা ps_strings);
		psp = (সংরচনা ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (পরিমাণ(শূন্য *) + পরিমাণ(শূন্য *) + পরিমাণ(পূর্ণাঙ্ক));

		// linear address space, so it is available.
		psp->ps_argvstr = (ৱর্ণ **)(sp + পরিমাণ(পূর্ণাঙ্ক));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + পরিমাণ(শূন্য *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.স্তূপ_উপৰেৰ - পরিমাণ(সংরচনা ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
