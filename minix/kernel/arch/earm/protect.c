/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/multiboot.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "archconst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/exec.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libexec.h>

ਬਣਤਰ tss_s tss[CONFIG_MAX_CPUS];
ਬਾਹਰੀ ਪੂਰਨ exc_ਸਦਿਸ਼_ਸਾਰਣੀ;

ਪੂਰਨ prot_init_ਪੂਰਾ = 0;

phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *vir)
{
	/* defined in kernel.lds */
	ਬਾਹਰੀ ਅੱਖਰ kern_vir_ਅਧਾਰ, kern_phys_ਅਧਾਰ;
	u32_t ਵਿੱਥ = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ -
	    (vir_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ;
	ਵਾਪਸ (phys_ਬਾਈਟਾਂ)vir - ਵਿੱਥ;
}

ਪੂਰਨ tss_init(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਖਾਲੀ * kernel_ਢੇਰ)
{

	ਬਣਤਰ tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((ਬਿਨਾਨਿਸ਼ਾਨੀ) kernel_ਢੇਰ) - ARM_ਢੇਰ_ਸਿਖਰ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * ਆਕਾਰ(reg_t))) = cpu;

	ਵਾਪਸ 0;
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

ਖਾਲੀ prot_init(ਖਾਲੀ)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_ਸਦਿਸ਼_ਸਾਰਣੀ);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_ਪਛਾਣ_ਤੱਤ(&kinfo); /* Still need 1:1 for device memory . */
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
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
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
