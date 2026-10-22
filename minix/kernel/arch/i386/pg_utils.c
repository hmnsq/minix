
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <string.h>

/* These are set/computed in kernel.lds. */
ಬಾಹ್ಯ ಅಕ್ಷರ kern_vir_ತಳ, kern_phys_ತಳ, kern_ಗಾತ್ರ;

/* Retrieve the absolute values to something we can use. */
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_vir_ಆರಂಭ = (phys_ಬೈಟುಗಳು) &kern_vir_ತಳ;
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_phys_ಆರಂಭ = (phys_ಬೈಟುಗಳು) &kern_phys_ತಳ;
ಸ್ಥಿರ phys_ಬೈಟುಗಳು kern_kernlen = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;

/* page directory we can use to map things */
ಸ್ಥಿರ u32_t pagedir[1024]  __aligned(4096);

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

        ಆದರೆ((o=ಆರಂಭ % I386_ಪುಟ_ಗಾತ್ರ))
                ಆರಂಭ -= o;
        ಆದರೆ((o=ಕೊನೆ % I386_ಪುಟ_ಗಾತ್ರ))
                ಕೊನೆ += I386_ಪುಟ_ಗಾತ್ರ - o;

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

phys_ಬೈಟುಗಳು alloc_ಅತಿಕಡಿಮೆ(kinfo_t *cbi, phys_ಬೈಟುಗಳು len)
{
	/* Allocate the lowest physical page we have. */
	ಪೂರ್ಣಾಂಕ m;
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಲಿಗಳು 0xffffffff
	phys_ಬೈಟುಗಳು ಅತಿಕಡಿಮೆ = ಖಾಲಿಗಳು;
	ಪ್ರತಿಜ್ಞಾ(len > 0);
	len = roundup(len, I386_ಪುಟ_ಗಾತ್ರ);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಗಾಗಿ(m = 0; m < cbi->mmap_ಗಾತ್ರ; m++) {
		ಆದರೆ(cbi->memmap[m].mm_ಉದ್ದಳತೆ < len) ಮುಂದುವರಿ;
		ಆದರೆ(cbi->memmap[m].mm_ತಳ_addr < ಅತಿಕಡಿಮೆ) ಅತಿಕಡಿಮೆ = cbi->memmap[m].mm_ತಳ_addr;
	}
	ಪ್ರತಿಜ್ಞಾ(ಅತಿಕಡಿಮೆ != ಖಾಲಿಗಳು);
	cut_memmap(cbi, ಅತಿಕಡಿಮೆ, len);
	cbi->kernel_allocated_ಬೈಟುಗಳು_dynamic += len;
	ಹಿಂತಿರುಗಿ ಅತಿಕಡಿಮೆ;
}

