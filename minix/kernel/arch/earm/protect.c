/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव <machine/multiboot.h>

#अन्तर्भाव "kernel/kernel.h"

#अन्तर्भाव "archconst.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <sys/exec.h>
#अन्तर्भाव <libexec.h>

संरचना tss_s tss[CONFIG_MAX_CPUS];
बाह्य पूर्णाङ्क exc_vector_table;

पूर्णाङ्क prot_init_done = 0;

phys_bytes vir2phys(शून्य *vir)
{
	/* defined in kernel.lds */
	बाह्य वर्ण _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	प्रत्यागम (phys_bytes)vir - offset;
}

पूर्णाङ्क tss_init(unsigned cpu, शून्य * kernel_ढेर)
{

	संरचना tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_ढेर) - ARM_ढेर_शीर्ष_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * परिमाण(reg_t))) = cpu;

	प्रत्यागम 0;
}

multiboot_module_t *bootmod(पूर्णाङ्क pnr)
{
	पूर्णाङ्क i;

	प्रतिज्ञा(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	प्रति(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		पूर्णाङ्क p;
		p = i - NR_TASKS;
		यदि(image[i].proc_nr == pnr) {
			प्रतिज्ञा(p < MULTIBOOT_MAX_MODS);
			प्रतिज्ञा(p < kinfo.mbi.mi_mods_गिनती);
			प्रत्यागम &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

पूर्णाङ्क booting_cpu = 0;

शून्य prot_init(शून्य)
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

स्थिर पूर्णाङ्क alloc_for_vm = 0;

शून्य arch_post_init(शून्य)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	संरचना proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

स्थिर पूर्णाङ्क libexec_pg_alloc(संरचना exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((वर्ण *) vaddr, 0, len);
	alloc_for_vm += len;
	प्रत्यागम OK;
}

शून्य arch_boot_proc(संरचना boot_image *ip, संरचना proc *rp)
{
	multiboot_module_t *mod;
	संरचना ps_strings *psp;
	वर्ण *sp;

	यदि(rp->p_nr < 0) प्रत्यागम;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	यदि(rp->p_nr == VM_PROC_NR) {
		संरचना exec_info execi;

		memset(&execi, 0, परिमाण(execi));

		/* exec parameters */
		execi.ढेर_ऊपरी = kinfo.user_sp;
		execi.ढेर_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (वर्ण *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, परिमाण(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = रिक्त;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		यदि(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (वर्ण *)execi.ढेर_ऊपरी;
		sp -= परिमाण(संरचना ps_strings);
		psp = (संरचना ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (परिमाण(शून्य *) + परिमाण(शून्य *) + परिमाण(पूर्णाङ्क));

		// linear address space, so it is available.
		psp->ps_argvstr = (वर्ण **)(sp + परिमाण(पूर्णाङ्क));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + परिमाण(शून्य *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ढेर_ऊपरी - परिमाण(संरचना ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
