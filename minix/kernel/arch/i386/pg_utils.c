
#అన్తర్భావ <minix/cpufeature.h>

#అన్తర్భావ <assert.h>
#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ <string.h>

/* These are set/computed in kernel.lds. */
బాహ్య అక్షరం kern_vir_పునాది, kern_phys_పునాది, kern_కొలత;

/* Retrieve the absolute values to something we can use. */
స్థిర phys_బైట్లు kern_vir_ప్రారంభం = (phys_బైట్లు) &kern_vir_పునాది;
స్థిర phys_బైట్లు kern_phys_ప్రారంభం = (phys_బైట్లు) &kern_phys_పునాది;
స్థిర phys_బైట్లు kern_kernlen = (phys_బైట్లు) &kern_కొలత;

/* page directory we can use to map things */
స్థిర u32_t pagedir[1024]  __aligned(4096);

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

        ఒకవేళ((o=ప్రారంభం % I386_పుట_కొలత))
                ప్రారంభం -= o;
        ఒకవేళ((o=చివర % I386_పుట_కొలత))
                చివర += I386_పుట_కొలత - o;

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

phys_బైట్లు alloc_అత్యల్ప(kinfo_t *cbi, phys_బైట్లు len)
{
	/* Allocate the lowest physical page we have. */
	పూర్ణాంకం m;
#నిర్వచించు ఖాళీలు 0xffffffff
	phys_బైట్లు అత్యల్ప = ఖాళీలు;
	ప్రతిజ్ఞా(len > 0);
	len = roundup(len, I386_పుట_కొలత);

	ప్రతిజ్ఞా(kernel_may_alloc);

	కొరకు(m = 0; m < cbi->mmap_కొలత; m++) {
		ఒకవేళ(cbi->memmap[m].mm_నిడివి < len) కొనసాగు;
		ఒకవేళ(cbi->memmap[m].mm_పునాది_addr < అత్యల్ప) అత్యల్ప = cbi->memmap[m].mm_పునాది_addr;
	}
	ప్రతిజ్ఞా(అత్యల్ప != ఖాళీలు);
	cut_memmap(cbi, అత్యల్ప, len);
	cbi->kernel_allocated_బైట్లు_dynamic += len;
	తిరిగి అత్యల్ప;
}

