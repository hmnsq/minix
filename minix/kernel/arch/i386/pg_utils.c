
#अन्तर्भाव <minix/cpufeature.h>

#अन्तर्भाव <assert.h>
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <string.h>

/* These are set/computed in kernel.lds. */
बाह्य वर्ण kern_vir_आधारः, kern_phys_आधारः, kern_आकार;

/* Retrieve the absolute values to something we can use. */
स्थिर phys_अष्टकानि kern_vir_start = (phys_अष्टकानि) &kern_vir_आधारः;
स्थिर phys_अष्टकानि kern_phys_start = (phys_अष्टकानि) &kern_phys_आधारः;
स्थिर phys_अष्टकानि kern_kernlen = (phys_अष्टकानि) &kern_आकार;

/* page directory we can use to map things */
स्थिर u32_t pagedir[1024]  __aligned(4096);

शून्य print_memmap(kinfo_t *cbi)
{
        पूर्णाङ्क m;
        प्रतिज्ञा(cbi->mmap_आकार < MAXMEMMAP);
        प्रति(m = 0; m < cbi->mmap_आकार; m++) {
		phys_अष्टकानि addr = cbi->memmap[m].mm_आधारः_addr, endit = cbi->memmap[m].mm_आधारः_addr + cbi->memmap[m].mm_दीर्घता;
                प्रारूप_लिख("%08lx-%08lx ",addr, endit);
        }
        प्रारूप_लिख("\nsize %08lx\n", cbi->mmap_आकार);
}

शून्य cut_memmap(kinfo_t *cbi, phys_अष्टकानि start, phys_अष्टकानि अन्तः_2)
{
        पूर्णाङ्क m;
        phys_अष्टकानि o;

        यदि((o=start % I386_PAGE_आकार))
                start -= o;
        यदि((o=अन्तः_2 % I386_PAGE_आकार))
                अन्तः_2 += I386_PAGE_आकार - o;

	प्रतिज्ञा(kernel_may_alloc);

        प्रति(m = 0; m < cbi->mmap_आकार; m++) {
                phys_अष्टकानि substart = start, subend = अन्तः_2;
                phys_अष्टकानि memaddr = cbi->memmap[m].mm_आधारः_addr,
                        memend = cbi->memmap[m].mm_आधारः_addr + cbi->memmap[m].mm_दीर्घता;

                /* adjust cut range to be a subset of the free memory */
                यदि(substart < memaddr) substart = memaddr;
                यदि(subend > memend) subend = memend;
                यदि(substart >= subend) अनुवर्त;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_आधारः_addr = cbi->memmap[m].mm_दीर्घता = 0;
                यदि(substart > memaddr)
                        सङ्कलय_memmap(cbi, memaddr, substart-memaddr);
                यदि(subend < memend)
                        सङ्कलय_memmap(cbi, subend, memend-subend);
        }
}

phys_अष्टकानि alloc_निम्नतमम्(kinfo_t *cbi, phys_अष्टकानि len)
{
	/* Allocate the lowest physical page we have. */
	पूर्णाङ्क m;
#परिभाषय रिक्तानि 0xffffffff
	phys_अष्टकानि निम्नतमम् = रिक्तानि;
	प्रतिज्ञा(len > 0);
	len = roundup(len, I386_PAGE_आकार);

	प्रतिज्ञा(kernel_may_alloc);

	प्रति(m = 0; m < cbi->mmap_आकार; m++) {
		यदि(cbi->memmap[m].mm_दीर्घता < len) अनुवर्त;
		यदि(cbi->memmap[m].mm_आधारः_addr < निम्नतमम्) निम्नतमम् = cbi->memmap[m].mm_आधारः_addr;
	}
	प्रतिज्ञा(निम्नतमम् != रिक्तानि);
	cut_memmap(cbi, निम्नतमम्, len);
	cbi->kernel_allocated_अष्टकानि_dynamic += len;
	प्रत्यागम निम्नतमम्;
}

