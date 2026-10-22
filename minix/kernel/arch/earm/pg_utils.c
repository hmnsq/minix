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
خارجي حرف kern_vir_پایه, kern_phys_پایه, kern_اندازه;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بایتها kern_vir_شروع = (phys_بایتها) &kern_vir_پایه;
ساكن phys_بایتها kern_phys_شروع = (phys_بایتها) &kern_phys_پایه;
ساكن phys_بایتها kern_kernlen = (phys_بایتها) &kern_اندازه;

/* page directory we can use to map things */
ساكن u32_t pagedir[4096]  __aligned(16384);

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

	إذا((o=شروع % ARM_صفحه_اندازه))
		شروع -= o;
	إذا((o=انتها % ARM_صفحه_اندازه))
		انتها += ARM_صفحه_اندازه - o;

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

فراغ جمع_کن_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحيح m;
#تعریف_کن حد 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	إذا(addr > حد) {
		أرجع;
	}

	إذا(addr + len > حد) {
		len -= (addr + len - حد);
	}

	أكد(cbi->mmap_اندازه < MAXMEMMAP);
	إذا(len == 0) {
		أرجع;
	}
	addr = roundup(addr, ARM_صفحه_اندازه);
	len = rounddown(len, ARM_صفحه_اندازه);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بایتها highmark;
		إذا(cbi->memmap[m].mm_درازی) {
			استمر;
		}
		cbi->memmap[m].mm_پایه_addr = addr;
		cbi->memmap[m].mm_درازی = len;
		cbi->memmap[m].type = MULTIBOOT_حافظه_موجود;
		إذا(m >= cbi->mmap_اندازه) {
			cbi->mmap_اندازه = m+1;
		}
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
#تعریف_کن PG_PAGETABLES 24
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

#تعریف_کن صفحه_KB (ARM_صفحه_اندازه / 1024)

phys_بایتها pg_alloc_صفحه(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظه_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_اندازه; m++) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_درازی) {
			استمر;
		}
		أكد(mmap->mm_درازی > 0);
		أكد(!(mmap->mm_درازی % ARM_صفحه_اندازه));
		أكد(!(mmap->mm_پایه_addr % ARM_صفحه_اندازه));

		u32_t addr = mmap->mm_پایه_addr;
		mmap->mm_پایه_addr += ARM_صفحه_اندازه;
		mmap->mm_درازی  -= ARM_صفحه_اندازه;

		cbi->kernel_allocated_بایتها_dynamic += ARM_صفحه_اندازه;

		أرجع addr;
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
	 لكل(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_اندازه;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		إذا (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_انتها) {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_CACHED;
		} وإلا {
			pagedir[i] =  phys | flags | ARM_VM_SECTION_DEVICE;
		}
        }
}

صحيح pg_mapkernel(فراغ)
{
	صحيح pde;
	u32_t mapped = 0, kern_phys = kern_phys_شروع;

	أكد(!(kern_vir_شروع % ARM_SECTION_اندازه));
	أكد(!(kern_phys_شروع % ARM_SECTION_اندازه));
	pde = kern_vir_شروع / ARM_SECTION_اندازه; /* start pde */
	طالما(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_ماسک) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_اندازه;
		kern_phys += ARM_SECTION_اندازه;
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
	#تعریف_کن CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_بایتها pg_بارگیری(فراغ)
{
	phys_بایتها phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بایتها pg_rounddown(phys_بایتها b)
{
	phys_بایتها o;
	إذا(!(o = b % ARM_صفحه_اندازه)) {
		أرجع b;
	}
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
		أكد(!(vaddr % ARM_صفحه_اندازه));
	} وإلا  {
		أكد((vaddr % ARM_صفحه_اندازه) == (phys % ARM_صفحه_اندازه));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_شروع);

	طالما(vaddr < vaddr_انتها) {
		phys_بایتها منبع = phys;
		أكد(!(vaddr % ARM_صفحه_اندازه));
		إذا(phys == PG_ALLOCATEME) {
			منبع = pg_alloc_صفحه(cbi);
		} وإلا {
			أكد(!(phys % ARM_صفحه_اندازه));
		}
		أكد(!(منبع % ARM_صفحه_اندازه));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بایتها ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_ماسک)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (منبع & ARM_VM_PTE_ماسک)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_صفحه_اندازه;
		إذا(phys != PG_ALLOCATEME) {
			phys += ARM_صفحه_اندازه;
		}
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
