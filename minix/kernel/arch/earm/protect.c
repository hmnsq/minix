/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#समावेश <assert.h>
#समावेश <string.h>

#समावेश <machine/multiboot.h>

#समावेश "kernel/kernel.h"

#समावेश "archconst.h"
#समावेश "arch_proto.h"

#समावेश <sys/exec.h>
#समावेश <libexec.h>

काष्ठा tss_s tss[CONFIG_MAX_CPUS];
बाह्य पूर्णांक exc_vector_table;

पूर्णांक prot_init_done = 0;

phys_bytes vir2phys(व्योम *vir)
{
	/* defined in kernel.lds */
	बाह्य अक्षर _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	वापस (phys_bytes)vir - offset;
}

पूर्णांक tss_init(अचिन्हित cpu, व्योम * kernel_ढेर)
{

	काष्ठा tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((अचिन्हित) kernel_ढेर) - ARM_ढेर_शीर्ष_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * माप(reg_t))) = cpu;

	वापस 0;
}

multiboot_module_t *bootmod(पूर्णांक pnr)
{
	पूर्णांक i;

	निश्चित(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	क्रम(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		पूर्णांक p;
		p = i - NR_TASKS;
		अगर(image[i].proc_nr == pnr) {
			निश्चित(p < MULTIBOOT_MAX_MODS);
			निश्चित(p < kinfo.mbi.mi_mods_गिनती);
			वापस &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

पूर्णांक booting_cpu = 0;

व्योम prot_init(व्योम)
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

अटल पूर्णांक alloc_for_vm = 0;

व्योम arch_post_init(व्योम)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	काष्ठा proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

अटल पूर्णांक libexec_pg_alloc(काष्ठा exec_info *execi, vir_bytes vaddr, माप_प्रकार len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	स_रखो((अक्षर *) vaddr, 0, len);
	alloc_for_vm += len;
	वापस OK;
}

व्योम arch_boot_proc(काष्ठा boot_image *ip, काष्ठा proc *rp)
{
	multiboot_module_t *mod;
	काष्ठा ps_strings *psp;
	अक्षर *sp;

	अगर(rp->p_nr < 0) वापस;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	अगर(rp->p_nr == VM_PROC_NR) {
		काष्ठा exec_info execi;

		स_रखो(&execi, 0, माप(execi));

		/* exec parameters */
		execi.ढेर_ऊपरी = kinfo.user_sp;
		execi.ढेर_आकार = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (अक्षर *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, माप(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = शून्य;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		अगर(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (अक्षर *)execi.ढेर_ऊपरी;
		sp -= माप(काष्ठा ps_strings);
		psp = (काष्ठा ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (माप(व्योम *) + माप(व्योम *) + माप(पूर्णांक));

		// linear address space, so it is available.
		psp->ps_argvstr = (अक्षर **)(sp + माप(पूर्णांक));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + माप(व्योम *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ढेर_ऊपरी - माप(काष्ठा ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
