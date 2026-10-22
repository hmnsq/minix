
#ضمن <minix/cpufeature.h>

#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن <string.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_ادھار, kern_phys_ادھار, kern_ناپ;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بائٹاں kern_vir_شروع = (phys_بائٹاں) &kern_vir_ادھار;
ساكن phys_بائٹاں kern_phys_شروع = (phys_بائٹاں) &kern_phys_ادھار;
ساكن phys_بائٹاں kern_kernlen = (phys_بائٹاں) &kern_ناپ;

/* page directory we can use to map things */
ساكن u32_t pagedir[1024]  __aligned(4096);

فراغ print_memmap(kinfo_t *cbi)
{
        صحيح m;
        أكد(cbi->mmap_ناپ < MAXMEMMAP);
        لكل(m = 0; m < cbi->mmap_ناپ; m++) {
		phys_بائٹاں addr = cbi->memmap[m].mm_ادھار_addr, endit = cbi->memmap[m].mm_ادھار_addr + cbi->memmap[m].mm_لمائی;
                لکھو("%08lx-%08lx ",addr, endit);
        }
        لکھو("\nsize %08lx\n", cbi->mmap_ناپ);
}

فراغ cut_memmap(kinfo_t *cbi, phys_بائٹاں شروع, phys_بائٹاں سرا)
{
        صحيح m;
        phys_بائٹاں o;

        إذا((o=شروع % I386_ورقہ_ناپ))
                شروع -= o;
        إذا((o=سرا % I386_ورقہ_ناپ))
                سرا += I386_ورقہ_ناپ - o;

	أكد(kernel_may_alloc);

        لكل(m = 0; m < cbi->mmap_ناپ; m++) {
                phys_بائٹاں substart = شروع, subend = سرا;
                phys_بائٹاں memaddr = cbi->memmap[m].mm_ادھار_addr,
                        memend = cbi->memmap[m].mm_ادھار_addr + cbi->memmap[m].mm_لمائی;

                /* adjust cut range to be a subset of the free memory */
                إذا(substart < memaddr) substart = memaddr;
                إذا(subend > memend) subend = memend;
                إذا(substart >= subend) استمر;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_ادھار_addr = cbi->memmap[m].mm_لمائی = 0;
                إذا(substart > memaddr)
                        جوڑو_memmap(cbi, memaddr, substart-memaddr);
                إذا(subend < memend)
                        جوڑو_memmap(cbi, subend, memend-subend);
        }
}

phys_بائٹاں alloc_سبھ_توں_نیواں(kinfo_t *cbi, phys_بائٹاں len)
{
	/* Allocate the lowest physical page we have. */
	صحيح m;
#تعریف_کرو خالی_تھاواں 0xffffffff
	phys_بائٹاں سبھ_توں_نیواں = خالی_تھاواں;
	أكد(len > 0);
	len = roundup(len, I386_ورقہ_ناپ);

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_ناپ; m++) {
		إذا(cbi->memmap[m].mm_لمائی < len) استمر;
		إذا(cbi->memmap[m].mm_ادھار_addr < سبھ_توں_نیواں) سبھ_توں_نیواں = cbi->memmap[m].mm_ادھار_addr;
	}
	أكد(سبھ_توں_نیواں != خالی_تھاواں);
	cut_memmap(cbi, سبھ_توں_نیواں, len);
	cbi->kernel_allocated_بائٹاں_dynamic += len;
	أرجع سبھ_توں_نیواں;
}

