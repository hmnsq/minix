/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ <machine/multiboot.h>

#అన్తర్భావ "kernel/kernel.h"

#అన్తర్భావ "archconst.h"
#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ <sys/exec.h>
#అన్తర్భావ <libexec.h>

నిర్మాణం tss_s tss[CONFIG_MAX_CPUS];
బాహ్య పూర్ణాంకం exc_సదిశ_పట్టిక;

పూర్ణాంకం prot_init_పూర్తి = 0;

phys_బైట్లు vir2phys(శూన్య *vir)
{
	/* defined in kernel.lds */
	బాహ్య అక్షరం kern_vir_పునాది, kern_phys_పునాది;
	u32_t స్థానభ్రంశం = (vir_బైట్లు) &kern_vir_పునాది -
	    (vir_బైట్లు) &kern_phys_పునాది;
	తిరిగి (phys_బైట్లు)vir - స్థానభ్రంశం;
}

పూర్ణాంకం tss_init(గుర్తులేని cpu, శూన్య * kernel_దొంతర)
{

	నిర్మాణం tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((గుర్తులేని) kernel_దొంతర) - ARM_దొంతర_శిఖరం_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * పరిమాణం(reg_t))) = cpu;

	తిరిగి 0;
}

multiboot_module_t *bootmod(పూర్ణాంకం pnr)
{
	పూర్ణాంకం i;

	ప్రతిజ్ఞా(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	కొరకు(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		పూర్ణాంకం p;
		p = i - NR_TASKS;
		ఒకవేళ(image[i].proc_nr == pnr) {
			ప్రతిజ్ఞా(p < MULTIBOOT_MAX_MODS);
			ప్రతిజ్ఞా(p < kinfo.mbi.mi_mods_లెక్క);
			తిరిగి &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

పూర్ణాంకం booting_cpu = 0;

శూన్య prot_init(శూన్య)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_సదిశ_పట్టిక);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_తటస్థం(&kinfo); /* Still need 1:1 for device memory . */
	pg_mapkernel();
	pg_ఎక్కించు();

	prot_init_పూర్తి = 1;
}

స్థిర పూర్ణాంకం alloc_for_vm = 0;

శూన్య arch_post_init(శూన్య)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	నిర్మాణం proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

స్థిర పూర్ణాంకం libexec_pg_alloc(నిర్మాణం exec_info *execi, vir_బైట్లు vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_ఎక్కించు();
	memset((అక్షరం *) vaddr, 0, len);
	alloc_for_vm += len;
	తిరిగి OK;
}

శూన్య arch_boot_proc(నిర్మాణం boot_image *ip, నిర్మాణం proc *rp)
{
	multiboot_module_t *mod;
	నిర్మాణం ps_అక్షరశ్రేణులు *psp;
	అక్షరం *sp;

	ఒకవేళ(rp->p_nr < 0) తిరిగి;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	ఒకవేళ(rp->p_nr == VM_PROC_NR) {
		నిర్మాణం exec_info execi;

		memset(&execi, 0, పరిమాణం(execi));

		/* exec parameters */
		execi.దొంతర_ఎగువ = kinfo.user_sp;
		execi.దొంతర_కొలత = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (అక్షరం *) mod->mod_ప్రారంభం; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_చివర - mod->mod_ప్రారంభం;
		strlcpy(execi.progname, ip->proc_name, పరిమాణం(execi.progname));
		execi.చట్రం_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = ఖాళీ;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		ఒకవేళ(libexec_ఎక్కించు_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (అక్షరం *)execi.దొంతర_ఎగువ;
		sp -= పరిమాణం(నిర్మాణం ps_అక్షరశ్రేణులు);
		psp = (నిర్మాణం ps_అక్షరశ్రేణులు *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (పరిమాణం(శూన్య *) + పరిమాణం(శూన్య *) + పరిమాణం(పూర్ణాంకం));

		// linear address space, so it is available.
		psp->ps_argvstr = (అక్షరం **)(sp + పరిమాణం(పూర్ణాంకం));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + పరిమాణం(శూన్య *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_బైట్లు)sp,
			execi.దొంతర_ఎగువ - పరిమాణం(నిర్మాణం ps_అక్షరశ్రేణులు),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		కూడు_memmap(&kinfo, mod->mod_ప్రారంభం, mod->mod_చివర-mod->mod_ప్రారంభం);
		mod->mod_చివర = mod->mod_ప్రారంభం = 0;

		/* Remember them */
		kinfo.vm_allocated_బైట్లు = alloc_for_vm;
	}
}