ಶೂನ್ಯ ಕೂಡಿಸು_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        ಪೂರ್ಣಾಂಕ m;
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        ಆದರೆ(addr > ಎಲ್ಲೆ) ಹಿಂತಿರುಗಿ;
        ಆದರೆ(addr + len > ಎಲ್ಲೆ) {
                len -= (addr + len - ಎಲ್ಲೆ);
        }
        ಪ್ರತಿಜ್ಞಾ(cbi->mmap_ಗಾತ್ರ < MAXMEMMAP);
        ಆದರೆ(len == 0) ಹಿಂತಿರುಗಿ;
	addr = roundup(addr, I386_ಪುಟ_ಗಾತ್ರ);
	len = rounddown(len, I386_ಪುಟ_ಗಾತ್ರ);

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

        ಗಾಗಿ(m = 0; m < MAXMEMMAP; m++) {
		phys_ಬೈಟುಗಳು highmark;
                ಆದರೆ(cbi->memmap[m].mm_ಉದ್ದಳತೆ) ಮುಂದುವರಿ;
                cbi->memmap[m].mm_ತಳ_addr = addr;
                cbi->memmap[m].mm_ಉದ್ದಳತೆ = len;
                cbi->memmap[m].mm_type = MULTIBOOT_ಸ್ಮರಣೆ_ಲಭ್ಯ;
                ಆದರೆ(m >= cbi->mmap_ಗಾತ್ರ)
                        cbi->mmap_ಗಾತ್ರ = m+1;
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
#ವ್ಯಾಖ್ಯಾನಿಸು PG_PAGETABLES 6
	ಸ್ಥಿರ u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pt_inuse = 0;
	ಆದರೆ(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(pagetables[pt_inuse]) == I386_ಪುಟ_ಗಾತ್ರ);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ಹಿಂತಿರುಗಿ ret;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_KB (I386_ಪುಟ_ಗಾತ್ರ / 1024)

phys_ಬೈಟುಗಳು pg_alloc_ಪುಟ(kinfo_t *cbi)
{
	ಪೂರ್ಣಾಂಕ m;
	multiboot_ಸ್ಮರಣೆ_map_t *mmap;

	ಪ್ರತಿಜ್ಞಾ(kernel_may_alloc);

	ಗಾಗಿ(m = cbi->mmap_ಗಾತ್ರ-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		ಆದರೆ(!mmap->mm_ಉದ್ದಳತೆ) ಮುಂದುವರಿ;
		ಪ್ರತಿಜ್ಞಾ(mmap->mm_ಉದ್ದಳತೆ > 0);
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_ಉದ್ದಳತೆ % I386_ಪುಟ_ಗಾತ್ರ));
		ಪ್ರತಿಜ್ಞಾ(!(mmap->mm_ತಳ_addr % I386_ಪುಟ_ಗಾತ್ರ));

		mmap->mm_ಉದ್ದಳತೆ -= I386_ಪುಟ_ಗಾತ್ರ;

                cbi->kernel_allocated_ಬೈಟುಗಳು_dynamic += I386_ಪುಟ_ಗಾತ್ರ;

		ಹಿಂತಿರುಗಿ mmap->mm_ತಳ_addr + mmap->mm_ಉದ್ದಳತೆ;
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
        ಗಾಗಿ(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_ಪುಟ_ಗಾತ್ರ;
		ಆದರೆ((cbi->mem_ಮೇಲಿನ_phys & I386_VM_ADDR_ಮುಖವಾಡ_4_MB)
			<= (phys & I386_VM_ADDR_ಮುಖವಾಡ_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

ಪೂರ್ಣಾಂಕ pg_mapkernel(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ pde;
	u32_t mapped = 0, kern_phys = kern_phys_ಆರಂಭ;

        ಪ್ರತಿಜ್ಞಾ(!(kern_vir_ಆರಂಭ % I386_BIG_ಪುಟ_ಗಾತ್ರ));
        ಪ್ರತಿಜ್ಞಾ(!(kern_phys % I386_BIG_ಪುಟ_ಗಾತ್ರ));
        pde = kern_vir_ಆರಂಭ / I386_BIG_ಪುಟ_ಗಾತ್ರ; /* start pde */
	ಆಗ(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_ಪುಟ_ಗಾತ್ರ;
		kern_phys += I386_BIG_ಪುಟ_ಗಾತ್ರ;
		pde++;
	}
	ಹಿಂತಿರುಗಿ pde;	/* free pde */
}

ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ)
{
        u32_t cr0, cr4;
        ಪೂರ್ಣಾಂಕ pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PAE
	ಆದರೆ(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#ಆದರೆ_ಅಂತ್ಯ

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ಪ್ರತಿಜ್ಞಾ(cr0 & I386_CR0_PE);

        /* First clear PG and PGE flag, as PGE must be enabled after PG. */
        write_cr0(cr0 & ~I386_CR0_PG);
        write_cr4(cr4 & ~(I386_CR4_PGE | I386_CR4_PSE));

        cr0= read_cr0();
        cr4= read_cr4();

        /* Our page table contains 4MB entries. */
        cr4 |= I386_CR4_PSE;

        write_cr4(cr4);

        /* First enable paging, then enable global page flag. */
        cr0 |= I386_CR0_PG;
        write_cr0(cr0);
        cr0 |= I386_CR0_WP;
        write_cr0(cr0);

        /* May we enable these features? */
        ಆದರೆ(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_ಬೈಟುಗಳು pg_ತುಂಬು(ಶೂನ್ಯ)
{
	phys_ಬೈಟುಗಳು phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	ಹಿಂತಿರುಗಿ phpagedir;
}

ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ)
{
	memset(pagedir, 0, ಗಾತ್ರ(pagedir));
}

phys_ಬೈಟುಗಳು pg_rounddown(phys_ಬೈಟುಗಳು b)
{
	phys_ಬೈಟುಗಳು o;
	ಆದರೆ(!(o = b % I386_ಪುಟ_ಗಾತ್ರ))
		ಹಿಂತಿರುಗಿ b;
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
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % I386_ಪುಟ_ಗಾತ್ರ));
	} ಇಲ್ಲದಿದ್ದರೆ  {
		ಪ್ರತಿಜ್ಞಾ((vaddr % I386_ಪುಟ_ಗಾತ್ರ) == (phys % I386_ಪುಟ_ಗಾತ್ರ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ಪ್ರತಿಜ್ಞಾ(vaddr < kern_vir_ಆರಂಭ);

	ಆಗ(vaddr < vaddr_ಕೊನೆ) {
		phys_ಬೈಟುಗಳು ಮೂಲ = phys;
		ಪ್ರತಿಜ್ಞಾ(!(vaddr % I386_ಪುಟ_ಗಾತ್ರ));
		ಆದರೆ(phys == PG_ALLOCATEME) {
			ಮೂಲ = pg_alloc_ಪುಟ(cbi);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಪ್ರತಿಜ್ಞಾ(!(phys % I386_ಪುಟ_ಗಾತ್ರ));
		}
		ಪ್ರತಿಜ್ಞಾ(!(ಮೂಲ % I386_ಪುಟ_ಗಾತ್ರ));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		ಆದರೆ(mapped_pde < pde) {
			phys_ಬೈಟುಗಳು ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_ಮುಖವಾಡ)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ಪ್ರತಿಜ್ಞಾ(pt);
		pt[pte] = (ಮೂಲ & I386_VM_ADDR_ಮುಖವಾಡ) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_ಪುಟ_ಗಾತ್ರ;
		ಆದರೆ(phys != PG_ALLOCATEME)
			phys += I386_ಪುಟ_ಗಾತ್ರ;
	}
}

ಶೂನ್ಯ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

