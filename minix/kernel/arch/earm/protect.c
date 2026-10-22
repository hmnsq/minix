/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ <machine/multiboot.h>

#અન્તર્ભાવ "kernel/kernel.h"

#અન્તર્ભાવ "archconst.h"
#અન્તર્ભાવ "arch_proto.h"

#અન્તર્ભાવ <sys/exec.h>
#અન્તર્ભાવ <libexec.h>

સંરચના tss_s tss[CONFIG_MAX_CPUS];
બાહ્ય પૂર્ણાંક exc_vector_table;

પૂર્ણાંક prot_init_done = 0;

phys_bytes vir2phys(શૂન્ય *vir)
{
	/* defined in kernel.lds */
	બાહ્ય અક્ષર _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	પાછા (phys_bytes)vir - offset;
}

પૂર્ણાંક tss_init(unsigned cpu, શૂન્ય * kernel_ઢગલો)
{

	સંરચના tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_ઢગલો) - ARM_ઢગલો_ટોચ_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * કદ(reg_t))) = cpu;

	પાછા 0;
}

multiboot_module_t *bootmod(પૂર્ણાંક pnr)
{
	પૂર્ણાંક i;

	પ્રતિજ્ઞા(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	માટે(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		પૂર્ણાંક p;
		p = i - NR_TASKS;
		જો(image[i].proc_nr == pnr) {
			પ્રતિજ્ઞા(p < MULTIBOOT_MAX_MODS);
			પ્રતિજ્ઞા(p < kinfo.mbi.mi_mods_ગણતરી);
			પાછા &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

પૂર્ણાંક booting_cpu = 0;

શૂન્ય prot_init(શૂન્ય)
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

સ્થિર પૂર્ણાંક alloc_for_vm = 0;

શૂન્ય arch_post_init(શૂન્ય)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	સંરચના proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

સ્થિર પૂર્ણાંક libexec_pg_alloc(સંરચના exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((અક્ષર *) vaddr, 0, len);
	alloc_for_vm += len;
	પાછા OK;
}

શૂન્ય arch_boot_proc(સંરચના boot_image *ip, સંરચના proc *rp)
{
	multiboot_module_t *mod;
	સંરચના ps_strings *psp;
	અક્ષર *sp;

	જો(rp->p_nr < 0) પાછા;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	જો(rp->p_nr == VM_PROC_NR) {
		સંરચના exec_info execi;

		memset(&execi, 0, કદ(execi));

		/* exec parameters */
		execi.ઢગલો_ઉપલું = kinfo.user_sp;
		execi.ઢગલો_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (અક્ષર *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, કદ(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ખાલી;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		જો(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (અક્ષર *)execi.ઢગલો_ઉપલું;
		sp -= કદ(સંરચના ps_strings);
		psp = (સંરચના ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (કદ(શૂન્ય *) + કદ(શૂન્ય *) + કદ(પૂર્ણાંક));

		// linear address space, so it is available.
		psp->ps_argvstr = (અક્ષર **)(sp + કદ(પૂર્ણાંક));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + કદ(શૂન્ય *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.ઢગલો_ઉપલું - કદ(સંરચના ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
