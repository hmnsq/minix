#ضمن <minix/cpufeature.h>

#ضمن <minix/type.h>
#ضمن <assert.h>
#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"
#ضمن <machine/cpu.h>
#ضمن <arm/armreg.h>

#ضمن <string.h>
#ضمن <minix/type.h>

/* These are set/computed in kernel.lds. */
خارجي حرف kern_vir_بنياد, kern_phys_بنياد, kern_سائيز;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بائيٽون kern_vir_شروعات = (phys_بائيٽون) &kern_vir_بنياد;
ساكن phys_بائيٽون kern_phys_شروعات = (phys_بائيٽون) &kern_phys_بنياد;
ساكن phys_بائيٽون kern_kernlen = (phys_بائيٽون) &kern_سائيز;

/* page directory we can use to map things */
ساكن u32_t pagedir[4096]  __aligned(16384);

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

	إذا((o=شروعات % ARM_صفحو_سائيز))
		شروعات -= o;
	إذا((o=پڇاڙي % ARM_صفحو_سائيز))
		پڇاڙي += ARM_صفحو_سائيز - o;

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

فراغ جوڙيو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحيح m;
#وصف_ڪريو حد_بندي 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	إذا(addr > حد_بندي) {
		أرجع;
	}

	إذا(addr + len > حد_بندي) {
		len -= (addr + len - حد_بندي);
	}

	أكد(cbi->mmap_سائيز < MAXMEMMAP);
	إذا(len == 0) {
		أرجع;
	}
	addr = roundup(addr, ARM_صفحو_سائيز);
	len = rounddown(len, ARM_صفحو_سائيز);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بائيٽون highmark;
		إذا(cbi->memmap[m].mm_ڊيگهائي) {
			استمر;
		}
		cbi->memmap[m].mm_بنياد_addr = addr;
		cbi->memmap[m].mm_ڊيگهائي = len;
		cbi->memmap[m].type = MULTIBOOT_يادگيري_موجود;
		إذا(m >= cbi->mmap_سائيز) {
			cbi->mmap_سائيز = m+1;
		}
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
#وصف_ڪريو PG_PAGETABLES 24
	ساكن u32_t pagetables[PG_PAGETABLES][256]  __aligned(1024);
	ساكن صحيح pt_inuse = 0;
	إذا(pt_inuse >= PG_PAGETABLES) {
		panic("no more pagetables");
	}
	أكد(حجم(pagetables[pt_inuse]) == 1024);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	أرجع ret;
}

#وصف_ڪريو صفحو_KB (ARM_صفحو_سائيز / 1024)

phys_بائيٽون pg_alloc_صفحو(kinfo_t *cbi)
{
	صحيح m;
	multiboot_يادگيري_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_سائيز; m++) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_ڊيگهائي) {
			استمر;
		}
		أكد(mmap->mm_ڊيگهائي > 0);
		أكد(!(mmap->mm_ڊيگهائي % ARM_صفحو_سائيز));
		أكد(!(mmap->mm_بنياد_addr % ARM_صفحو_سائيز));

		u32_t addr = mmap->mm_بنياد_addr;
		mmap->mm_بنياد_addr += ARM_صفحو_سائيز;
		mmap->mm_ڊيگهائي  -= ARM_صفحو_سائيز;

		cbi->kernel_allocated_بائيٽون_dynamic += ARM_صفحو_سائيز;

		أرجع addr;
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
	 لكل(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_سائيز;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		إذا (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_پڇاڙي) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} وإلا {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_شروعات;

	أكد(!(kern_vir_شروعات % ARM_SECTION_سائيز));
	أكد(!(kern_phys_شروعات % ARM_SECTION_سائيز));
	pde = kern_vir_شروعات / ARM_SECTION_سائيز; /* start pde */
	طالما(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_نقاب) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_سائيز;
		kern_phys += ARM_SECTION_سائيز;
		pde++;
	}
	أرجع pde;	/* free pde */
}

فراغ vm_enable_paging(فراغ)
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
	#وصف_ڪريو CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_بائيٽون pg_ڀريو(فراغ)
{
	phys_بائيٽون phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائيٽون pg_rounddown(phys_بائيٽون b)
{
	phys_بائيٽون o;
	إذا(!(o = b % ARM_صفحو_سائيز)) {
		أرجع b;
	}
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
		أكد(!(vaddr % ARM_صفحو_سائيز));
	} وإلا  {
		أكد((vaddr % ARM_صفحو_سائيز) == (phys % ARM_صفحو_سائيز));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_شروعات);

	طالما(vaddr < vaddr_پڇاڙي) {
		phys_بائيٽون ذريعو = phys;
		أكد(!(vaddr % ARM_صفحو_سائيز));
		إذا(phys == PG_ALLOCATEME) {
			ذريعو = pg_alloc_صفحو(cbi);
		} وإلا {
			أكد(!(phys % ARM_صفحو_سائيز));
		}
		أكد(!(ذريعو % ARM_صفحو_سائيز));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بائيٽون ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_نقاب)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (ذريعو & ARM_VM_PTE_نقاب)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_صفحو_سائيز;
		إذا(phys != PG_ALLOCATEME) {
			phys += ARM_صفحو_سائيز;
		}
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
