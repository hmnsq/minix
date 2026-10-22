#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arm/armreg.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>

/* These are set/computed in kernel.lds. */
ਬਾਹਰੀ ਅੱਖਰ kern_vir_ਅਧਾਰ, kern_phys_ਅਧਾਰ, kern_ਮਾਪ;

/* Retrieve the absolute values to something we can use. */
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_vir_ਸ਼ੁਰੂ = (phys_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_phys_ਸ਼ੁਰੂ = (phys_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ;
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_kernlen = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;

/* page directory we can use to map things */
ਸਥਿਰ u32_t pagedir[4096]  __aligned(16384);

ਖਾਲੀ print_memmap(kinfo_t *cbi)
{
	ਪੂਰਨ m;
	ਪ੍ਰਤਿਜ੍ਞਾ(cbi->mmap_ਮਾਪ < MAXMEMMAP);
	ਲਈ(m = 0; m < cbi->mmap_ਮਾਪ; m++) {
		phys_ਬਾਈਟਾਂ addr = cbi->memmap[m].mm_ਅਧਾਰ_addr, endit = cbi->memmap[m].mm_ਅਧਾਰ_addr + cbi->memmap[m].mm_ਲੰਮਾਈ;
		ਲਿਖੋ("%08lx-%08lx ",addr, endit);
	}
	ਲਿਖੋ("\nsize %08lx\n", cbi->mmap_ਮਾਪ);
}

ਖਾਲੀ cut_memmap(kinfo_t *cbi, phys_ਬਾਈਟਾਂ ਸ਼ੁਰੂ, phys_ਬਾਈਟਾਂ ਸਿਰਾ)
{
	ਪੂਰਨ m;
	phys_ਬਾਈਟਾਂ o;

	ਜੇ((o=ਸ਼ੁਰੂ % ARM_ਪੰਨਾ_ਮਾਪ))
		ਸ਼ੁਰੂ -= o;
	ਜੇ((o=ਸਿਰਾ % ARM_ਪੰਨਾ_ਮਾਪ))
		ਸਿਰਾ += ARM_ਪੰਨਾ_ਮਾਪ - o;

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

	ਲਈ(m = 0; m < cbi->mmap_ਮਾਪ; m++) {
		phys_ਬਾਈਟਾਂ substart = ਸ਼ੁਰੂ, subend = ਸਿਰਾ;
		phys_ਬਾਈਟਾਂ memaddr = cbi->memmap[m].mm_ਅਧਾਰ_addr,
		    memend = cbi->memmap[m].mm_ਅਧਾਰ_addr + cbi->memmap[m].mm_ਲੰਮਾਈ;

		/* adjust cut range to be a subset of the free memory */
		ਜੇ(substart < memaddr) substart = memaddr;
		ਜੇ(subend > memend) subend = memend;
		ਜੇ(substart >= subend) ਜਾਰੀ;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_ਅਧਾਰ_addr = cbi->memmap[m].mm_ਲੰਮਾਈ = 0;
		ਜੇ(substart > memaddr)
			ਜੋੜੋ_memmap(cbi, memaddr, substart-memaddr);
		ਜੇ(subend < memend)
			ਜੋੜੋ_memmap(cbi, subend, memend-subend);
	}
}

ਖਾਲੀ ਜੋੜੋ_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	ਪੂਰਨ m;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ਜੇ(addr > ਹੱਦਬੰਦੀ) {
		ਵਾਪਸ;
	}

	ਜੇ(addr + len > ਹੱਦਬੰਦੀ) {
		len -= (addr + len - ਹੱਦਬੰਦੀ);
	}

	ਪ੍ਰਤਿਜ੍ਞਾ(cbi->mmap_ਮਾਪ < MAXMEMMAP);
	ਜੇ(len == 0) {
		ਵਾਪਸ;
	}
	addr = roundup(addr, ARM_ਪੰਨਾ_ਮਾਪ);
	len = rounddown(len, ARM_ਪੰਨਾ_ਮਾਪ);

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

        ਲਈ(m = 0; m < MAXMEMMAP; m++) {
		phys_ਬਾਈਟਾਂ highmark;
		ਜੇ(cbi->memmap[m].mm_ਲੰਮਾਈ) {
			ਜਾਰੀ;
		}
		cbi->memmap[m].mm_ਅਧਾਰ_addr = addr;
		cbi->memmap[m].mm_ਲੰਮਾਈ = len;
		cbi->memmap[m].type = MULTIBOOT_ਸਮ੍ਰਿਤੀ_ਉਪਲਬਧ;
		ਜੇ(m >= cbi->mmap_ਮਾਪ) {
			cbi->mmap_ਮਾਪ = m+1;
		}
		highmark = addr + len;
		ਜੇ(highmark > cbi->mem_ਉੱਚਾ_phys) {
			cbi->mem_ਉੱਚਾ_phys = highmark;
		}
		ਵਾਪਸ;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_ਬਾਈਟਾਂ *ph)
{
	u32_t *ret;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PG_PAGETABLES 24
	ਸਥਿਰ u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ਸਥਿਰ ਪੂਰਨ pt_inuse = 0;
	ਜੇ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ਵਾਪਸ ret;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_KB (ARM_ਪੰਨਾ_ਮਾਪ / 1024)

phys_ਬਾਈਟਾਂ pg_alloc_ਪੰਨਾ(kinfo_t *cbi)
{
	ਪੂਰਨ m;
	multiboot_ਸਮ੍ਰਿਤੀ_map_t *mmap;

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

	ਲਈ(m = 0; m < cbi->mmap_ਮਾਪ; m++) {
		mmap = &cbi->memmap[m];
		ਜੇ(!mmap->mm_ਲੰਮਾਈ) {
			ਜਾਰੀ;
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(mmap->mm_ਲੰਮਾਈ > 0);
		ਪ੍ਰਤਿਜ੍ਞਾ(!(mmap->mm_ਲੰਮਾਈ % ARM_ਪੰਨਾ_ਮਾਪ));
		ਪ੍ਰਤਿਜ੍ਞਾ(!(mmap->mm_ਅਧਾਰ_addr % ARM_ਪੰਨਾ_ਮਾਪ));

		u32_t addr = mmap->mm_ਅਧਾਰ_addr;
		mmap->mm_ਅਧਾਰ_addr += ARM_ਪੰਨਾ_ਮਾਪ;
		mmap->mm_ਲੰਮਾਈ  -= ARM_ਪੰਨਾ_ਮਾਪ;

		cbi->kernel_allocated_ਬਾਈਟਾਂ_dynamic += ARM_ਪੰਨਾ_ਮਾਪ;

		ਵਾਪਸ addr;
	}

	panic("can't find free memory");
}

ਖਾਲੀ pg_ਪਛਾਣ_ਤੱਤ(kinfo_t *cbi)
{
	uint32_t i;
	phys_ਬਾਈਟਾਂ phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(cbi->mem_ਉੱਚਾ_phys);

        /* Set up an identity mapping page directory */
	 ਲਈ(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_ਮਾਪ;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ਜੇ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_ਸਿਰਾ) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ਨਹੀਂਤਾਂ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

ਪੂਰਨ pg_mapkernel(ਖਾਲੀ)
{
	ਪੂਰਨ pde;
	u32_t mapped = 0, kern_phys = kern_phys_ਸ਼ੁਰੂ;

	ਪ੍ਰਤਿਜ੍ਞਾ(!(kern_vir_ਸ਼ੁਰੂ % ARM_SECTION_ਮਾਪ));
	ਪ੍ਰਤਿਜ੍ਞਾ(!(kern_phys_ਸ਼ੁਰੂ % ARM_SECTION_ਮਾਪ));
	pde = kern_vir_ਸ਼ੁਰੂ / ARM_SECTION_ਮਾਪ; /* start pde */
	ਜਦੋਂ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_ਨਕਾਬ) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_ਮਾਪ;
		kern_phys += ARM_SECTION_ਮਾਪ;
		pde++;
	}
	ਵਾਪਸ pde;	/* free pde */
}

ਖਾਲੀ vm_enable_paging(ਖਾਲੀ)
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
	#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_ਬਾਈਟਾਂ pg_ਲੱਦੋ(ਖਾਲੀ)
{
	phys_ਬਾਈਟਾਂ phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ਵਾਪਸ phpagedir;
}

ਖਾਲੀ pg_clear(ਖਾਲੀ)
{
	memset(pagedir, 0, ਆਕਾਰ(pagedir));
}

phys_ਬਾਈਟਾਂ pg_rounddown(phys_ਬਾਈਟਾਂ b)
{
	phys_ਬਾਈਟਾਂ o;
	ਜੇ(!(o = b % ARM_ਪੰਨਾ_ਮਾਪ)) {
		ਵਾਪਸ b;
	}
	ਵਾਪਸ b  - o;
}

ਖਾਲੀ pg_map(phys_ਬਾਈਟਾਂ phys, vir_ਬਾਈਟਾਂ vaddr, vir_ਬਾਈਟਾਂ vaddr_ਸਿਰਾ,
	kinfo_t *cbi)
{
	ਸਥਿਰ ਪੂਰਨ mapped_pde = -1;
	ਸਥਿਰ u32_t *pt = ਨਿਰਾ;
	ਪੂਰਨ pde, pte;

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

	ਜੇ(phys == PG_ALLOCATEME) {
		ਪ੍ਰਤਿਜ੍ਞਾ(!(vaddr % ARM_ਪੰਨਾ_ਮਾਪ));
	} ਨਹੀਂਤਾਂ  {
		ਪ੍ਰਤਿਜ੍ਞਾ((vaddr % ARM_ਪੰਨਾ_ਮਾਪ) == (phys % ARM_ਪੰਨਾ_ਮਾਪ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(vaddr < kern_vir_ਸ਼ੁਰੂ);

	ਜਦੋਂ(vaddr < vaddr_ਸਿਰਾ) {
		phys_ਬਾਈਟਾਂ ਸਰੋਤ = phys;
		ਪ੍ਰਤਿਜ੍ਞਾ(!(vaddr % ARM_ਪੰਨਾ_ਮਾਪ));
		ਜੇ(phys == PG_ALLOCATEME) {
			ਸਰੋਤ = pg_alloc_ਪੰਨਾ(cbi);
		} ਨਹੀਂਤਾਂ {
			ਪ੍ਰਤਿਜ੍ਞਾ(!(phys % ARM_ਪੰਨਾ_ਮਾਪ));
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(!(ਸਰੋਤ % ARM_ਪੰਨਾ_ਮਾਪ));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ਜੇ(mapped_pde < pde) {
			phys_ਬਾਈਟਾਂ ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_ਨਕਾਬ)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(pt);
		pt[pte] = (ਸਰੋਤ & ARM_VM_PTE_ਨਕਾਬ)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_ਪੰਨਾ_ਮਾਪ;
		ਜੇ(phys != PG_ALLOCATEME) {
			phys += ARM_ਪੰਨਾ_ਮਾਪ;
		}
	}
}

ਖਾਲੀ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
