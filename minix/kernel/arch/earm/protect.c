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

ರಚನೆ tss_s tss[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ exc_ಸದಿಶ_ಕೋಷ್ಟಕ;

ಪೂರ್ಣಾಂಕ prot_init_ಮುಗಿದ = 0;

phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *vir)
{
	/* defined in kernel.lds */
	ಬಾಹ್ಯ ಅಕ್ಷರ kern_vir_ತಳ, kern_phys_ತಳ;
	u32_t ಸ್ಥಾನಾಂತರ = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ -
	    (vir_ಬೈಟುಗಳು) &kern_phys_ತಳ;
	ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು)vir - ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ tss_init(ಚಿಹ್ನೆರಹಿತ cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ)
{

	ರಚನೆ tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((ಚಿಹ್ನೆರಹಿತ) kernel_ರಾಶಿ) - ARM_ರಾಶಿ_ತುದಿ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ಗಾತ್ರ(reg_t))) = cpu;

	ಹಿಂತಿರುಗಿ 0;
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

ಶೂನ್ಯ prot_init(ಶೂನ್ಯ)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_ಸದಿಶ_ಕೋಷ್ಟಕ);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_ತಟಸ್ಥ(&kinfo); /* Still need 1:1 for device memory . */
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
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
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
