/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#ضمن <assert.h>
#ضمن <string.h>

#ضمن <machine/multiboot.h>

#ضمن "kernel/kernel.h"

#ضمن "archconst.h"
#ضمن "arch_proto.h"

#ضمن <sys/exec.h>
#ضمن <libexec.h>

بنية tss_s tss[CONFIG_MAX_CPUS];
خارجي صحيح exc_سمتیہ_جدول;

صحيح prot_init_پورا = 0;

phys_بائٹاں vir2phys(فراغ *vir)
{
	/* defined in kernel.lds */
	خارجي حرف kern_vir_ادھار, kern_phys_ادھار;
	u32_t وتھ = (vir_بائٹاں) &kern_vir_ادھار -
	    (vir_بائٹاں) &kern_phys_ادھار;
	أرجع (phys_بائٹاں)vir - وتھ;
}

صحيح tss_init(بنانشانی cpu, فراغ * kernel_ڈھیر)
{

	بنية tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((بنانشانی) kernel_ڈھیر) - ARM_ڈھیر_چوٹی_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * حجم(reg_t))) = cpu;

	أرجع 0;
}

multiboot_module_t *bootmod(صحيح pnr)
{
	صحيح i;

	أكد(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	لكل(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		صحيح p;
		p = i - NR_TASKS;
		إذا(image[i].proc_nr == pnr) {
			أكد(p < MULTIBOOT_MAX_MODS);
			أكد(p < kinfo.mbi.mi_mods_گنتی);
			أرجع &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

صحيح booting_cpu = 0;

فراغ prot_init(فراغ)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_سمتیہ_جدول);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_پچھان_تت(&kinfo); /* Still need 1:1 for device memory . */
	pg_mapkernel();
	pg_لدو();

	prot_init_پورا = 1;
}

ساكن صحيح alloc_for_vm = 0;

فراغ arch_post_init(فراغ)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	بنية proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

ساكن صحيح libexec_pg_alloc(بنية exec_info *execi, vir_بائٹاں vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_لدو();
	memset((حرف *) vaddr, 0, len);
	alloc_for_vm += len;
	أرجع OK;
}

فراغ arch_boot_proc(بنية boot_image *ip, بنية proc *rp)
{
	multiboot_module_t *mod;
	بنية ps_لڑیاں *psp;
	حرف *sp;

	إذا(rp->p_nr < 0) أرجع;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	إذا(rp->p_nr == VM_PROC_NR) {
		بنية exec_info execi;

		memset(&execi, 0, حجم(execi));

		/* exec parameters */
		execi.ڈھیر_بلند = kinfo.user_sp;
		execi.ڈھیر_ناپ = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (حرف *) mod->mod_شروع; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_سرا - mod->mod_شروع;
		strlcpy(execi.progname, ip->proc_name, حجم(execi.progname));
		execi.چوکھٹا_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = عدم;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		إذا(libexec_لدو_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (حرف *)execi.ڈھیر_بلند;
		sp -= حجم(بنية ps_لڑیاں);
		psp = (بنية ps_لڑیاں *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (حجم(فراغ *) + حجم(فراغ *) + حجم(صحيح));

		// linear address space, so it is available.
		psp->ps_argvstr = (حرف **)(sp + حجم(صحيح));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + حجم(فراغ *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_بائٹاں)sp,
			execi.ڈھیر_بلند - حجم(بنية ps_لڑیاں),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		جوڑو_memmap(&kinfo, mod->mod_شروع, mod->mod_سرا-mod->mod_شروع);
		mod->mod_سرا = mod->mod_شروع = 0;

		/* Remember them */
		kinfo.vm_allocated_بائٹاں = alloc_for_vm;
	}
}
