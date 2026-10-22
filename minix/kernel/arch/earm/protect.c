/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ <machine/multiboot.h>

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಅನ್ತರ್ಭಾವ "archconst.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <sys/exec.h>
#ಅನ್ತರ್ಭಾವ <libexec.h>

ಸಂರಚನಾ tss_s tss[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ exc_vector_table;

ಪೂರ್ಣಾಙ್ಕ prot_init_done = 0;

phys_bytes vir2phys(ಶೂನ್ಯ *vir)
{
	/* defined in kernel.lds */
	ಬಾಹ್ಯ ವರ್ಣ _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	ಪ್ರತ್ಯಾಗಮ (phys_bytes)vir - offset;
}

ಪೂರ್ಣಾಙ್ಕ tss_init(unsigned cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ)
{

	ಸಂರಚನಾ tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_ರಾಶಿ) - ARM_ರಾಶಿ_ತುದಿ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ಪರಿಮಾಣ(reg_t))) = cpu;

	ಪ್ರತ್ಯಾಗಮ 0;
}

multiboot_module_t *bootmod(ಪೂರ್ಣಾಙ್ಕ pnr)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿಜ್ಞಾ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	ಪ್ರತಿ(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		ಪೂರ್ಣಾಙ್ಕ p;
		p = i - NR_TASKS;
		ಯದಿ(image[i].proc_nr == pnr) {
			ಪ್ರತಿಜ್ಞಾ(p < MULTIBOOT_MAX_MODS);
			ಪ್ರತಿಜ್ಞಾ(p < kinfo.mbi.mi_mods_ಎಣಿಕೆ);
			ಪ್ರತ್ಯಾಗಮ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

ಪೂರ್ಣಾಙ್ಕ booting_cpu = 0;

ಶೂನ್ಯ prot_init(ಶೂನ್ಯ)
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ alloc_for_vm = 0;

ಶೂನ್ಯ arch_post_init(ಶೂನ್ಯ)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	ಸಂರಚನಾ proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ libexec_pg_alloc(ಸಂರಚನಾ exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((ವರ್ಣ *) vaddr, 0, len);
	alloc_for_vm += len;
	ಪ್ರತ್ಯಾಗಮ OK;
}

ಶೂನ್ಯ arch_boot_proc(ಸಂರಚನಾ boot_image *ip, ಸಂರಚನಾ proc *rp)
{
	multiboot_module_t *mod;
	ಸಂರಚನಾ ps_strings *psp;
	ವರ್ಣ *sp;

	ಯದಿ(rp->p_nr < 0) ಪ್ರತ್ಯಾಗಮ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ಯದಿ(rp->p_nr == VM_PROC_NR) {
		ಸಂರಚನಾ exec_info execi;

		memset(&execi, 0, ಪರಿಮಾಣ(execi));

		/* exec parameters */
		execi.ರಾಶಿ_ಮೇಲಿನ = kinfo.user_sp;
		execi.ರಾಶಿ_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (ವರ್ಣ *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, ಪರಿಮಾಣ(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ರಿಕ್ತ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ಯದಿ(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (ವರ್ಣ *)execi.ರಾಶಿ_ಮೇಲಿನ;
		sp -= ಪರಿಮಾಣ(ಸಂರಚನಾ ps_strings);
		psp = (ಸಂರಚನಾ ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (ಪರಿಮಾಣ(ಶೂನ್ಯ *) + ಪರಿಮಾಣ(ಶೂನ್ಯ *) + ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));

		// linear address space, so it is available.
		psp->ps_argvstr = (ವರ್ಣ **)(sp + ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + ಪರಿಮಾಣ(ಶೂನ್ಯ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ರಾಶಿ_ಮೇಲಿನ - ಪರಿಮಾಣ(ಸಂರಚನಾ ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
