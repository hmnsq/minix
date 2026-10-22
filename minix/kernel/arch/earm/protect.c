/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ <machine/multiboot.h>

#அந்தர்பாவ "kernel/kernel.h"

#அந்தர்பாவ "archconst.h"
#அந்தர்பாவ "arch_proto.h"

#அந்தர்பாவ <sys/exec.h>
#அந்தர்பாவ <libexec.h>

கட்டமைப்பு tss_s tss[CONFIG_MAX_CPUS];
வெளி முழு exc_vector_table;

முழு prot_init_done = 0;

phys_bytes vir2phys(வெறுமை *vir)
{
	/* defined in kernel.lds */
	வெளி எழுத்து _kern_vir_base, _kern_phys_base;
	u32_t offset = (vir_bytes) &_kern_vir_base -
	    (vir_bytes) &_kern_phys_base;
	திரும்பு (phys_bytes)vir - offset;
}

முழு tss_init(unsigned cpu, வெறுமை * kernel_அடுக்கு)
{

	கட்டமைப்பு tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((unsigned) kernel_அடுக்கு) - ARM_அடுக்கு_உச்சி_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * அளவு(reg_t))) = cpu;

	திரும்பு 0;
}

multiboot_module_t *bootmod(முழு pnr)
{
	முழு i;

	ப்ரதிஜ்ஞா(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	க்கு(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		முழு p;
		p = i - NR_TASKS;
		எனில்(image[i].proc_nr == pnr) {
			ப்ரதிஜ்ஞா(p < MULTIBOOT_MAX_MODS);
			ப்ரதிஜ்ஞா(p < kinfo.mbi.mi_mods_மொத்தம்);
			திரும்பு &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

முழு booting_cpu = 0;

வெறுமை prot_init(வெறுமை)
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

நிலையான முழு alloc_for_vm = 0;

வெறுமை arch_post_init(வெறுமை)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	கட்டமைப்பு proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

நிலையான முழு libexec_pg_alloc(கட்டமைப்பு exec_info *execi, vir_bytes vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_load();
	memset((எழுத்து *) vaddr, 0, len);
	alloc_for_vm += len;
	திரும்பு OK;
}

வெறுமை arch_boot_proc(கட்டமைப்பு boot_image *ip, கட்டமைப்பு proc *rp)
{
	multiboot_module_t *mod;
	கட்டமைப்பு ps_strings *psp;
	எழுத்து *sp;

	எனில்(rp->p_nr < 0) திரும்பு;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	எனில்(rp->p_nr == VM_PROC_NR) {
		கட்டமைப்பு exec_info execi;

		memset(&execi, 0, அளவு(execi));

		/* exec parameters */
		execi.அடுக்கு_உயர்வு = kinfo.user_sp;
		execi.அடுக்கு_size = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (எழுத்து *) mod->mod_start; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_end - mod->mod_start;
		strlcpy(execi.progname, ip->proc_name, அளவு(execi.progname));
		execi.frame_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = வெற்று;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		எனில்(libexec_load_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (எழுத்து *)execi.அடுக்கு_உயர்வு;
		sp -= அளவு(கட்டமைப்பு ps_strings);
		psp = (கட்டமைப்பு ps_strings *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (அளவு(வெறுமை *) + அளவு(வெறுமை *) + அளவு(முழு));

		// linear address space, so it is available.
		psp->ps_argvstr = (எழுத்து **)(sp + அளவு(முழு));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + அளவு(வெறுமை *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_bytes)sp,
			execi.அடுக்கு_உயர்வு - அளவு(கட்டமைப்பு ps_strings),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		add_memmap(&kinfo, mod->mod_start, mod->mod_end-mod->mod_start);
		mod->mod_end = mod->mod_start = 0;

		/* Remember them */
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
