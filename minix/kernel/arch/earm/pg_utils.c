#అన్తర్భావ <minix/cpufeature.h>

#అన్తర్భావ <minix/type.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <arm/armreg.h>

#అన్తర్భావ <string.h>
#అన్తర్భావ <minix/type.h>

/* These are set/computed in kernel.lds. */
బాహ్య అక్షరం kern_vir_పునాది, kern_phys_పునాది, kern_కొలత;

/* Retrieve the absolute values to something we can use. */
స్థిర phys_బైట్లు kern_vir_ప్రారంభం = (phys_బైట్లు) &kern_vir_పునాది;
స్థిర phys_బైట్లు kern_phys_ప్రారంభం = (phys_బైట్లు) &kern_phys_పునాది;
స్థిర phys_బైట్లు kern_kernlen = (phys_బైట్లు) &kern_కొలత;

/* page directory we can use to map things */
స్థిర u32_t pagedir[4096]  __aligned(16384);

శూన్య print_memmap(kinfo_t *cbi)
{
	పూర్ణాంకం m;
	ప్రతిజ్ఞా(cbi->mmap_కొలత < MAXMEMMAP);
	కొరకు(m = 0; m < cbi->mmap_కొలత; m++) {
		phys_బైట్లు addr = cbi->memmap[m].mm_పునాది_addr, endit = cbi->memmap[m].mm_పునాది_addr + cbi->memmap[m].mm_నిడివి;
		రాయి("%08lx-%08lx ",addr, endit);
	}
	రాయి("\nsize %08lx\n", cbi->mmap_కొలత);
}

శూన్య cut_memmap(kinfo_t *cbi, phys_బైట్లు ప్రారంభం, phys_బైట్లు చివర)
{
	పూర్ణాంకం m;
	phys_బైట్లు o;

	ఒకవేళ((o=ప్రారంభం % ARM_పుట_కొలత))
		ప్రారంభం -= o;
	ఒకవేళ((o=చివర % ARM_పుట_కొలత))
		చివర += ARM_పుట_కొలత - o;

	ప్రతిజ్ఞా(kernel_may_alloc);

	కొరకు(m = 0; m < cbi->mmap_కొలత; m++) {
		phys_బైట్లు substart = ప్రారంభం, subend = చివర;
		phys_బైట్లు memaddr = cbi->memmap[m].mm_పునాది_addr,
		    memend = cbi->memmap[m].mm_పునాది_addr + cbi->memmap[m].mm_నిడివి;

		/* adjust cut range to be a subset of the free memory */
		ఒకవేళ(substart < memaddr) substart = memaddr;
		ఒకవేళ(subend > memend) subend = memend;
		ఒకవేళ(substart >= subend) కొనసాగు;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_పునాది_addr = cbi->memmap[m].mm_నిడివి = 0;
		ఒకవేళ(substart > memaddr)
			కూడు_memmap(cbi, memaddr, substart-memaddr);
		ఒకవేళ(subend < memend)
			కూడు_memmap(cbi, subend, memend-subend);
	}
}

