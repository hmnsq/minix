/* This file contains code for initialization of protected mode, to initialize
 * code and data segment descriptors, and to initialize global descriptors
 * for local descriptors in the process table.
 */

#شامل <assert.h>
#شامل <string.h>

#شامل <machine/multiboot.h>

#شامل "kernel/kernel.h"

#شامل "archconst.h"
#شامل "arch_proto.h"

#شامل <sys/exec.h>
#شامل <libexec.h>

ساخت tss_s tss[CONFIG_MAX_CPUS];
خارجی صحیح exc_سمتیہ_جدول;

صحیح prot_init_مکمل = 0;

phys_بائٹیں vir2phys(خالی *vir)
{
	/* defined in kernel.lds */
	خارجی حرف kern_vir_بنیاد, kern_phys_بنیاد;
	u32_t انحراف = (vir_بائٹیں) &kern_vir_بنیاد -
	    (vir_بائٹیں) &kern_phys_بنیاد;
	واپس (phys_بائٹیں)vir - انحراف;
}

صحیح tss_init(غیرعلامتی cpu, خالی * kernel_ڈھیر)
{

	ساخت tss_s * t = &tss[cpu];

	/*
	 * make space for process pointer and cpu id and point to the first
	 * usable word
	 */
	t->sp0 = ((غیرعلامتی) kernel_ڈھیر) - ARM_ڈھیر_چوٹی_RESERVED;
	/*
	 * set the cpu id at the top of the stack so we know on which cpu is
	 * this stack in use when we trap to kernel
	 */
	*((reg_t *)(t->sp0 + 1 * حجم(reg_t))) = cpu;

	واپس 0;
}

multiboot_module_t *bootmod(صحیح pnr)
{
	صحیح i;

	أكد(pnr >= 0);

	/* Search for desired process in boot process
	 * list. The first NR_TASKS ones do not correspond
	 * to a module, however, so we don't search those.
	 */
	برائے(i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		صحیح p;
		p = i - NR_TASKS;
		اگر(image[i].proc_nr == pnr) {
			أكد(p < MULTIBOOT_MAX_MODS);
			أكد(p < kinfo.mbi.mi_mods_گنتی);
			واپس &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

صحیح booting_cpu = 0;

خالی prot_init(خالی)
{
	/* tell the HW where we stored our vector table */
	write_vbar((reg_t)&exc_سمتیہ_جدول);

	/* Set up a new post-relocate bootstrap pagetable so that
	 * we can map in VM, and we no longer rely on pre-relocated
	 * data.
	 */

	pg_clear();
	pg_شناختی_عنصر(&kinfo); /* Still need 1:1 for device memory . */
	pg_mapkernel();
	pg_بھرو();

	prot_init_مکمل = 1;
}

ساکن صحیح alloc_for_vm = 0;

خالی arch_post_init(خالی)
{
	/* Let memory mapping code know what's going on at bootstrap time */
	ساخت proc *vm;
	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
	pg_info(&vm->p_seg.p_ttbr, &vm->p_seg.p_ttbr_v);
}

ساکن صحیح libexec_pg_alloc(ساخت exec_info *execi, vir_بائٹیں vaddr, size_t len)
{
	pg_map(PG_ALLOCATEME, vaddr, vaddr+len, &kinfo);
	pg_بھرو();
	memset((حرف *) vaddr, 0, len);
	alloc_for_vm += len;
	واپس OK;
}

خالی arch_boot_proc(ساخت boot_image *ip, ساخت proc *rp)
{
	multiboot_module_t *mod;
	ساخت ps_لڑیاں *psp;
	حرف *sp;

	اگر(rp->p_nr < 0) واپس;

	mod = bootmod(rp->p_nr);

	/* Important special case: we put VM in the bootstrap pagetable
	 * so it can run.
	 */

	اگر(rp->p_nr == VM_PROC_NR) {
		ساخت exec_info execi;

		memset(&execi, 0, حجم(execi));

		/* exec parameters */
		execi.ڈھیر_بلند = kinfo.user_sp;
		execi.ڈھیر_سائز = 64 * 1024;	/* not too crazy as it must be preallocated */
		execi.proc_e = ip->endpoint;
		execi.hdr = (حرف *) mod->mod_آغاز; /* phys mem direct */
		execi.filesize = execi.hdr_len = mod->mod_انتہا - mod->mod_آغاز;
		strlcpy(execi.progname, ip->proc_name, حجم(execi.progname));
		execi.چوکھٹا_len = 0;

		/* callbacks for use in the kernel */
		execi.copymem = libexec_copy_memcpy;
		execi.clearmem = libexec_clear_memset;
		execi.allocmem_prealloc_junk = libexec_pg_alloc;
		execi.allocmem_prealloc_cleared = libexec_pg_alloc;
		execi.allocmem_ondemand = libexec_pg_alloc;
		execi.clearproc = تہی;

		/* parse VM ELF binary and alloc/map it into bootstrap pagetable */
		اگر(libexec_بھرو_elf(&execi) != OK)
			panic("VM loading failed");

		/* Setup a ps_strings struct on the stack, pointing to the
		 * following argv, envp. */
		sp = (حرف *)execi.ڈھیر_بلند;
		sp -= حجم(ساخت ps_لڑیاں);
		psp = (ساخت ps_لڑیاں *) sp;

		/* Take the stack pointer down three words to give startup code
		 * something to use as "argc", "argv" and "envp".
		 */
		sp -= (حجم(خالی *) + حجم(خالی *) + حجم(صحیح));

		// linear address space, so it is available.
		psp->ps_argvstr = (حرف **)(sp + حجم(صحیح));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + حجم(خالی *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, execi.pc, (vir_بائٹیں)sp,
			execi.ڈھیر_بلند - حجم(ساخت ps_لڑیاں),
			ip->proc_name);

		/* Free VM blob that was just copied into existence. */
		جوڑو_memmap(&kinfo, mod->mod_آغاز, mod->mod_انتہا-mod->mod_آغاز);
		mod->mod_انتہا = mod->mod_آغاز = 0;

		/* Remember them */
		kinfo.vm_allocated_بائٹیں = alloc_for_vm;
	}
}