శూన్య కూడు_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        పూర్ణాంకం m;
#నిర్వచించు హద్దు 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        ఒకవేళ(addr > హద్దు) తిరిగి;
        ఒకవేళ(addr + len > హద్దు) {
                len -= (addr + len - హద్దు);
        }
        ప్రతిజ్ఞా(cbi->mmap_కొలత < MAXMEMMAP);
        ఒకవేళ(len == 0) తిరిగి;
	addr = roundup(addr, I386_పుట_కొలత);
	len = rounddown(len, I386_పుట_కొలత);

	ప్రతిజ్ఞా(kernel_may_alloc);

        కొరకు(m = 0; m < MAXMEMMAP; m++) {
		phys_బైట్లు highmark;
                ఒకవేళ(cbi->memmap[m].mm_నిడివి) కొనసాగు;
                cbi->memmap[m].mm_పునాది_addr = addr;
                cbi->memmap[m].mm_నిడివి = len;
                cbi->memmap[m].mm_type = MULTIBOOT_జ్ఞాపకం_అందుబాటు;
                ఒకవేళ(m >= cbi->mmap_కొలత)
                        cbi->mmap_కొలత = m+1;
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
#నిర్వచించు PG_PAGETABLES 6
	స్థిర u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	స్థిర పూర్ణాంకం pt_inuse = 0;
	ఒకవేళ(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	ప్రతిజ్ఞా(పరిమాణం(pagetables[pt_inuse]) == I386_పుట_కొలత);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	తిరిగి ret;
}

#నిర్వచించు పుట_KB (I386_పుట_కొలత / 1024)

phys_బైట్లు pg_alloc_పుట(kinfo_t *cbi)
{
	పూర్ణాంకం m;
	multiboot_జ్ఞాపకం_map_t *mmap;

	ప్రతిజ్ఞా(kernel_may_alloc);

	కొరకు(m = cbi->mmap_కొలత-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		ఒకవేళ(!mmap->mm_నిడివి) కొనసాగు;
		ప్రతిజ్ఞా(mmap->mm_నిడివి > 0);
		ప్రతిజ్ఞా(!(mmap->mm_నిడివి % I386_పుట_కొలత));
		ప్రతిజ్ఞా(!(mmap->mm_పునాది_addr % I386_పుట_కొలత));

		mmap->mm_నిడివి -= I386_పుట_కొలత;

                cbi->kernel_allocated_బైట్లు_dynamic += I386_పుట_కొలత;

		తిరిగి mmap->mm_పునాది_addr + mmap->mm_నిడివి;
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
        కొరకు(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_పుట_కొలత;
		ఒకవేళ((cbi->mem_ఎగువ_phys & I386_VM_ADDR_ముసుగు_4_MB)
			<= (phys & I386_VM_ADDR_ముసుగు_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

పూర్ణాంకం pg_mapkernel(శూన్య)
{
	పూర్ణాంకం pde;
	u32_t mapped = 0, kern_phys = kern_phys_ప్రారంభం;

        ప్రతిజ్ఞా(!(kern_vir_ప్రారంభం % I386_BIG_పుట_కొలత));
        ప్రతిజ్ఞా(!(kern_phys % I386_BIG_పుట_కొలత));
        pde = kern_vir_ప్రారంభం / I386_BIG_పుట_కొలత; /* start pde */
	అయితే(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_పుట_కొలత;
		kern_phys += I386_BIG_పుట_కొలత;
		pde++;
	}
	తిరిగి pde;	/* free pde */
}

శూన్య vm_enable_paging(శూన్య)
{
        u32_t cr0, cr4;
        పూర్ణాంకం pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#నిర్వచితమైతే PAE
	ఒకవేళ(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#అయితే_ముగింపు

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	ప్రతిజ్ఞా(cr0 & I386_CR0_PE);

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
        ఒకవేళ(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_బైట్లు pg_ఎక్కించు(శూన్య)
{
	phys_బైట్లు phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	తిరిగి phpagedir;
}

శూన్య pg_clear(శూన్య)
{
	memset(pagedir, 0, పరిమాణం(pagedir));
}

phys_బైట్లు pg_rounddown(phys_బైట్లు b)
{
	phys_బైట్లు o;
	ఒకవేళ(!(o = b % I386_పుట_కొలత))
		తిరిగి b;
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
		ప్రతిజ్ఞా(!(vaddr % I386_పుట_కొలత));
	} లేకపోతే  {
		ప్రతిజ్ఞా((vaddr % I386_పుట_కొలత) == (phys % I386_పుట_కొలత));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	ప్రతిజ్ఞా(vaddr < kern_vir_ప్రారంభం);

	అయితే(vaddr < vaddr_చివర) {
		phys_బైట్లు ఆధారం = phys;
		ప్రతిజ్ఞా(!(vaddr % I386_పుట_కొలత));
		ఒకవేళ(phys == PG_ALLOCATEME) {
			ఆధారం = pg_alloc_పుట(cbi);
		} లేకపోతే {
			ప్రతిజ్ఞా(!(phys % I386_పుట_కొలత));
		}
		ప్రతిజ్ఞా(!(ఆధారం % I386_పుట_కొలత));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		ఒకవేళ(mapped_pde < pde) {
			phys_బైట్లు ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_ముసుగు)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		ప్రతిజ్ఞా(pt);
		pt[pte] = (ఆధారం & I386_VM_ADDR_ముసుగు) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_పుట_కొలత;
		ఒకవేళ(phys != PG_ALLOCATEME)
			phys += I386_పుట_కొలత;
	}
}

శూన్య pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