فراغ جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحيح m;
#تعریف_کرو حد_بندی 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        إذا(addr > حد_بندی) أرجع;
        إذا(addr + len > حد_بندی) {
                len -= (addr + len - حد_بندی);
        }
        أكد(cbi->mmap_ناپ < MAXMEMMAP);
        إذا(len == 0) أرجع;
	addr = roundup(addr, I386_ورقہ_ناپ);
	len = rounddown(len, I386_ورقہ_ناپ);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بائٹاں highmark;
                إذا(cbi->memmap[m].mm_لمائی) استمر;
                cbi->memmap[m].mm_ادھار_addr = addr;
                cbi->memmap[m].mm_لمائی = len;
                cbi->memmap[m].mm_type = MULTIBOOT_حافظہ_دستیاب;
                إذا(m >= cbi->mmap_ناپ)
                        cbi->mmap_ناپ = m+1;
		highmark = addr + len;
		إذا(highmark > cbi->mem_بلند_phys) {
			cbi->mem_بلند_phys = highmark;
		}

                أرجع;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بائٹاں *ph)
{
	u32_t *ret;
#تعریف_کرو PG_PAGETABLES 6
	ساكن u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(حجم(pagetables[pt_inuse]) == I386_ورقہ_ناپ);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#تعریف_کرو ورقہ_KB (I386_ورقہ_ناپ / 1024)

phys_بائٹاں pg_alloc_ورقہ(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظہ_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = cbi->mmap_ناپ-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_لمائی) استمر;
		أكد(mmap->mm_لمائی > 0);
		أكد(!(mmap->mm_لمائی % I386_ورقہ_ناپ));
		أكد(!(mmap->mm_ادھار_addr % I386_ورقہ_ناپ));

		mmap->mm_لمائی -= I386_ورقہ_ناپ;

                cbi->kernel_allocated_بائٹاں_dynamic += I386_ورقہ_ناپ;

		أرجع mmap->mm_ادھار_addr + mmap->mm_لمائی;
	}

	panic("can't find free memory");
}

فراغ pg_پچھان_تت(kinfo_t *cbi)
{
	uint32_t i;
	phys_بائٹاں phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بلند_phys);

        /* Set up an identity mapping page directory */
        لكل(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_ورقہ_ناپ;
		إذا((cbi->mem_بلند_phys & I386_VM_ADDR_نقاب_4_MB)
			<= (phys & I386_VM_ADDR_نقاب_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_شروع;

        أكد(!(kern_vir_شروع % I386_BIG_ورقہ_ناپ));
        أكد(!(kern_phys % I386_BIG_ورقہ_ناپ));
        pde = kern_vir_شروع / I386_BIG_ورقہ_ناپ; /* start pde */
	طالما(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_ورقہ_ناپ;
		kern_phys += I386_BIG_ورقہ_ناپ;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
{
        u32_t cr0, cr4;
        صحيح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#جے_تعریف_شدہ PAE
	إذا(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#جے_انت

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

phys_بائٹاں pg_لدو(فراغ)
{
	phys_بائٹاں phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائٹاں pg_rounddown(phys_بائٹاں b)
{
	phys_بائٹاں o;
	إذا(!(o = b % I386_ورقہ_ناپ))
		أرجع b;
	أرجع b  - o;
}

فراغ pg_map(phys_بائٹاں phys, vir_بائٹاں vaddr, vir_بائٹاں vaddr_سرا,
	kinfo_t *cbi)
{
	ساكن صحيح mapped_pde = -1;
	ساكن u32_t *pt = عدم;
	صحيح pde, pte;

	أكد(kernel_may_alloc);

	إذا(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % I386_ورقہ_ناپ));
	} وإلا  {
		أكد((vaddr % I386_ورقہ_ناپ) == (phys % I386_ورقہ_ناپ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_شروع);

	طالما(vaddr < vaddr_سرا) {
		phys_بائٹاں سروت = phys;
		أكد(!(vaddr % I386_ورقہ_ناپ));
		إذا(phys == PG_ALLOCATEME) {
			سروت = pg_alloc_ورقہ(cbi);
		} وإلا {
			أكد(!(phys % I386_ورقہ_ناپ));
		}
		أكد(!(سروت % I386_ورقہ_ناپ));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بائٹاں ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_نقاب)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (سروت & I386_VM_ADDR_نقاب) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_ورقہ_ناپ;
		إذا(phys != PG_ALLOCATEME)
			phys += I386_ورقہ_ناپ;
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

