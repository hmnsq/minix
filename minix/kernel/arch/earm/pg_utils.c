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
خارجي حرف kern_vir_ادھار, kern_phys_ادھار, kern_ناپ;

/* Retrieve the absolute values to something we can use. */
ساكن phys_بائٹاں kern_vir_شروع = (phys_بائٹاں) &kern_vir_ادھار;
ساكن phys_بائٹاں kern_phys_شروع = (phys_بائٹاں) &kern_phys_ادھار;
ساكن phys_بائٹاں kern_kernlen = (phys_بائٹاں) &kern_ناپ;

/* page directory we can use to map things */
ساكن u32_t pagedir[4096]  __aligned(16384);

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

	إذا((o=شروع % ARM_ورقہ_ناپ))
		شروع -= o;
	إذا((o=سرا % ARM_ورقہ_ناپ))
		سرا += ARM_ورقہ_ناپ - o;

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

فراغ جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
	صحيح m;
#تعریف_کرو حد_بندی 0xFFFFF000
	/* Truncate available memory at 4GB as the rest of minix
	 * currently can't deal with any bigger.
	 */
	إذا(addr > حد_بندی) {
		أرجع;
	}

	إذا(addr + len > حد_بندی) {
		len -= (addr + len - حد_بندی);
	}

	أكد(cbi->mmap_ناپ < MAXMEMMAP);
	إذا(len == 0) {
		أرجع;
	}
	addr = roundup(addr, ARM_ورقہ_ناپ);
	len = rounddown(len, ARM_ورقہ_ناپ);

	أكد(kernel_may_alloc);

        لكل(m = 0; m < MAXMEMMAP; m++) {
		phys_بائٹاں highmark;
		إذا(cbi->memmap[m].mm_لمائی) {
			استمر;
		}
		cbi->memmap[m].mm_ادھار_addr = addr;
		cbi->memmap[m].mm_لمائی = len;
		cbi->memmap[m].type = MULTIBOOT_حافظہ_دستیاب;
		إذا(m >= cbi->mmap_ناپ) {
			cbi->mmap_ناپ = m+1;
		}
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
#تعریف_کرو PG_PAGETABLES 24
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

#تعریف_کرو ورقہ_KB (ARM_ورقہ_ناپ / 1024)

phys_بائٹاں pg_alloc_ورقہ(kinfo_t *cbi)
{
	صحيح m;
	multiboot_حافظہ_map_t *mmap;

	أكد(kernel_may_alloc);

	لكل(m = 0; m < cbi->mmap_ناپ; m++) {
		mmap = &cbi->memmap[m];
		إذا(!mmap->mm_لمائی) {
			استمر;
		}
		أكد(mmap->mm_لمائی > 0);
		أكد(!(mmap->mm_لمائی % ARM_ورقہ_ناپ));
		أكد(!(mmap->mm_ادھار_addr % ARM_ورقہ_ناپ));

		u32_t addr = mmap->mm_ادھار_addr;
		mmap->mm_ادھار_addr += ARM_ورقہ_ناپ;
		mmap->mm_لمائی  -= ARM_ورقہ_ناپ;

		cbi->kernel_allocated_بائٹاں_dynamic += ARM_ورقہ_ناپ;

		أرجع addr;
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
	 لكل(i = 0; i < ARM_VM_DIR_ENTRIES; i++) {
		u32_t flags = ARM_VM_SECTION
			| ARM_VM_SECTION_USER
			| ARM_VM_SECTION_DOMAIN;

		phys = i * ARM_SECTION_ناپ;
		/* mark mormal memory as cacheable. TODO: fix hard coded values */
		إذا (phys >= PHYS_MEM_BEGIN && phys <= PHYS_MEM_سرا) {
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

	أكد(!(kern_vir_شروع % ARM_SECTION_ناپ));
	أكد(!(kern_phys_شروع % ARM_SECTION_ناپ));
	pde = kern_vir_شروع / ARM_SECTION_ناپ; /* start pde */
	طالما(mapped < kern_kernlen) {
		pagedir[pde] = (kern_phys & ARM_VM_SECTION_نقاب) 
			| ARM_VM_SECTION
			| ARM_VM_SECTION_SUPER
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED;
		mapped += ARM_SECTION_ناپ;
		kern_phys += ARM_SECTION_ناپ;
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
	#تعریف_کرو CORTEX_A8_L2EN   (0x02)
	actlr = read_actlr();
	actlr |= CORTEX_A8_L2EN;
	write_actlr(actlr);

	write_sctlr(sctlr);
}

phys_بائٹاں pg_لدو(فراغ)
{
	phys_بائٹاں phpagedir = vir2phys(pagedir);
	write_ttbr0(phpagedir);
	أرجع phpagedir;
}

فراغ pg_clear(فراغ)
{
	memset(pagedir, 0, حجم(pagedir));
}

phys_بائٹاں pg_rounddown(phys_بائٹاں b)
{
	phys_بائٹاں o;
	إذا(!(o = b % ARM_ورقہ_ناپ)) {
		أرجع b;
	}
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
		أكد(!(vaddr % ARM_ورقہ_ناپ));
	} وإلا  {
		أكد((vaddr % ARM_ورقہ_ناپ) == (phys % ARM_ورقہ_ناپ));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}
	أكد(vaddr < kern_vir_شروع);

	طالما(vaddr < vaddr_سرا) {
		phys_بائٹاں سروت = phys;
		أكد(!(vaddr % ARM_ورقہ_ناپ));
		إذا(phys == PG_ALLOCATEME) {
			سروت = pg_alloc_ورقہ(cbi);
		} وإلا {
			أكد(!(phys % ARM_ورقہ_ناپ));
		}
		أكد(!(سروت % ARM_ورقہ_ناپ));
		pde = ARM_VM_PDE(vaddr);
		pte = ARM_VM_PTE(vaddr);
		إذا(mapped_pde < pde) {
			phys_بائٹاں ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & ARM_VM_PDE_نقاب)
					| ARM_VM_PAGEDIR
					| ARM_VM_PDE_DOMAIN;
			mapped_pde = pde;
		}
		أكد(pt);
		pt[pte] = (سروت & ARM_VM_PTE_نقاب)
			| ARM_VM_PAGETABLE
			| ARM_VM_PTE_CACHED
			| ARM_VM_PTE_USER;
		vaddr += ARM_ورقہ_ناپ;
		إذا(phys != PG_ALLOCATEME) {
			phys += ARM_ورقہ_ناپ;
		}
	}
}

فراغ pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}
