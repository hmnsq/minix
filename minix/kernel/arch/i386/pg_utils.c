
#شامل <minix/cpufeature.h>

#شامل <assert.h>
#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

#شامل <string.h>

/* These are set/computed in kernel.lds. */
خارجی حرف kern_vir_بنیاد, kern_phys_بنیاد, kern_سائز;

/* Retrieve the absolute values to something we can use. */
ساکن phys_بائٹیں kern_vir_آغاز = (phys_بائٹیں) &kern_vir_بنیاد;
ساکن phys_بائٹیں kern_phys_آغاز = (phys_بائٹیں) &kern_phys_بنیاد;
ساکن phys_بائٹیں kern_kernlen = (phys_بائٹیں) &kern_سائز;

/* page directory we can use to map things */
ساکن u32_t pagedir[1024]  __aligned(4096);

خالی print_memmap(kinfo_t *cbi)
{
        صحیح m;
        أكد(cbi->mmap_سائز < MAXMEMMAP);
        برائے(m = 0; m < cbi->mmap_سائز; m++) {
		phys_بائٹیں addr = cbi->memmap[m].mm_بنیاد_addr, endit = cbi->memmap[m].mm_بنیاد_addr + cbi->memmap[m].mm_لمبائی;
                لکھو("%08lx-%08lx ",addr, endit);
        }
        لکھو("\nsize %08lx\n", cbi->mmap_سائز);
}

خالی cut_memmap(kinfo_t *cbi, phys_بائٹیں آغاز, phys_بائٹیں انتہا)
{
        صحیح m;
        phys_بائٹیں o;

        اگر((o=آغاز % I386_صفحہ_سائز))
                آغاز -= o;
        اگر((o=انتہا % I386_صفحہ_سائز))
                انتہا += I386_صفحہ_سائز - o;

	أكد(kernel_may_alloc);

        برائے(m = 0; m < cbi->mmap_سائز; m++) {
                phys_بائٹیں substart = آغاز, subend = انتہا;
                phys_بائٹیں memaddr = cbi->memmap[m].mm_بنیاد_addr,
                        memend = cbi->memmap[m].mm_بنیاد_addr + cbi->memmap[m].mm_لمبائی;

                /* adjust cut range to be a subset of the free memory */
                اگر(substart < memaddr) substart = memaddr;
                اگر(subend > memend) subend = memend;
                اگر(substart >= subend) جاری;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_بنیاد_addr = cbi->memmap[m].mm_لمبائی = 0;
                اگر(substart > memaddr)
                        جوڑو_memmap(cbi, memaddr, substart-memaddr);
                اگر(subend < memend)
                        جوڑو_memmap(cbi, subend, memend-subend);
        }
}

phys_بائٹیں alloc_سب_سے_نیچا(kinfo_t *cbi, phys_بائٹیں len)
{
	/* Allocate the lowest physical page we have. */
	صحیح m;
#تعریف_کرو خالی_خانے 0xffffffff
	phys_بائٹیں سب_سے_نیچا = خالی_خانے;
	أكد(len > 0);
	len = roundup(len, I386_صفحہ_سائز);

	أكد(kernel_may_alloc);

	برائے(m = 0; m < cbi->mmap_سائز; m++) {
		اگر(cbi->memmap[m].mm_لمبائی < len) جاری;
		اگر(cbi->memmap[m].mm_بنیاد_addr < سب_سے_نیچا) سب_سے_نیچا = cbi->memmap[m].mm_بنیاد_addr;
	}
	أكد(سب_سے_نیچا != خالی_خانے);
	cut_memmap(cbi, سب_سے_نیچا, len);
	cbi->kernel_allocated_بائٹیں_dynamic += len;
	واپس سب_سے_نیچا;
}

