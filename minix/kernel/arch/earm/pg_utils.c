#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>

#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <arm/armreg.h>

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>

/* These are set/computed in kernel.lds. */
ಬಾಹ್ಯ ಅಕ್ಷರ kern_vir_ತಳ, kern_phys_ತಳ, kern_ಗಾತ್ರ;

/* Retrieve the absolute values to something we can use. */
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_vir_ಆರಂಭ = (phys_ಬೈಟುಗಳು) &kern_vir_ತಳ;
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_phys_ಆರಂಭ = (phys_ಬೈಟುಗಳು) &kern_phys_ತಳ;
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_kernlen = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;

/* page directory we can use to map things */
ಸ್ಥಿರ u32_t pagedir[4096]  __aligned(16384);

ಶೂನ್ಯ print_memmap(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಂಕ m;
	ಪ್ರತಿಜ್ಞಾ(cbi->mmap_ಗಾತ್ರ < MAXMEMMAP);
	ಗಾಗಿ(m = 0; m < cbi->mmap_ಗಾತ್ರ; m++) {
		phys_ಬೈಟುಗಳು addr = cbi->memmap[m].mm_ತಳ_addr, endit = cbi->memmap[m].mm_ತಳ_addr + cbi->memmap[m].mm_ಉದ್ದಳತೆ;
		ಬರೆ("%08lx-%08lx ",addr, endit);
	}
	ಬರೆ("\nsize %08lx\n", cbi->mmap_ಗಾತ್ರ);
}

ಶೂನ್ಯ cut_memmap(kinfo_t *cbi, phys_ಬೈಟುಗಳು ಆರಂಭ, phys_ಬೈಟುಗಳು ಕೊನೆ)
{
	ಪೂರ್ಣಾಂಕ m;
	phys_ಬೈಟುಗಳು o;

	ಆದರೆ((o=ಆರಂಭ % ARM_ಪುಟ_ಗಾತ್ರ))
		ಆರಂಭ -= o;
	ಆದರೆ((o=ಕೊನೆ % ARM_ಪುಟ_ಗಾತ್ರ))
		ಕೊನೆ += ARM_ಪುಟ_ಗಾತ್ರ - o;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಗಾಗಿ(m = 0; m < cbi->mmap_ಗಾತ್ರ; m++) {
		phys_ಬೈಟುಗಳು substart = ಆರಂಭ, subend = ಕೊನೆ;
		phys_ಬೈಟುಗಳು memaddr = cbi->memmap[m].mm_ತಳ_addr,
		    memend = cbi->memmap[m].mm_ತಳ_addr + cbi->memmap[m].mm_ಉದ್ದಳತೆ;

		/* adjust cut range to be a subset of the free memory */
		ಆದರೆ(substart < memaddr) substart = memaddr;
		ಆದರೆ(subend > memend) subend = memend;
		ಆದರೆ(substart >= subend) ಮುಂದುವರಿ;

		/* if there is any overlap, forget this one and add
		 * 1-2 subranges back
		 */
		cbi->memmap[m].mm_ತಳ_addr = cbi->memmap[m].mm_ಉದ್ದಳತೆ = 0;
		ಆದರೆ(substart > memaddr)
			ಕೂಡಿಸು_memmap(cbi, memaddr, substart-memaddr);
		ಆದರೆ(subend < memend)
			ಕೂಡಿಸು_memmap(cbi, subend, memend-subend);
	}
}

ಶೂನ್ಯ ಕೂಡಿಸು_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	ಪೂರ್ಣಾಂಕ m;
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	ಆದರೆ(addr > ಎಲ್ಲೆ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ(addr + len > ಎಲ್ಲೆ) {
		len -= (addr + len - ಎಲ್ಲೆ);
	}

	ಪ್ರತಿಜ್ಞಾ(cbi->mmap_ಗಾತ್ರ < MAXMEMMAP);
	ಆದರೆ(len == 0) {
		ಹಿಂತಿರುಗಿ;
	}
	addr = roundup(addr, ARM_ಪುಟ_ಗಾತ್ರ);
	len = rounddown(len, ARM_ಪುಟ_ಗಾತ್ರ);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

        ಗಾಗಿ(m = 0; m < MAXMEMMAP; m++) {
		phys_ಬೈಟುಗಳು highmark;
		ಆದರೆ(cbi->memmap[m].mm_ಉದ್ದಳತೆ) {
			ಮುಂದುವರಿ;
		}
		cbi->memmap[m].mm_ತಳ_addr = addr;
		cbi->memmap[m].mm_ಉದ್ದಳತೆ = len;
		cbi->memmap[m].type = MULTIBOOT_ಸ್ಮರಣೆ_ಲಭ್ಯ;
		ಆದರೆ(m >= cbi->mmap_ಗಾತ್ರ) {
			cbi->mmap_ಗಾತ್ರ = m+1;
		}
		highmark = addr + len;
		ಆದರೆ(highmark > cbi->mem_ಮೇಲಿನ_phys) {
			cbi->mem_ಮೇಲಿನ_phys = highmark;
		}
		ಹಿಂತಿರುಗಿ;
        }

	panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_ಬೈಟುಗಳು *ph)
{
	u32_t *ret;
#ವ್ಯಾಖ್ಯಾನಿಸು PG_PAGETABLES 24
	ಸ್ಥಿರ u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pt_inuse = 0;
	ಆದರೆ(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ಹಿಂತಿರುಗಿ ret;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_KB (ARM_ಪುಟ_ಗಾತ್ರ / 1024)

phys_ಬೈಟುಗಳು pg_alloc_ಪುಟ(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಂಕ m;
	multiboot_ಸ್ಮರಣೆ_map_t *mmap;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಗಾಗಿ(m = 0; m < cbi->mmap_ಗಾತ್ರ; m++) {
		mmap = &cbi->memmap[m];
		ಆದರೆ(!mmap->mm_ಉದ್ದಳತೆ) {
			ಮುಂದುವರಿ;
		}
		ಪ್ರತಿಜ್ಞಾ(mmap->mm_ಉದ್ದಳತೆ > 0);
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_ಉದ್ದಳತೆ % ARM_ಪುಟ_ಗಾತ್ರ));
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_ತಳ_addr % ARM_ಪುಟ_ಗಾತ್ರ));

		u32_t addr = mmap->mm_ತಳ_addr;
		mmap->mm_ತಳ_addr += ARM_ಪುಟ_ಗಾತ್ರ;
		mmap->mm_ಉದ್ದಳತೆ  -= ARM_ಪುಟ_ಗಾತ್ರ;

		cbi->kernel_allocated_ಬೈಟುಗಳು_dynamic += ARM_ಪುಟ_ಗಾತ್ರ;

		ಹಿಂತಿರುಗಿ addr;
	}

	panic("can't find free memory");
}

