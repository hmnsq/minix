
#ضمن <minix/cpufeature.h>

#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن <string.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_بنسټ, kern_phys_بنسټ, kern_اندازه;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بایټونه kern_vir_پیل = (phys_بایټونه) &kern_vir_بنسټ;
ساكن phys_بایټونه kern_phys_پیل = (phys_بایټونه) &kern_phys_بنسټ;
ساكن phys_بایټونه kern_kernlen = (phys_بایټونه) &kern_اندازه;

/* page directory we can use to map things */
ساكن u32_t pagedir[1024]  __aligned(4096);

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

        إذا((o=پیل % I386_پاڼه_اندازه))
                پیل -= o;
        إذا((o=پای_ټکی % I386_پاڼه_اندازه))
                پای_ټکی += I386_پاڼه_اندازه - o;

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

phys_بایټونه alloc_ټیټ_ترین(kinfo_t *cbi, phys_بایټونه len)
{
	/* Allocate the lowest physical page we have. */
	صحيح m;
#تعریف_کړه تشه 0xffffffff
	phys_بایټونه ټیټ_ترین = تشه;
	أكد(len > 0);
	len = roundup(len, I386_پاڼه_اندازه);

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		إذا(cbi->memmap[m].mm_اوږدتیا < len) استمر;
		إذا(cbi->memmap[m].mm_بنسټ_addr < ټیټ_ترین) ټیټ_ترین = cbi->memmap[m].mm_بنسټ_addr;
	}
	أكد(ټیټ_ترین != تشه);
	cut_memmap(cbi, ټیټ_ترین, len);
	cbi->kernel_allocated_بایټونه_dynamic += len;
	أرجع ټیټ_ترین;
}

فراغ جمع_کړه_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحيح m;
#تعریف_کړه حد 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        إذا(addr > حد) أرجع;
        إذا(addr + len > حد) {
                len -= (addr + len - حد);
        }
        أكد(cbi->mmap_اندازه < MAXMEMMAP);
        إذا(len == 0) أرجع;
	addr = roundup(addr, I386_پاڼه_اندازه);
	len = rounddown(len, I386_پاڼه_اندازه);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بایټونه highmark;
                إذا(cbi->memmap[m].mm_اوږدتیا) استمر;
                cbi->memmap[m].mm_بنسټ_addr = addr;
                cbi->memmap[m].mm_اوږدتیا = len;
                cbi->memmap[m].mm_type = MULTIBOOT_حافظه_شته;
                إذا(m >= cbi->mmap_اندازه)
                        cbi->mmap_اندازه = m+1;
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
#تعریف_کړه PG_PAGETABLES 6
	ساكن u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(حجم(pagetables[pt_inuse]) == I386_پاڼه_اندازه);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#تعریف_کړه پاڼه_KB (I386_پاڼه_اندازه / 1024)

phys_بایټونه pg_alloc_پاڼه(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظه_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = cbi->mmap_اندازه-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_اوږدتیا) استمر;
		أكد(mmap->mm_اوږدتیا > 0);
		أكد(!(mmap->mm_اوږدتیا % I386_پاڼه_اندازه));
		أكد(!(mmap->mm_بنسټ_addr % I386_پاڼه_اندازه));

		mmap->mm_اوږدتیا -= I386_پاڼه_اندازه;

                cbi->kernel_allocated_بایټونه_dynamic += I386_پاڼه_اندازه;

		أرجع mmap->mm_بنسټ_addr + mmap->mm_اوږدتیا;
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
        لكل(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_پاڼه_اندازه;
		إذا((cbi->mem_لوړ_phys & I386_VM_ADDR_ماسک_4_MB)
			<= (phys & I386_VM_ADDR_ماسک_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_پیل;

        أكد(!(kern_vir_پیل % I386_BIG_پاڼه_اندازه));
        أكد(!(kern_phys % I386_BIG_پاڼه_اندازه));
        pde = kern_vir_پیل / I386_BIG_پاڼه_اندازه; /* start pde */
	طالما(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_پاڼه_اندازه;
		kern_phys += I386_BIG_پاڼه_اندازه;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
{
        u32_t cr0, cr4;
        صحيح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#که_تعریف_شوی PAE
	إذا(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#د_که_پای

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	أكد(cr0 & I386_CR0_PE);

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
        إذا(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_بایټونه pg_بارول(فراغ)
{
	phys_بایټونه phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بایټونه pg_rounddown(phys_بایټونه b)
{
	phys_بایټونه o;
	إذا(!(o = b % I386_پاڼه_اندازه))
		أرجع b;
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
		أكد(!(vaddr % I386_پاڼه_اندازه));
	} وإلا  {
		أكد((vaddr % I386_پاڼه_اندازه) == (phys % I386_پاڼه_اندازه));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_پیل);

	طالما(vaddr < vaddr_پای_ټکی) {
		phys_بایټونه سرچینه = phys;
		أكد(!(vaddr % I386_پاڼه_اندازه));
		إذا(phys == PG_ALLOCATEME) {
			سرچینه = pg_alloc_پاڼه(cbi);
		} وإلا {
			أكد(!(phys % I386_پاڼه_اندازه));
		}
		أكد(!(سرچینه % I386_پاڼه_اندازه));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بایټونه ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_ماسک)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (سرچینه & I386_VM_ADDR_ماسک) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_پاڼه_اندازه;
		إذا(phys != PG_ALLOCATEME)
			phys += I386_پاڼه_اندازه;
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