خالی جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        صحیح m;
#تعریف_کرو حد_بندی 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        اگر(addr > حد_بندی) واپس;
        اگر(addr + len > حد_بندی) {
                len -= (addr + len - حد_بندی);
        }
        أكد(cbi->mmap_سائز < MAXMEMMAP);
        اگر(len == 0) واپس;
	addr = roundup(addr, I386_صفحہ_سائز);
	len = rounddown(len, I386_صفحہ_سائز);

	أكد(kernel_may_alloc);

        برائے(m = 0; m < MAXMEMMAP; m++) {
		phys_بائٹیں highmark;
                اگر(cbi->memmap[m].mm_لمبائی) جاری;
                cbi->memmap[m].mm_بنیاد_addr = addr;
                cbi->memmap[m].mm_لمبائی = len;
                cbi->memmap[m].mm_type = MULTIBOOT_حافظہ_دستیاب;
                اگر(m >= cbi->mmap_سائز)
                        cbi->mmap_سائز = m+1;
		highmark = addr + len;
		اگر(highmark > cbi->mem_بلند_phys) {
			cbi->mem_بلند_phys = highmark;
		}

                واپس;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_بائٹیں *ph)
{
	u32_t *ret;
#تعریف_کرو PG_PAGETABLES 6
	ساکن u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	ساکن صحیح pt_inuse = 0;
	اگر(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	أكد(حجم(pagetables[pt_inuse]) == I386_صفحہ_سائز);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	واپس ret;
}

#تعریف_کرو صفحہ_KB (I386_صفحہ_سائز / 1024)

phys_بائٹیں pg_alloc_صفحہ(kinfo_t *cbi)
{
	صحیح m;
	multiboot_حافظہ_map_t *mmap;

	أكد(kernel_may_alloc);

	برائے(m = cbi->mmap_سائز-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		اگر(!mmap->mm_لمبائی) جاری;
		أكد(mmap->mm_لمبائی > 0);
		أكد(!(mmap->mm_لمبائی % I386_صفحہ_سائز));
		أكد(!(mmap->mm_بنیاد_addr % I386_صفحہ_سائز));

		mmap->mm_لمبائی -= I386_صفحہ_سائز;

                cbi->kernel_allocated_بائٹیں_dynamic += I386_صفحہ_سائز;

		واپس mmap->mm_بنیاد_addr + mmap->mm_لمبائی;
	}

	panic("can't find free memory");
}

خالی pg_شناختی_عنصر(kinfo_t *cbi)
{
	uint32_t i;
	phys_بائٹیں phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	أكد(cbi->mem_بلند_phys);

        /* Set up an identity mapping page directory */
        برائے(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_صفحہ_سائز;
		اگر((cbi->mem_بلند_phys & I386_VM_ADDR_نقاب_4_MB)
			<= (phys & I386_VM_ADDR_نقاب_4_MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

صحیح pg_mapkernel(خالی)
{
	صحیح pde;
	u32_t mapped = 0, kern_phys = kern_phys_آغاز;

        أكد(!(kern_vir_آغاز % I386_BIG_صفحہ_سائز));
        أكد(!(kern_phys % I386_BIG_صفحہ_سائز));
        pde = kern_vir_آغاز / I386_BIG_صفحہ_سائز; /* start pde */
	جب(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_صفحہ_سائز;
		kern_phys += I386_BIG_صفحہ_سائز;
		pde++;
	}
	واپس pde;	/* free pde */
}

خالی vm_enable_paging(خالی)
{
        u32_t cr0, cr4;
        صحیح pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#اگر_تعریف_شدہ PAE
	اگر(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#اگر_ختم

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
        اگر(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_بائٹیں pg_بھرو(خالی)
{
	phys_بائٹیں phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	واپس phpagedir;
}

خالی pg_clear(خالی)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائٹیں pg_rounddown(phys_بائٹیں b)
{
	phys_بائٹیں o;
	اگر(!(o = b % I386_صفحہ_سائز))
		واپس b;
	واپس b  - o;
}

خالی pg_map(phys_بائٹیں phys, vir_بائٹیں vaddr, vir_بائٹیں vaddr_انتہا,
	kinfo_t *cbi)
{
	ساکن صحیح mapped_pde = -1;
	ساکن u32_t *pt = تہی;
	صحیح pde, pte;

	أكد(kernel_may_alloc);

	اگر(phys == PG_ALLOCATEME) {
		أكد(!(vaddr % I386_صفحہ_سائز));
	} ورنہ  {
		أكد((vaddr % I386_صفحہ_سائز) == (phys % I386_صفحہ_سائز));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	أكد(vaddr < kern_vir_آغاز);

	جب(vaddr < vaddr_انتہا) {
		phys_بائٹیں ماخذ = phys;
		أكد(!(vaddr % I386_صفحہ_سائز));
		اگر(phys == PG_ALLOCATEME) {
			ماخذ = pg_alloc_صفحہ(cbi);
		} ورنہ {
			أكد(!(phys % I386_صفحہ_سائز));
		}
		أكد(!(ماخذ % I386_صفحہ_سائز));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		اگر(mapped_pde < pde) {
			phys_بائٹیں ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_نقاب)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (ماخذ & I386_VM_ADDR_نقاب) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_صفحہ_سائز;
		اگر(phys != PG_ALLOCATEME)
			phys += I386_صفحہ_سائز;
	}
}

خالی pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