ಶೂನ್ಯ pg_ತಟಸ್ಥ(kinfo_t *cbi)
{
	uint32_t i;
	phys_ಬೈಟುಗಳು phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	ಪ್ರತಿಜ್ಞಾ(cbi->mem_ಮೇಲಿನ_phys);

        /* Set up an identity mapping page directory */
	 ಗಾಗಿ(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_ಗಾತ್ರ;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		ಆದರೆ (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_ಕೊನೆ) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} ಇಲ್ಲದಿದ್ದರೆ {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

ಪೂರ್ಣಾಂಕ pg_mapkernel(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ pde;
	u32_t mapped = 0, kern_phys = kern_phys_ಆರಂಭ;

	ಪ್ರತಿಜ್ಞಾ(!(kern_vir_ಆರಂಭ % ARM_SECTION_ಗಾತ್ರ));
	ಪ್ರತಿಜ್ಞಾ(!(kern_phys_ಆರಂಭ % ARM_SECTION_ಗಾತ್ರ));
	pde = kern_vir_ಆರಂಭ / ARM_SECTION_ಗಾತ್ರ; /* start pde */
	ಆಗ(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_ಮುಖವಾಡ) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_ಗಾತ್ರ;
		kern_phys += ARM_SECTION_ಗಾತ್ರ;
		pde++;
	}
	ಹಿಂತಿರುಗಿ pde;	/* free pde */
}

ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ)
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
	#ವ್ಯಾಖ್ಯಾನಿಸು CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_ಬೈಟುಗಳು pg_ತುಂಬು(ಶೂನ್ಯ)
{
	phys_ಬೈಟುಗಳು phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	ಹಿಂತಿರುಗಿ phpagedir;
}

ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ)
{
	memset(pagedir, 0, ಗಾತ್ರ(pagedir));
}

phys_ಬೈಟುಗಳು pg_rounddown(phys_ಬೈಟುಗಳು b)
{
	phys_ಬೈಟುಗಳು o;
	ಆದರೆ(!(o = b % ARM_ಪುಟ_ಗಾತ್ರ)) {
		ಹಿಂತಿರುಗಿ b;
	}
	ಹಿಂತಿರುಗಿ b  - o;
}

ಶೂನ್ಯ pg_map(phys_ಬೈಟುಗಳು phys, vir_ಬೈಟುಗಳು vaddr, vir_ಬೈಟುಗಳು vaddr_ಕೊನೆ,
	kinfo_t *cbi)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mapped_pde = -1;
	ಸ್ಥಿರ u32_t *pt = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ pde, pte;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಆದರೆ(phys == PG_ALLOCATEME) {
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % ARM_ಪುಟ_ಗಾತ್ರ));
	} ಇಲ್ಲದಿದ್ದರೆ  {
		ಪ್ರತಿಜ್ಞಾ((vaddr % ARM_ಪುಟ_ಗಾತ್ರ) == (phys % ARM_ಪುಟ_ಗಾತ್ರ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	ಪ್ರತಿಜ್ಞಾ(vaddr < kern_vir_ಆರಂಭ);

	ಆಗ(vaddr < vaddr_ಕೊನೆ) {
		phys_ಬೈಟುಗಳು ಮೂಲ = phys;
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % ARM_ಪುಟ_ಗಾತ್ರ));
		ಆದರೆ(phys == PG_ALLOCATEME) {
			ಮೂಲ = pg_alloc_ಪುಟ(cbi);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಪ್ರತಿಜ್ಞಾ(!(phys % ARM_ಪುಟ_ಗಾತ್ರ));
		}
		ಪ್ರತಿಜ್ಞಾ(!(ಮೂಲ % ARM_ಪುಟ_ಗಾತ್ರ));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		ಆದರೆ(mapped_pde < pde) {
			phys_ಬೈಟುಗಳು ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_ಮುಖವಾಡ)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		ಪ್ರತಿಜ್ಞಾ(pt);
		pt[pte] = (ಮೂಲ & ARM_VM_PTE_ಮುಖವಾಡ)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_ಪುಟ_ಗಾತ್ರ;
		ಆದರೆ(phys != PG_ALLOCATEME) {
			phys += ARM_ಪುಟ_ಗಾತ್ರ;
		}
	}
}

ಶೂನ್ಯ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
