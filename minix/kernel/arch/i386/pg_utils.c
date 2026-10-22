
#include <minix/cpufeature.h>

#include <assert.h>
#include "kernel/kernel.h"
#include "arch_proto.h"

#include <string.h>

/* These are set/computed in kernel.lds. */
extern char _kern_vir_base, _kern_phys_base, _kern_size;

/* Retrieve the absolute values to something we can use. */
static phys_bytes kern_vir_start = (phys_bytes) &_kern_vir_base;
static phys_bytes kern_phys_start = (phys_bytes) &_kern_phys_base;
static phys_bytes kern_kernlen = (phys_bytes) &_kern_size;

/* page directory we can use to map things */
static u32_t pagedir[1024]  __aligned(4096);

void print_memmap(kinfo_t *cbi)
{
        整数 m;
        assert(cbi->mmap_size < MAXMEMMAP);
        为(m = 0; m < cbi->mmap_size; m++) {
		phys_bytes addr = cbi->memmap[m].mm_base_addr, endit = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;
                printf("%08lx-%08lx ",addr, endit);
        }
        printf("\nsize %08lx\n", cbi->mmap_size);
}

void cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end)
{
        整数 m;
        phys_bytes o;

        如果((o=start % I386_PAGE_SIZE))
                start -= o;
        如果((o=end % I386_PAGE_SIZE))
                end += I386_PAGE_SIZE - o;

	assert(kernel_may_alloc);

        为(m = 0; m < cbi->mmap_size; m++) {
                phys_bytes substart = start, subend = end;
                phys_bytes memaddr = cbi->memmap[m].mm_base_addr,
                        memend = cbi->memmap[m].mm_base_addr + cbi->memmap[m].mm_length;

                /* adjust cut range to be a subset of the free memory */
                如果(substart < memaddr) substart = memaddr;
                如果(subend > memend) subend = memend;
                如果(substart >= subend) continue;

                /* if there is any overlap, forget this one and add
                 * 1-2 subranges back
                 */
                cbi->memmap[m].mm_base_addr = cbi->memmap[m].mm_length = 0;
                如果(substart > memaddr)
                        add_memmap(cbi, memaddr, substart-memaddr);
                如果(subend < memend)
                        add_memmap(cbi, subend, memend-subend);
        }
}

phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len)
{
	/* Allocate the lowest physical page we have. */
	整数 m;
#define EMPTY 0xffffffff
	phys_bytes lowest = EMPTY;
	assert(len > 0);
	len = roundup(len, I386_PAGE_SIZE);

	assert(kernel_may_alloc);

	为(m = 0; m < cbi->mmap_size; m++) {
		如果(cbi->memmap[m].mm_length < len) continue;
		如果(cbi->memmap[m].mm_base_addr < lowest) lowest = cbi->memmap[m].mm_base_addr;
	}
	assert(lowest != EMPTY);
	cut_memmap(cbi, lowest, len);
	cbi->kernel_allocated_bytes_dynamic += len;
	返回 lowest;
}

void add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
        整数 m;
#define LIMIT 0xFFFFF000
        /* Truncate available memory at 4GB as the rest of minix
         * currently can't deal with any bigger.
         */
        如果(addr > LIMIT) 返回;
        如果(addr + len > LIMIT) {
                len -= (addr + len - LIMIT);
        }
        assert(cbi->mmap_size < MAXMEMMAP);
        如果(len == 0) 返回;
	addr = roundup(addr, I386_PAGE_SIZE);
	len = rounddown(len, I386_PAGE_SIZE);

	assert(kernel_may_alloc);

        为(m = 0; m < MAXMEMMAP; m++) {
		phys_bytes highmark;
                如果(cbi->memmap[m].mm_length) continue;
                cbi->memmap[m].mm_base_addr = addr;
                cbi->memmap[m].mm_length = len;
                cbi->memmap[m].mm_type = MULTIBOOT_MEMORY_AVAILABLE;
                如果(m >= cbi->mmap_size)
                        cbi->mmap_size = m+1;
		highmark = addr + len;
		如果(highmark > cbi->mem_high_phys) {
			cbi->mem_high_phys = highmark;
		}

                返回;
        }

        panic("no available memmap slot");
}

u32_t *alloc_pagetable(phys_bytes *ph)
{
	u32_t *ret;
#define PG_PAGETABLES 6
	static u32_t pagetables[PG_PAGETABLES][1024]  __aligned(4096);
	static 整数 pt_inuse = 0;
	如果(pt_inuse >= PG_PAGETABLES) panic("no more pagetables");
	assert(sizeof(pagetables[pt_inuse]) == I386_PAGE_SIZE);
	ret = pagetables[pt_inuse++];
	*ph = vir2phys(ret);
	返回 ret;
}

#define PAGE_KB (I386_PAGE_SIZE / 1024)

phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	整数 m;
	multiboot_memory_map_t *mmap;

	assert(kernel_may_alloc);

	为(m = cbi->mmap_size-1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		如果(!mmap->mm_length) continue;
		assert(mmap->mm_length > 0);
		assert(!(mmap->mm_length % I386_PAGE_SIZE));
		assert(!(mmap->mm_base_addr % I386_PAGE_SIZE));

		mmap->mm_length -= I386_PAGE_SIZE;

                cbi->kernel_allocated_bytes_dynamic += I386_PAGE_SIZE;

		返回 mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

void pg_identity(kinfo_t *cbi)
{
	uint32_t i;
	phys_bytes phys;

	/* We map memory that does not correspond to physical memory
	 * as non-cacheable. Make sure we know what it is.
	 */
	assert(cbi->mem_high_phys);

        /* Set up an identity mapping page directory */
        为(i = 0; i < I386_VM_DIR_ENTRIES; i++) {
		u32_t flags = I386_VM_PRESENT | I386_VM_BIGPAGE
			| I386_VM_USER
			| I386_VM_WRITE;
                phys = i * I386_BIG_PAGE_SIZE;
		如果((cbi->mem_high_phys & I386_VM_ADDR_MASK_4MB)
			<= (phys & I386_VM_ADDR_MASK_4MB)) {
			flags |= I386_VM_PWT | I386_VM_PCD;
		}
                pagedir[i] =  phys | flags;
        }
}

整数 pg_mapkernel(void)
{
	整数 pde;
	u32_t mapped = 0, kern_phys = kern_phys_start;

        assert(!(kern_vir_start % I386_BIG_PAGE_SIZE));
        assert(!(kern_phys % I386_BIG_PAGE_SIZE));
        pde = kern_vir_start / I386_BIG_PAGE_SIZE; /* start pde */
	当(mapped < kern_kernlen) {
	        pagedir[pde] = kern_phys | I386_VM_PRESENT | 
			I386_VM_BIGPAGE | I386_VM_WRITE;
		mapped += I386_BIG_PAGE_SIZE;
		kern_phys += I386_BIG_PAGE_SIZE;
		pde++;
	}
	返回 pde;	/* free pde */
}

void vm_enable_paging(void)
{
        u32_t cr0, cr4;
        整数 pgeok;

        pgeok = _cpufeature(_CPUF_I386_PGE);

#ifdef PAE
	如果(_cpufeature(_CPUF_I386_PAE) == 0)
		panic("kernel built with PAE support, CPU seems to lack PAE support?\n");
#endif

        cr0= read_cr0();
        cr4= read_cr4();

	/* The boot loader should have put us in protected mode. */
	assert(cr0 & I386_CR0_PE);

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
        如果(pgeok)
                cr4 |= I386_CR4_PGE;

        write_cr4(cr4);
}

phys_bytes pg_load(void)
{
	phys_bytes phpagedir = vir2phys(pagedir);
        write_cr3(phpagedir);
	返回 phpagedir;
}

void pg_clear(void)
{
	memset(pagedir, 0, sizeof(pagedir));
}

phys_bytes pg_rounddown(phys_bytes b)
{
	phys_bytes o;
	如果(!(o = b % I386_PAGE_SIZE))
		返回 b;
	返回 b  - o;
}

void pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end,
	kinfo_t *cbi)
{
	static 整数 mapped_pde = -1;
	static u32_t *pt = NULL;
	整数 pde, pte;

	assert(kernel_may_alloc);

	如果(phys == PG_ALLOCATEME) {
		assert(!(vaddr % I386_PAGE_SIZE));
	} 否则  {
		assert((vaddr % I386_PAGE_SIZE) == (phys % I386_PAGE_SIZE));
		vaddr = pg_rounddown(vaddr);
		phys = pg_rounddown(phys);
	}	
	assert(vaddr < kern_vir_start);

	当(vaddr < vaddr_end) {
		phys_bytes source = phys;
		assert(!(vaddr % I386_PAGE_SIZE));
		如果(phys == PG_ALLOCATEME) {
			source = pg_alloc_page(cbi);
		} 否则 {
			assert(!(phys % I386_PAGE_SIZE));
		}
		assert(!(source % I386_PAGE_SIZE));
		pde = I386_VM_PDE(vaddr);
		pte = I386_VM_PTE(vaddr);
		如果(mapped_pde < pde) {
			phys_bytes ph;
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & I386_VM_ADDR_MASK)
		                | I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
			mapped_pde = pde;
		}
		assert(pt);
		pt[pte] = (source & I386_VM_ADDR_MASK) |
			I386_VM_PRESENT | I386_VM_USER | I386_VM_WRITE;
		vaddr += I386_PAGE_SIZE;
		如果(phys != PG_ALLOCATEME)
			phys += I386_PAGE_SIZE;
	}
}

void pg_info(reg_t *pagedir_ph, u32_t **pagedir_v)
{
	*pagedir_ph = vir2phys(pagedir);
	*pagedir_v = pagedir;
}

