#ضمن <minix/cpufeature.h>

#ضمن <minix/type.h>
#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

#ضمن <string.h>
#ضمن <minix/type.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_بنسټ, kern_phys_بنسټ, kern_اندازه;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بایټونه kern_vir_پیل = (phys_بایټونه) &kern_vir_بنسټ;
ساكن phys_بایټونه kern_phys_پیل = (phys_بایټونه) &kern_phys_بنسټ;
ساكن phys_بایټونه kern_kernlen = (phys_بایټونه) &kern_اندازه;

/* page directory we can use to map things */
ساكن u32_t pagedir[4096]  __aligned(16384);

فراغ print_memmap(kinfo_t *cbi)
{
	صحيح m;
	أكد(cbi->mmap_اندازه < MAXMEMMAP);
	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		phys_بایټونه addr = cbi->memmap[m].mm_بنسټ_addr, endit = cbi->memmap[m].mm_بنسټ_addr + cbi->memmap[m].mm_اوږدتیا;
		چاپ_کړه("%08lx-%08lx ",addr, endit);
	}
	چاپ_کړه("\nsize %08lx\n", cbi->mmap_اندازه);
}

فراغ cut_memmap(kinfo_t *cbi, phys_بایټونه پیل, phys_بایټونه پای_ټکی)
{
	صحيح m;
	phys_بایټونه o;

	إذا((o=پیل % ARM_پاڼه_اندازه))
		پیل -= o;
	إذا((o=پای_ټکی % ARM_پاڼه_اندازه))
		پای_ټکی += ARM_پاڼه_اندازه - o;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		phys_بایټونه substart = پیل, subend = پای_ټکی;
		phys_بایټونه memaddr = cbi->memmap[m].mm_بنسټ_addr,
		    memend = cbi->memmap[m].mm_بنسټ_addr + cbi->memmap[m].mm_اوږدتیا;

		/* adjust cut range to be a subset of the free memory */
		إذا(substart < memaddr) substart = memaddr;
		إذا(subend > memend) subend = memend;
		إذا(substart >= subend) استمر;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_بنسټ_addr = cbi->memmap[m].mm_اوږدتیا = 0;
		إذا(substart > memaddr)
			جمع_کړه_memmap(cbi, memaddr, substart-memaddr);
		إذا(subend < memend)
			جمع_کړه_memmap(cbi, subend, memend-subend);
	}
}

