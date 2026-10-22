
#ضمن <minix/cpufeature.h>

#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن <string.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_پایه, kern_phys_پایه, kern_اندازه;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بایتها kern_vir_شروع = (phys_بایتها) &kern_vir_پایه;
ساكن phys_بایتها kern_phys_شروع = (phys_بایتها) &kern_phys_پایه;
ساكن phys_بایتها kern_kernlen = (phys_بایتها) &kern_اندازه;

/* page directory we can use to map things */
ساكن u32_t pagedir[1024]  __aligned(4096);

فراغ print_memmap(kinfo_t *cbi)
{
        صحيح m;
        أكد(cbi->mmap_اندازه < MAXMEMMAP);
        لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		phys_بایتها addr = cbi->memmap[m].mm_پایه_addr, endit = cbi->memmap[m].mm_پایه_addr + cbi->memmap[m].mm_درازی;
                چاپ_کن("%08lx-%08lx ",addr, endit);
        }
        چاپ_کن("\nsize %08lx\n", cbi->mmap_اندازه);
}

فراغ cut_memmap(kinfo_t *cbi, phys_بایتها شروع, phys_بایتها انتها)
{
        صحيح m;
        phys_بایتها o;

        إذا((o=شروع % I386_صفحه_اندازه))
                شروع -= o;
        إذا((o=انتها % I386_صفحه_اندازه))
                انتها += I386_صفحه_اندازه - o;

	أكد(kernel_may_alloc);

        لكل(m = 0; m < cbi->mmap_اندازه; m++) {
                phys_بایتها substart = شروع, subend = انتها;
                phys_بایتها memaddr = cbi->memmap[m].mm_پایه_addr,
                        memend = cbi->memmap[m].mm_پایه_addr + cbi->memmap[m].mm_درازی;

                /* adjust cut range to be a subset of the free memory */
                إذا(substart < memaddr) substart = memaddr;
                إذا(subend > memend) subend = memend;
                إذا(substart >= subend) استمر;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_پایه_addr = cbi->memmap[m].mm_درازی = 0;
                إذا(substart > memaddr)
                        جمع_کن_memmap(cbi, memaddr, substart-memaddr);
                إذا(subend < memend)
                        جمع_کن_memmap(cbi, subend, memend-subend);
        }
}

phys_بایتها alloc_پایینترین(kinfo_t *cbi, phys_بایتها len)
{
	/* Allocate the lowest physical page we have. */
	صحيح m;
#تعریف_کن خالی 0xffffffff
	phys_بایتها پایینترین = خالی;
	أكد(len > 0);
	len = roundup(len, I386_صفحه_اندازه);

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		إذا(cbi->memmap[m].mm_درازی < len) استمر;
		إذا(cbi->memmap[m].mm_پایه_addr < پایینترین) پایینترین = cbi->memmap[m].mm_پایه_addr;
	}
	أكد(پایینترین != خالی);
	cut_memmap(cbi, پایینترین, len);
	cbi->kernel_allocated_بایتها_dynamic += len;
	أرجع پایینترین;
}

فراغ جمع_کن_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحيح m;
#تعریف_کن حد 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        إذا(addr > حد) أرجع;
        إذا(addr + len > حد) {
                len -= (addr + len - حد);
        }
        أكد(cbi->mmap_اندازه < MAXMEMMAP);
        إذا(len == 0) أرجع;
	addr = roundup(addr, I386_صفحه_اندازه);
	len = rounddown(len, I386_صفحه_اندازه);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بایتها highmark;
                إذا(cbi->memmap[m].mm_درازی) استمر;
                cbi->memmap[m].mm_پایه_addr = addr;
                cbi->memmap[m].mm_درازی = len;
                cbi->memmap[m].mm_type = MULTIBOOT_حافظه_موجود;
                إذا(m >= cbi->mmap_اندازه)
                        cbi->mmap_اندازه = m+1;
		highmark = addr + len;
		إذا(highmark > cbi->mem_بالا_phys) {
			cbi->mem_بالا_phys = highmark;
		}

                أرجع;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بایتها *ph)
{
	u32_t *ret;
#تعریف_کن PG_PAGETABLES 6
	ساكن u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(حجم(pagetables[pt_inuse]) == I386_صفحه_اندازه);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#تعریف_کن صفحه_KB (I386_صفحه_اندازه / 1024)

phys_بایتها pg_alloc_صفحه(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظه_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = cbi->mmap_اندازه-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_درازی) استمر;
		أكد(mmap->mm_درازی > 0);
		أكد(!(mmap->mm_درازی % I386_صفحه_اندازه));
		أكد(!(mmap->mm_پایه_addr % I386_صفحه_اندازه));

		mmap->mm_درازی -= I386_صفحه_اندازه;

                cbi->kernel_allocated_بایتها_dynamic += I386_صفحه_اندازه;

		أرجع mmap->mm_پایه_addr + mmap->mm_درازی;
	}

	panic("can't find free memory");
}

فراغ pg_عنصر_خنثی(kinfo_t *cbi)
{
	uint32_t i;
	phys_بایتها phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بالا_phys);

        /* Set up an identity mapping page directory */
        لكل(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_صفحه_اندازه;
		إذا((cbi->mem_بالا_phys & I386_VM_ADDR_ماسک_4_MB)
			<= (phys & I386_VM_ADDR_ماسک_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_شروع;

        أكد(!(kern_vir_شروع % I386_BIG_صفحه_اندازه));
        أكد(!(kern_phys % I386_BIG_صفحه_اندازه));
        pde = kern_vir_شروع / I386_BIG_صفحه_اندازه; /* start pde */
	طالما(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_صفحه_اندازه;
		kern_phys += I386_BIG_صفحه_اندازه;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
{
        u32_t cr0, cr4;
        صحيح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#اگر_تعریف_شده PAE
	إذا(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#پایان_اگر

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

phys_بایتها pg_بارگیری(فراغ)
{
	phys_بایتها phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بایتها pg_rounddown(phys_بایتها b)
{
	phys_بایتها o;
	إذا(!(o = b % I386_صفحه_اندازه))
		أرجع b;
	أرجع b  - o;
}

فراغ pg_map(phys_بایتها phys, vir_بایتها vaddr, vir_بایتها vaddr_انتها,
	kinfo_t *cbi)
{
	ساكن صحيح mapped_pde = -1;
	ساكن u32_t *pt = عدم;
	صحيح pde, pte;

	أكد(kernel_may_alloc);

	إذا(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % I386_صفحه_اندازه));
	} وإلا  {
		أكد((vaddr % I386_صفحه_اندازه) == (phys % I386_صفحه_اندازه));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_شروع);

	طالما(vaddr < vaddr_انتها) {
		phys_بایتها منبع = phys;
		أكد(!(vaddr % I386_صفحه_اندازه));
		إذا(phys == PG_ALLOCATEME) {
			منبع = pg_alloc_صفحه(cbi);
		} وإلا {
			أكد(!(phys % I386_صفحه_اندازه));
		}
		أكد(!(منبع % I386_صفحه_اندازه));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بایتها ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_ماسک)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (منبع & I386_VM_ADDR_ماسک) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_صفحه_اندازه;
		إذا(phys != PG_ALLOCATEME)
			phys += I386_صفحه_اندازه;
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