శూన్య కూడు_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	పూర్ణాంకం m;
#నిర్వచించు హద్దు 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ఒకవేళ(addr > హద్దు) {
		తిరిగి;
	}

	ఒకవేళ(addr + len > హద్దు) {
		len -= (addr + len - హద్దు);
	}

	ప్రతిజ్ఞా(cbi->mmap_కొలత < MAXMEMMAP);
	ఒకవేళ(len == 0) {
		తిరిగి;
	}
	addr = roundup(addr, ARM_పుట_కొలత);
	len = rounddown(len, ARM_పుట_కొలత);

	ప్రతిజ్ఞా(kernel_may_alloc);

        కొరకు(m = 0; m < MAXMEMMAP; m++) {
		phys_బైట్లు highmark;
		ఒకవేళ(cbi->memmap[m].mm_నిడివి) {
			కొనసాగు;
		}
		cbi->memmap[m].mm_పునాది_addr = addr;
		cbi->memmap[m].mm_నిడివి = len;
		cbi->memmap[m].type = MULTIBOOT_జ్ఞాపకం_అందుబాటు;
		ఒకవేళ(m >= cbi->mmap_కొలత) {
			cbi->mmap_కొలత = m+1;
		}
		highmark = addr + len;
		ఒకవేళ(highmark > cbi->mem_ఎగువ_phys) {
			cbi->mem_ఎగువ_phys = highmark;
		}
		తిరిగి;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_బైట్లు *ph)
{
	u32_t *ret;
#నిర్వచించు PG_PAGETABLES 24
	స్థిర u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	స్థిర పూర్ణాంకం pt_inuse = 0;
	ఒకవేళ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ప్రతిజ్ఞా(పరిమాణం(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	తిరిగి ret;
}

#నిర్వచించు పుట_KB (ARM_పుట_కొలత / 1024)

phys_బైట్లు pg_alloc_పుట(kinfo_t *cbi)
{
	పూర్ణాంకం m;
	multiboot_జ్ఞాపకం_map_t *mmap;

	ప్రతిజ్ఞా(kernel_may_alloc);

	కొరకు(m = 0; m < cbi->mmap_కొలత; m++) {
		mmap = &cbi->memmap[m];
		ఒకవేళ(!mmap->mm_నిడివి) {
			కొనసాగు;
		}
		ప్రతిజ్ఞా(mmap->mm_నిడివి > 0);
		ప్రతిజ్ఞా(!(mmap->mm_నిడివి % ARM_పుట_కొలత));
		ప్రతిజ్ఞా(!(mmap->mm_పునాది_addr % ARM_పుట_కొలత));

		u32_t addr = mmap->mm_పునాది_addr;
		mmap->mm_పునాది_addr += ARM_పుట_కొలత;
		mmap->mm_నిడివి  -= ARM_పుట_కొలత;

		cbi->kernel_allocated_బైట్లు_dynamic += ARM_పుట_కొలత;

		తిరిగి addr;
	}

	panic("can't find free memory");
}

శూన్య pg_తటస్థం(kinfo_t *cbi)
{
	uint32_t i;
	phys_బైట్లు phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ప్రతిజ్ఞా(cbi->mem_ఎగువ_phys);

        /* Set up an identity mapping page directory */
	 కొరకు(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_కొలత;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ఒకవేళ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_చివర) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} లేకపోతే {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

పూర్ణాంకం pg_mapkernel(శూన్య)
{
	పూర్ణాంకం pde;
	u32_t mapped = 0, kern_phys = kern_phys_ప్రారంభం;

	ప్రతిజ్ఞా(!(kern_vir_ప్రారంభం % ARM_SECTION_కొలత));
	ప్రతిజ్ఞా(!(kern_phys_ప్రారంభం % ARM_SECTION_కొలత));
	pde = kern_vir_ప్రారంభం / ARM_SECTION_కొలత; /* start pde */
	అయితే(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_ముసుగు) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_కొలత;
		kern_phys += ARM_SECTION_కొలత;
		pde++;
	}
	తిరిగి pde;	/* free pde */
}

శూన్య vm_enable_paging(శూన్య)
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
	#నిర్వచించు CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_బైట్లు pg_ఎక్కించు(శూన్య)
{
	phys_బైట్లు phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	తిరిగి phpagedir;
}

శూన్య pg_clear(శూన్య)
{
	memset(pagedir, 0, పరిమాణం(pagedir));
}

phys_బైట్లు pg_rounddown(phys_బైట్లు b)
{
	phys_బైట్లు o;
	ఒకవేళ(!(o = b % ARM_పుట_కొలత)) {
		తిరిగి b;
	}
	తిరిగి b  - o;
}

శూన్య pg_map(phys_బైట్లు phys, vir_బైట్లు vaddr, vir_బైట్లు vaddr_చివర,
	kinfo_t *cbi)
{
	స్థిర పూర్ణాంకం mapped_pde = -1;
	స్థిర u32_t *pt = ఖాళీ;
	పూర్ణాంకం pde, pte;

	ప్రతిజ్ఞా(kernel_may_alloc);

	ఒకవేళ(phys == PG_ALLOCATEME) {
		ప్రతిజ్ఞా(!(vaddr % ARM_పుట_కొలత));
	} లేకపోతే  {
		ప్రతిజ్ఞా((vaddr % ARM_పుట_కొలత) == (phys % ARM_పుట_కొలత));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ప్రతిజ్ఞా(vaddr < kern_vir_ప్రారంభం);

	అయితే(vaddr < vaddr_చివర) {
		phys_బైట్లు ఆధారం = phys;
		ప్రతిజ్ఞా(!(vaddr % ARM_పుట_కొలత));
		ఒకవేళ(phys == PG_ALLOCATEME) {
			ఆధారం = pg_alloc_పుట(cbi);
		} లేకపోతే {
			ప్రతిజ్ఞా(!(phys % ARM_పుట_కొలత));
		}
		ప్రతిజ్ఞా(!(ఆధారం % ARM_పుట_కొలత));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ఒకవేళ(mapped_pde < pde) {
			phys_బైట్లు ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_ముసుగు)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ప్రతిజ్ఞా(pt);
		pt[pte] = (ఆధారం & ARM_VM_PTE_ముసుగు)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_పుట_కొలత;
		ఒకవేళ(phys != PG_ALLOCATEME) {
			phys += ARM_పుట_కొలత;
		}
	}
}

శూన్య pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