فراغ جمع_کړه_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحيح m;
#تعریف_کړه حد 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	إذا(addr > حد) {
		أرجع;
	}

	إذا(addr + len > حد) {
		len -= (addr + len - حد);
	}

	أكد(cbi->mmap_اندازه < MAXMEMMAP);
	إذا(len == 0) {
		أرجع;
	}
	addr = roundup(addr, ARM_پاڼه_اندازه);
	len = rounddown(len, ARM_پاڼه_اندازه);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بایټونه highmark;
		إذا(cbi->memmap[m].mm_اوږدتیا) {
			استمر;
		}
		cbi->memmap[m].mm_بنسټ_addr = addr;
		cbi->memmap[m].mm_اوږدتیا = len;
		cbi->memmap[m].type = MULTIBOOT_حافظه_شته;
		إذا(m >= cbi->mmap_اندازه) {
			cbi->mmap_اندازه = m+1;
		}
		highmark = addr + len;
		إذا(highmark > cbi->mem_لوړ_phys) {
			cbi->mem_لوړ_phys = highmark;
		}
		أرجع;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بایټونه *ph)
{
	u32_t *ret;
#تعریف_کړه PG_PAGETABLES 24
	ساكن u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	أكد(حجم(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#تعریف_کړه پاڼه_KB (ARM_پاڼه_اندازه / 1024)

phys_بایټونه pg_alloc_پاڼه(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظه_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_اوږدتیا) {
			استمر;
		}
		أكد(mmap->mm_اوږدتیا > 0);
		أكد(!(mmap->mm_اوږدتیا % ARM_پاڼه_اندازه));
		أكد(!(mmap->mm_بنسټ_addr % ARM_پاڼه_اندازه));

		u32_t addr = mmap->mm_بنسټ_addr;
		mmap->mm_بنسټ_addr += ARM_پاڼه_اندازه;
		mmap->mm_اوږدتیا  -= ARM_پاڼه_اندازه;

		cbi->kernel_allocated_بایټونه_dynamic += ARM_پاڼه_اندازه;

		أرجع addr;
	}

	panic("can't find free memory");
}

فراغ pg_بې_اغېزه_عنصر(kinfo_t *cbi)
{
	uint32_t i;
	phys_بایټونه phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_لوړ_phys);

        /* Set up an identity mapping page directory */
	 لكل(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_اندازه;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		إذا (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_پای_ټکی) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} وإلا {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_پیل;

	أكد(!(kern_vir_پیل % ARM_SECTION_اندازه));
	أكد(!(kern_phys_پیل % ARM_SECTION_اندازه));
	pde = kern_vir_پیل / ARM_SECTION_اندازه; /* start pde */
	طالما(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_ماسک) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_اندازه;
		kern_phys += ARM_SECTION_اندازه;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
{
	u32_t sctlr;
	u32_t actlr;

	write_ttbcr(0);

	/* Set all Domains to Client */
	write_dacr(0x55555555);

	sctlr = read_sctlr();

	/* Enable MMU */
	sctlr |= CPU_CONTROL_MMU_ENABLE;

	/* TRE set to zero (default reset value): TEX[2:0] are used, plus C and B bits.*/
	sctlr &= ~CPU_CONTROL_TR_ENABLE;

	/* AFE set to zero (default reset value): not using simplified model. */
	sctlr &= ~CPU_CONTROL_AF_ENABLE;

	/* Enable instruction ,data cache and branch prediction */
	sctlr |= CPU_CONTROL_DC_ENABLE;
	sctlr |= CPU_CONTROL_IC_ENABLE;
	sctlr |= CPU_CONTROL_BPRD_ENABLE;

	/* Enable barriers */
	sctlr |= CPU_CONTROL_32BD_ENABLE;

	/* Enable L2 cache (cortex-a8) */
	#تعریف_کړه CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_بایټونه pg_بارول(فراغ)
{
	phys_بایټونه phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بایټونه pg_rounddown(phys_بایټونه b)
{
	phys_بایټونه o;
	إذا(!(o = b % ARM_پاڼه_اندازه)) {
		أرجع b;
	}
	أرجع b  - o;
}

فراغ pg_map(phys_بایټونه phys, vir_بایټونه vaddr, vir_بایټونه vaddr_پای_ټکی,
	kinfo_t *cbi)
{
	ساكن صحيح mapped_pde = -1;
	ساكن u32_t *pt = عدم;
	صحيح pde, pte;

	أكد(kernel_may_alloc);

	إذا(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % ARM_پاڼه_اندازه));
	} وإلا  {
		أكد((vaddr % ARM_پاڼه_اندازه) == (phys % ARM_پاڼه_اندازه));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_پیل);

	طالما(vaddr < vaddr_پای_ټکی) {
		phys_بایټونه سرچینه = phys;
		أكد(!(vaddr % ARM_پاڼه_اندازه));
		إذا(phys == PG_ALLOCATEME) {
			سرچینه = pg_alloc_پاڼه(cbi);
		} وإلا {
			أكد(!(phys % ARM_پاڼه_اندازه));
		}
		أكد(!(سرچینه % ARM_پاڼه_اندازه));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بایټونه ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_ماسک)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (سرچینه & ARM_VM_PTE_ماسک)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_پاڼه_اندازه;
		إذا(phys != PG_ALLOCATEME) {
			phys += ARM_پاڼه_اندازه;
		}
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
