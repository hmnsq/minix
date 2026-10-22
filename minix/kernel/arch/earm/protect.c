/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ <machine/multiboot.h>

#അന്തര്ഭാവ "kernel/kernel.h"

#അന്തര്ഭാവ "archconst.h"
#അന്തര്ഭാവ "arch_proto.h"

#അന്തര്ഭാവ <sys/exec.h>
#അന്തര്ഭാവ <libexec.h>

ഘടന tss_s tss[CONFIG_MAX_CPUS];
ബാഹ്യ പൂർണ്ണസംഖ്യ exc_vector_table;

പൂർണ്ണസംഖ്യ prot_init_done = 0;

phys_bytes vir2phys(ശൂന്യം *vir)
{
	/* defined in kernel.lds */
	ബാഹ്യ അക്ഷരം _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	തിരികെ (phys_bytes)vir - offset;
}

പൂർണ്ണസംഖ്യ tss_init(unsigned cpu, ശൂന്യം * kernel_അട്ടി)
{

	ഘടന tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_അട്ടി) - ARM_അട്ടി_മുകൾ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * വലുപ്പം(reg_t))) = cpu;

	തിരികെ 0;
}

multiboot_module_t *bootmod(പൂർണ്ണസംഖ്യ pnr)
{
	പൂർണ്ണസംഖ്യ i;

	പ്രതിജ്ഞാ(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	വേണ്ടി(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		പൂർണ്ണസംഖ്യ p;
		p = i - NR_TASKS;
		എങ്കിൽ(image[i].proc_nr == pnr) {
			പ്രതിജ്ഞാ(p < MULTIBOOT_MAX_MODS);
			പ്രതിജ്ഞാ(p < kinfo.mbi.mi_mods_എണ്ണം);
			തിരികെ &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

പൂർണ്ണസംഖ്യ booting_cpu = 0;

ശൂന്യം prot_init(ശൂന്യം)
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

സ്ഥിതം പൂർണ്ണസംഖ്യ alloc_for_vm = 0;

ശൂന്യം arch_post_init(ശൂന്യം)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	ഘടന proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ libexec_pg_alloc(ഘടന exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((അക്ഷരം *) vaddr, 0, len);
	alloc_for_vm += len;
	തിരികെ OK;
}

ശൂന്യം arch_boot_proc(ഘടന boot_image *ip, ഘടന proc *rp)
{
	multiboot_module_t *mod;
	ഘടന ps_strings *psp;
	അക്ഷരം *sp;

	എങ്കിൽ(rp->p_nr < 0) തിരികെ;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	എങ്കിൽ(rp->p_nr == VM_PROC_NR) {
		ഘടന exec_info execi;

		memset(&execi, 0, വലുപ്പം(execi));

		/* exec parameters */
		execi.അട്ടി_മുകളിലെ = kinfo.user_sp;
		execi.അട്ടി_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (അക്ഷരം *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, വലുപ്പം(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ശൂന്യ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		എങ്കിൽ(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (അക്ഷരം *)execi.അട്ടി_മുകളിലെ;
		sp -= വലുപ്പം(ഘടന ps_strings);
		psp = (ഘടന ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (വലുപ്പം(ശൂന്യം *) + വലുപ്പം(ശൂന്യം *) + വലുപ്പം(പൂർണ്ണസംഖ്യ));

		// linear address space, so it is available.
		psp->ps_argvstr = (അക്ഷരം **)(sp + വലുപ്പം(പൂർണ്ണസംഖ്യ));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + വലുപ്പം(ശൂന്യം *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.അട്ടി_മുകളിലെ - വലുപ്പം(ഘടന ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
