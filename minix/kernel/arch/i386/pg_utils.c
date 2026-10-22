
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

/* These are set/computed in kernel.lds. */
ਬਾਹਰੀ ਅੱਖਰ kern_vir_ਅਧਾਰ, kern_phys_ਅਧਾਰ, kern_ਮਾਪ;

/* Retrieve the absolute values to something we can use. */
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_vir_ਸ਼ੁਰੂ = (phys_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_phys_ਸ਼ੁਰੂ = (phys_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ;
ਸਥਿਰ phys_ਬਾਈਟਾਂ kern_kernlen = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;

/* page directory we can use to map things */
ਸਥਿਰ u32_t pagedir[1024]  __aligned(4096);

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

        ਜੇ((o=ਸ਼ੁਰੂ % I386_ਪੰਨਾ_ਮਾਪ))
                ਸ਼ੁਰੂ -= o;
        ਜੇ((o=ਸਿਰਾ % I386_ਪੰਨਾ_ਮਾਪ))
                ਸਿਰਾ += I386_ਪੰਨਾ_ਮਾਪ - o;

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

phys_ਬਾਈਟਾਂ alloc_ਸਭ_ਤੋਂ_ਨੀਵਾਂ(kinfo_t *cbi, phys_ਬਾਈਟਾਂ len)
{
	/* Allocate the lowest physical page we have. */
	ਪੂਰਨ m;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਲੀ_ਥਾਵਾਂ 0xffffffff
	phys_ਬਾਈਟਾਂ ਸਭ_ਤੋਂ_ਨੀਵਾਂ = ਖਾਲੀ_ਥਾਵਾਂ;
	ਪ੍ਰਤਿਜ੍ਞਾ(len > 0);
	len = roundup(len, I386_ਪੰਨਾ_ਮਾਪ);

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

	ਲਈ(m = 0; m < cbi->mmap_ਮਾਪ; m++) {
		ਜੇ(cbi->memmap[m].mm_ਲੰਮਾਈ < len) ਜਾਰੀ;
		ਜੇ(cbi->memmap[m].mm_ਅਧਾਰ_addr < ਸਭ_ਤੋਂ_ਨੀਵਾਂ) ਸਭ_ਤੋਂ_ਨੀਵਾਂ = cbi->memmap[m].mm_ਅਧਾਰ_addr;
	}
	ਪ੍ਰਤਿਜ੍ਞਾ(ਸਭ_ਤੋਂ_ਨੀਵਾਂ != ਖਾਲੀ_ਥਾਵਾਂ);
	cut_memmap(cbi, ਸਭ_ਤੋਂ_ਨੀਵਾਂ, len);
	cbi->kernel_allocated_ਬਾਈਟਾਂ_dynamic += len;
	ਵਾਪਸ ਸਭ_ਤੋਂ_ਨੀਵਾਂ;
}

ਖਾਲੀ ਜੋੜੋ_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        ਪੂਰਨ m;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        ਜੇ(addr > ਹੱਦਬੰਦੀ) ਵਾਪਸ;
        ਜੇ(addr + len > ਹੱਦਬੰਦੀ) {
                len -= (addr + len - ਹੱਦਬੰਦੀ);
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(cbi->mmap_ਮਾਪ < MAXMEMMAP);
        ਜੇ(len == 0) ਵਾਪਸ;
	addr = roundup(addr, I386_ਪੰਨਾ_ਮਾਪ);
	len = rounddown(len, I386_ਪੰਨਾ_ਮਾਪ);

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

        ਲਈ(m = 0; m < MAXMEMMAP; m++) {
		phys_ਬਾਈਟਾਂ highmark;
                ਜੇ(cbi->memmap[m].mm_ਲੰਮਾਈ) ਜਾਰੀ;
                cbi->memmap[m].mm_ਅਧਾਰ_addr = addr;
                cbi->memmap[m].mm_ਲੰਮਾਈ = len;
                cbi->memmap[m].mm_type = MULTIBOOT_ਸਮ੍ਰਿਤੀ_ਉਪਲਬਧ;
                ਜੇ(m >= cbi->mmap_ਮਾਪ)
                        cbi->mmap_ਮਾਪ = m+1;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PG_PAGETABLES 6
	ਸਥਿਰ u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ਸਥਿਰ ਪੂਰਨ pt_inuse = 0;
	ਜੇ(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(pagetables[pt_inuse]) == I386_ਪੰਨਾ_ਮਾਪ);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	ਵਾਪਸ ret;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_KB (I386_ਪੰਨਾ_ਮਾਪ / 1024)

phys_ਬਾਈਟਾਂ pg_alloc_ਪੰਨਾ(kinfo_t *cbi)
{
	ਪੂਰਨ m;
	multiboot_ਸਮ੍ਰਿਤੀ_map_t *mmap;

	ਪ੍ਰਤਿਜ੍ਞਾ(kernel_may_alloc);

	ਲਈ(m = cbi->mmap_ਮਾਪ-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		ਜੇ(!mmap->mm_ਲੰਮਾਈ) ਜਾਰੀ;
		ਪ੍ਰਤਿਜ੍ਞਾ(mmap->mm_ਲੰਮਾਈ > 0);
		ਪ੍ਰਤਿਜ੍ਞਾ(!(mmap->mm_ਲੰਮਾਈ % I386_ਪੰਨਾ_ਮਾਪ));
		ਪ੍ਰਤਿਜ੍ਞਾ(!(mmap->mm_ਅਧਾਰ_addr % I386_ਪੰਨਾ_ਮਾਪ));

		mmap->mm_ਲੰਮਾਈ -= I386_ਪੰਨਾ_ਮਾਪ;

                cbi->kernel_allocated_ਬਾਈਟਾਂ_dynamic += I386_ਪੰਨਾ_ਮਾਪ;

		ਵਾਪਸ mmap->mm_ਅਧਾਰ_addr + mmap->mm_ਲੰਮਾਈ;
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
        ਲਈ(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_ਪੰਨਾ_ਮਾਪ;
		ਜੇ((cbi->mem_ਉੱਚਾ_phys & I386_VM_ADDR_ਨਕਾਬ_4_MB)
			<= (phys & I386_VM_ADDR_ਨਕਾਬ_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

ਪੂਰਨ pg_mapkernel(ਖਾਲੀ)
{
	ਪੂਰਨ pde;
	u32_t mapped = 0, kern_phys = kern_phys_ਸ਼ੁਰੂ;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(kern_vir_ਸ਼ੁਰੂ % I386_BIG_ਪੰਨਾ_ਮਾਪ));
        ਪ੍ਰਤਿਜ੍ਞਾ(!(kern_phys % I386_BIG_ਪੰਨਾ_ਮਾਪ));
        pde = kern_vir_ਸ਼ੁਰੂ / I386_BIG_ਪੰਨਾ_ਮਾਪ; /* start pde */
	ਜਦੋਂ(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_ਪੰਨਾ_ਮਾਪ;
		kern_phys += I386_BIG_ਪੰਨਾ_ਮਾਪ;
		pde++;
	}
	ਵਾਪਸ pde;	/* free pde */
}

ਖਾਲੀ vm_enable_paging(ਖਾਲੀ)
{
        u32_t cr0, cr4;
        ਪੂਰਨ pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PAE
	ਜੇ(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#ਜੇ_ਅੰਤ

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ਪ੍ਰਤਿਜ੍ਞਾ(cr0 & I386_CR0_PE);

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
        ਜੇ(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_ਬਾਈਟਾਂ pg_ਲੱਦੋ(ਖਾਲੀ)
{
	phys_ਬਾਈਟਾਂ phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	ਵਾਪਸ phpagedir;
}

ਖਾਲੀ pg_clear(ਖਾਲੀ)
{
	memset(pagedir, 0, ਆਕਾਰ(pagedir));
}

phys_ਬਾਈਟਾਂ pg_rounddown(phys_ਬਾਈਟਾਂ b)
{
	phys_ਬਾਈਟਾਂ o;
	ਜੇ(!(o = b % I386_ਪੰਨਾ_ਮਾਪ))
		ਵਾਪਸ b;
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
		ਪ੍ਰਤਿਜ੍ਞਾ(!(vaddr % I386_ਪੰਨਾ_ਮਾਪ));
	} ਨਹੀਂਤਾਂ  {
		ਪ੍ਰਤਿਜ੍ਞਾ((vaddr % I386_ਪੰਨਾ_ਮਾਪ) == (phys % I386_ਪੰਨਾ_ਮਾਪ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ਪ੍ਰਤਿਜ੍ਞਾ(vaddr < kern_vir_ਸ਼ੁਰੂ);

	ਜਦੋਂ(vaddr < vaddr_ਸਿਰਾ) {
		phys_ਬਾਈਟਾਂ ਸਰੋਤ = phys;
		ਪ੍ਰਤਿਜ੍ਞਾ(!(vaddr % I386_ਪੰਨਾ_ਮਾਪ));
		ਜੇ(phys == PG_ALLOCATEME) {
			ਸਰੋਤ = pg_alloc_ਪੰਨਾ(cbi);
		} ਨਹੀਂਤਾਂ {
			ਪ੍ਰਤਿਜ੍ਞਾ(!(phys % I386_ਪੰਨਾ_ਮਾਪ));
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(!(ਸਰੋਤ % I386_ਪੰਨਾ_ਮਾਪ));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		ਜੇ(mapped_pde < pde) {
			phys_ਬਾਈਟਾਂ ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_ਨਕਾਬ)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ਪ੍ਰਤਿਜ੍ਞਾ(pt);
		pt[pte] = (ਸਰੋਤ & I386_VM_ADDR_ਨਕਾਬ) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_ਪੰਨਾ_ਮਾਪ;
		ਜੇ(phys != PG_ALLOCATEME)
			phys += I386_ਪੰਨਾ_ਮਾਪ;
	}
}

ਖਾਲੀ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

