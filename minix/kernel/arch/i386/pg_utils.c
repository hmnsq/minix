
#ضمن <minix/cpufeature.h>

#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن <string.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_بنياد, kern_phys_بنياد, kern_سائيز;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بائيٽون kern_vir_شروعات = (phys_بائيٽون) &kern_vir_بنياد;
ساكن phys_بائيٽون kern_phys_شروعات = (phys_بائيٽون) &kern_phys_بنياد;
ساكن phys_بائيٽون kern_kernlen = (phys_بائيٽون) &kern_سائيز;

/* page directory we can use to map things */
ساكن u32_t pagedir[1024]  __aligned(4096);

فراغ print_memmap(kinfo_t *cbi)
{
        صحيح m;
        أكد(cbi->mmap_سائيز < MAXMEMMAP);
        لكل(m = 0; m < cbi->mmap_سائيز; m++) {
		phys_بائيٽون addr = cbi->memmap[m].mm_بنياد_addr, endit = cbi->memmap[m].mm_بنياد_addr + cbi->memmap[m].mm_ڊيگهائي;
                لکو("%08lx-%08lx ",addr, endit);
        }
        لکو("\nsize %08lx\n", cbi->mmap_سائيز);
}

فراغ cut_memmap(kinfo_t *cbi, phys_بائيٽون شروعات, phys_بائيٽون پڇاڙي)
{
        صحيح m;
        phys_بائيٽون o;

        إذا((o=شروعات % I386_صفحو_سائيز))
                شروعات -= o;
        إذا((o=پڇاڙي % I386_صفحو_سائيز))
                پڇاڙي += I386_صفحو_سائيز - o;

	أكد(kernel_may_alloc);

        لكل(m = 0; m < cbi->mmap_سائيز; m++) {
                phys_بائيٽون substart = شروعات, subend = پڇاڙي;
                phys_بائيٽون memaddr = cbi->memmap[m].mm_بنياد_addr,
                        memend = cbi->memmap[m].mm_بنياد_addr + cbi->memmap[m].mm_ڊيگهائي;

                /* adjust cut range to be a subset of the free memory */
                إذا(substart < memaddr) substart = memaddr;
                إذا(subend > memend) subend = memend;
                إذا(substart >= subend) استمر;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_بنياد_addr = cbi->memmap[m].mm_ڊيگهائي = 0;
                إذا(substart > memaddr)
                        جوڙيو_memmap(cbi, memaddr, substart-memaddr);
                إذا(subend < memend)
                        جوڙيو_memmap(cbi, subend, memend-subend);
        }
}

phys_بائيٽون alloc_سڀ_کان_هيٺ(kinfo_t *cbi, phys_بائيٽون len)
{
	/* Allocate the lowest physical page we have. */
	صحيح m;
#وصف_ڪريو خالي_جايون 0xffffffff
	phys_بائيٽون سڀ_کان_هيٺ = خالي_جايون;
	أكد(len > 0);
	len = roundup(len, I386_صفحو_سائيز);

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_سائيز; m++) {
		إذا(cbi->memmap[m].mm_ڊيگهائي < len) استمر;
		إذا(cbi->memmap[m].mm_بنياد_addr < سڀ_کان_هيٺ) سڀ_کان_هيٺ = cbi->memmap[m].mm_بنياد_addr;
	}
	أكد(سڀ_کان_هيٺ != خالي_جايون);
	cut_memmap(cbi, سڀ_کان_هيٺ, len);
	cbi->kernel_allocated_بائيٽون_dynamic += len;
	أرجع سڀ_کان_هيٺ;
}