शून्य सङ्कलय_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        पूर्णाङ्क m;
#परिभाषय सीमा 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        यदि(addr > सीमा) प्रत्यागम;
        यदि(addr + len > सीमा) {
                len -= (addr + len - सीमा);
        }
        प्रतिज्ञा(cbi->mmap_आकार < MAXMEMMAP);
        यदि(len == 0) प्रत्यागम;
	addr = roundup(addr, I386_PAGE_आकार);
	len = rounddown(len, I386_PAGE_आकार);

	प्रतिज्ञा(kernel_may_alloc);

        प्रति(m = 0; m < MAXMEMMAP; m++) {
		phys_अष्टकानि highmark;
                यदि(cbi->memmap[m].mm_दीर्घता) अनुवर्त;
                cbi->memmap[m].mm_आधारः_addr = addr;
                cbi->memmap[m].mm_दीर्घता = len;
                cbi->memmap[m].mm_type = MULTIBOOT_स्मृतिः_उपलब्धम्;
                यदि(m >= cbi->mmap_आकार)
                        cbi->mmap_आकार = m+1;
		highmark = addr + len;
		यदि(highmark > cbi->mem_उच्चः_phys) {
			cbi->mem_उच्चः_phys = highmark;
		}

                प्रत्यागम;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_अष्टकानि *ph)
{
	u32_t *ret;
#परिभाषय PG_PAGETABLES 6
	स्थिर u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	स्थिर पूर्णाङ्क pt_inuse = 0;
	यदि(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	प्रतिज्ञा(परिमाण(pagetables[pt_inuse]) == I386_PAGE_आकार);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	प्रत्यागम ret;
}

#परिभाषय PAGE_KB (I386_PAGE_आकार / 1024)

phys_अष्टकानि pg_alloc_page(kinfo_t *cbi)
{
	पूर्णाङ्क m;
	multiboot_स्मृतिः_map_t *mmap;

	प्रतिज्ञा(kernel_may_alloc);

	प्रति(m = cbi->mmap_आकार-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		यदि(!mmap->mm_दीर्घता) अनुवर्त;
		प्रतिज्ञा(mmap->mm_दीर्घता > 0);
		प्रतिज्ञा(!(mmap->mm_दीर्घता % I386_PAGE_आकार));
		प्रतिज्ञा(!(mmap->mm_आधारः_addr % I386_PAGE_आकार));

		mmap->mm_दीर्घता -= I386_PAGE_आकार;

                cbi->kernel_allocated_अष्टकानि_dynamic += I386_PAGE_आकार;

		प्रत्यागम mmap->mm_आधारः_addr + mmap->mm_दीर्घता;
	}

	panic("can't find free memory");
}

शून्य pg_तटस्थः(kinfo_t *cbi)
{
	uint32_t i;
	phys_अष्टकानि phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	प्रतिज्ञा(cbi->mem_उच्चः_phys);

        /* Set up an identity mapping page directory */
        प्रति(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_आकार;
		यदि((cbi->mem_उच्चः_phys & I386_VM_ADDR_आवरणम्_4_MB)
			<= (phys & I386_VM_ADDR_आवरणम्_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

पूर्णाङ्क pg_mapkernel(शून्य)
{
	पूर्णाङ्क pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        प्रतिज्ञा(!(kern_vir_start % I386_BIG_PAGE_आकार));
        प्रतिज्ञा(!(kern_phys % I386_BIG_PAGE_आकार));
        pde = kern_vir_start / I386_BIG_PAGE_आकार; /* start pde */
	यावत्(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_आकार;
		kern_phys += I386_BIG_PAGE_आकार;
		pde++;
	}
	प्रत्यागम pde;	/* free pde */
}

शून्य vm_enable_paging(शून्य)
{
        u32_t cr0, cr4;
        पूर्णाङ्क pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#यदि_परिभाषितम् PAE
	यदि(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#यदि_अन्तः

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	प्रतिज्ञा(cr0 & I386_CR0_PE);

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
        यदि(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_अष्टकानि pg_आरोपय(शून्य)
{
	phys_अष्टकानि phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	प्रत्यागम phpagedir;
}

शून्य pg_clear(शून्य)
{
	स्मृतिं_पूरय(pagedir, 0, परिमाण(pagedir));
}

phys_अष्टकानि pg_rounddown(phys_अष्टकानि b)
{
	phys_अष्टकानि o;
	यदि(!(o = b % I386_PAGE_आकार))
		प्रत्यागम b;
	प्रत्यागम b  - o;
}

शून्य pg_map(phys_अष्टकानि phys, vir_अष्टकानि vaddr, vir_अष्टकानि vaddr_अन्तः,
	kinfo_t *cbi)
{
	स्थिर पूर्णाङ्क mapped_pde = -1;
	स्थिर u32_t *pt = रिक्त;
	पूर्णाङ्क pde, pte;

	प्रतिज्ञा(kernel_may_alloc);

	यदि(phys == PG_ALLOCATEME) {
		प्रतिज्ञा(!(vaddr % I386_PAGE_आकार));
	} अन्यथा  {
		प्रतिज्ञा((vaddr % I386_PAGE_आकार) == (phys % I386_PAGE_आकार));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	प्रतिज्ञा(vaddr < kern_vir_start);

	यावत्(vaddr < vaddr_अन्तः) {
		phys_अष्टकानि source = phys;
		प्रतिज्ञा(!(vaddr % I386_PAGE_आकार));
		यदि(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} अन्यथा {
			प्रतिज्ञा(!(phys % I386_PAGE_आकार));
		}
		प्रतिज्ञा(!(source % I386_PAGE_आकार));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		यदि(mapped_pde < pde) {
			phys_अष्टकानि ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_आवरणम्)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		प्रतिज्ञा(pt);
		pt[pte] = (source & I386_VM_ADDR_आवरणम्) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_आकार;
		यदि(phys != PG_ALLOCATEME)
			phys += I386_PAGE_आकार;
	}
}

शून्य pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