فراغ جوڙيو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحيح m;
#وصف_ڪريو حد_بندي 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        إذا(addr > حد_بندي) أرجع;
        إذا(addr + len > حد_بندي) {
                len -= (addr + len - حد_بندي);
        }
        أكد(cbi->mmap_سائيز < MAXMEMMAP);
        إذا(len == 0) أرجع;
	addr = roundup(addr, I386_صفحو_سائيز);
	len = rounddown(len, I386_صفحو_سائيز);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بائيٽون highmark;
                إذا(cbi->memmap[m].mm_ڊيگهائي) استمر;
                cbi->memmap[m].mm_بنياد_addr = addr;
                cbi->memmap[m].mm_ڊيگهائي = len;
                cbi->memmap[m].mm_type = MULTIBOOT_يادگيري_موجود;
                إذا(m >= cbi->mmap_سائيز)
                        cbi->mmap_سائيز = m+1;
		highmark = addr + len;
		إذا(highmark > cbi->mem_مٿاهون_phys) {
			cbi->mem_مٿاهون_phys = highmark;
		}

                أرجع;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بائيٽون *ph)
{
	u32_t *ret;
#وصف_ڪريو PG_PAGETABLES 6
	ساكن u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(حجم(pagetables[pt_inuse]) == I386_صفحو_سائيز);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#وصف_ڪريو صفحو_KB (I386_صفحو_سائيز / 1024)

phys_بائيٽون pg_alloc_صفحو(kinfo_t *cbi)
{
	صحيح m;
	multiboot_يادگيري_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = cbi->mmap_سائيز-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_ڊيگهائي) استمر;
		أكد(mmap->mm_ڊيگهائي > 0);
		أكد(!(mmap->mm_ڊيگهائي % I386_صفحو_سائيز));
		أكد(!(mmap->mm_بنياد_addr % I386_صفحو_سائيز));

		mmap->mm_ڊيگهائي -= I386_صفحو_سائيز;

                cbi->kernel_allocated_بائيٽون_dynamic += I386_صفحو_سائيز;

		أرجع mmap->mm_بنياد_addr + mmap->mm_ڊيگهائي;
	}

	panic("can't find free memory");
}

فراغ pg_سڃاڻپ_عنصر(kinfo_t *cbi)
{
	uint32_t i;
	phys_بائيٽون phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_مٿاهون_phys);

        /* Set up an identity mapping page directory */
        لكل(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_صفحو_سائيز;
		إذا((cbi->mem_مٿاهون_phys & I386_VM_ADDR_نقاب_4_MB)
			<= (phys & I386_VM_ADDR_نقاب_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_شروعات;

        أكد(!(kern_vir_شروعات % I386_BIG_صفحو_سائيز));
        أكد(!(kern_phys % I386_BIG_صفحو_سائيز));
        pde = kern_vir_شروعات / I386_BIG_صفحو_سائيز; /* start pde */
	طالما(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_صفحو_سائيز;
		kern_phys += I386_BIG_صفحو_سائيز;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
{
        u32_t cr0, cr4;
        صحيح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#جيڪڏهن_وصف_ٿيل PAE
	إذا(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#جيڪڏهن_پورو

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

phys_بائيٽون pg_ڀريو(فراغ)
{
	phys_بائيٽون phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائيٽون pg_rounddown(phys_بائيٽون b)
{
	phys_بائيٽون o;
	إذا(!(o = b % I386_صفحو_سائيز))
		أرجع b;
	أرجع b  - o;
}

فراغ pg_map(phys_بائيٽون phys, vir_بائيٽون vaddr, vir_بائيٽون vaddr_پڇاڙي,
	kinfo_t *cbi)
{
	ساكن صحيح mapped_pde = -1;
	ساكن u32_t *pt = عدم;
	صحيح pde, pte;

	أكد(kernel_may_alloc);

	إذا(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % I386_صفحو_سائيز));
	} وإلا  {
		أكد((vaddr % I386_صفحو_سائيز) == (phys % I386_صفحو_سائيز));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_شروعات);

	طالما(vaddr < vaddr_پڇاڙي) {
		phys_بائيٽون ذريعو = phys;
		أكد(!(vaddr % I386_صفحو_سائيز));
		إذا(phys == PG_ALLOCATEME) {
			ذريعو = pg_alloc_صفحو(cbi);
		} وإلا {
			أكد(!(phys % I386_صفحو_سائيز));
		}
		أكد(!(ذريعو % I386_صفحو_سائيز));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بائيٽون ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_نقاب)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (ذريعو & I386_VM_ADDR_نقاب) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_صفحو_سائيز;
		إذا(phys != PG_ALLOCATEME)
			phys += I386_صفحو_سائيز;
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

