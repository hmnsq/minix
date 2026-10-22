#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#ಯದಿ 0
/* check interrupt state */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ check_int(unsigned ಪೂರ್ಣಾಙ್ಕ state, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ cpsr = 0;

	asm ಚಞ್ಚಲ("mrs %0, cpsr" : "=r" (cpsr));

	ಯದಿ ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", ಸಾಲು, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	ಯದಿ ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", ಸಾಲು, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dmb(ಶೂನ್ಯ)
{
	asm ಚಞ್ಚಲ("dmb" : : : "memory");
}

/* Data synchronization barrier */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dsb(ಶೂನ್ಯ)
{
	asm ಚಞ್ಚಲ("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ isb(ಶೂನ್ಯ)
{
	asm ಚಞ್ಚಲ("isb" : : : "memory");
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ barrier(ಶೂನ್ಯ)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_clidr(ಶೂನ್ಯ){
	u32_t clidr;
	asm ಚಞ್ಚಲ("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ಪ್ರತ್ಯಾಗಮ clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_csselr(ಶೂನ್ಯ){
	u32_t csselr;
	asm ಚಞ್ಚಲ("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ಪ್ರತ್ಯಾಗಮ csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_csselr(u32_t csselr){
	asm ಚಞ್ಚಲ("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ccsidr(ಶೂನ್ಯ)
{
	u32_t ccsidr;
	asm ಚಞ್ಚಲ("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ಪ್ರತ್ಯಾಗಮ ccsidr;
}

/* Read TLBTR, TLB Type Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_tlbtr(ಶೂನ್ಯ)
{
	u32_t tlbtr;
	asm ಚಞ್ಚಲ("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ಪ್ರತ್ಯಾಗಮ tlbtr;
}

/* keesj:move these out */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	ಯಾವತ್( (t = t >> 1) ) counter ++;
	ಪ್ರತ್ಯಾಗಮ counter;
}

/* keesj:move these out */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t ipow2(u32_t t)
{
	ಪ್ರತ್ಯಾಗಮ 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dcache_maint(ಪೂರ್ಣಾಙ್ಕ type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  ಸಾಲು_size,ಸಾಲು_length;
	u32_t  ಸಂಖ್ಯೆ_of_sets,ಸಂಖ್ಯೆ_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	ಪ್ರತಿ (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		ಸಾಲು_size = ccsidr & 0x7;
		ಸಾಲು_length = 2 << (ಸಾಲು_size + 1) ; /* 2**(line_size + 2) */
		ಸಂಖ್ಯೆ_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		ಸಂಖ್ಯೆ_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(ಸಂಖ್ಯೆ_of_ways);
		ಯದಿ(ipow2(ilog2(ಸಂಖ್ಯೆ_of_ways) < ಸಂಖ್ಯೆ_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(ಸಾಲು_length);
		ಪ್ರತಿ (way =0 ; way < ಸಂಖ್ಯೆ_of_ways; way++) {
			ಪ್ರತಿ (set =0 ; set < ಸಂಖ್ಯೆ_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				ಯದಿ (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm ಚಞ್ಚಲ("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} ಅನ್ಯಥಾ ಯದಿ (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm ಚಞ್ಚಲ("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dcache_clean(ಶೂನ್ಯ){
	dcache_maint(1);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dcache_invalidate(ಶೂನ್ಯ){
	dcache_maint(2);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ refresh_tlb(ಶೂನ್ಯ)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm ಚಞ್ಚಲ("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#ಯದಿ 0
	/* Invalidate entire data TLB */
	asm ಚಞ್ಚಲ("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm ಚಞ್ಚಲ("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm ಚಞ್ಚಲ("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm ಚಞ್ಚಲ("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_sctlr(ಶೂನ್ಯ)
{
	u32_t ctl;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ಪ್ರತ್ಯಾಗಮ ctl;
}

/* Write System Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_sctlr(u32_t ctl)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ttbr0(ಶೂನ್ಯ)
{
	u32_t bar;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ಪ್ರತ್ಯಾಗಮ bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm ಚಞ್ಚಲ("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ reload_ttbr0(ಶೂನ್ಯ)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ttbr1(ಶೂನ್ಯ)
{
	u32_t bar;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ಪ್ರತ್ಯಾಗಮ bar;
}

/* Write Translation Table Base Register 1 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_ttbr1(u32_t bar)
{
	barrier();

	asm ಚಞ್ಚಲ("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ reload_ttbr1(ಶೂನ್ಯ)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ttbcr(ಶೂನ್ಯ)
{
	u32_t bcr;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ಪ್ರತ್ಯಾಗಮ bcr;
}

/* Write Translation Table Base Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_ttbcr(u32_t bcr)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_dacr(ಶೂನ್ಯ)
{
	u32_t dacr;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ಪ್ರತ್ಯಾಗಮ dacr;
}

/* Write Domain Access Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_dacr(u32_t dacr)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_dfsr(ಶೂನ್ಯ)
{
	u32_t fsr;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ಪ್ರತ್ಯಾಗಮ fsr;
}

/* Write Data Fault Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_dfsr(u32_t fsr)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ifsr(ಶೂನ್ಯ)
{
	u32_t fsr;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ಪ್ರತ್ಯಾಗಮ fsr;
}

/* Write Instruction Fault Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_ifsr(u32_t fsr)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_dfar(ಶೂನ್ಯ)
{
	u32_t far;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ಪ್ರತ್ಯಾಗಮ far;
}

/* Write Data Fault Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_dfar(u32_t far)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_ifar(ಶೂನ್ಯ)
{
	u32_t far;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ಪ್ರತ್ಯಾಗಮ far;
}

/* Write Instruction Fault Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_ifar(u32_t far)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_vbar(ಶೂನ್ಯ)
{
	u32_t vbar;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ಪ್ರತ್ಯಾಗಮ vbar;
}

/* Write Vector Base Address Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_vbar(u32_t vbar)
{
	asm ಚಞ್ಚಲ("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_midr(ಶೂನ್ಯ)
{
	u32_t id;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ಪ್ರತ್ಯಾಗಮ id;
}

/* Read Auxiliary Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_actlr(ಶೂನ್ಯ)
{
	u32_t ctl;

	asm ಚಞ್ಚಲ("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ಪ್ರತ್ಯಾಗಮ ctl;
}

/* Write Auxiliary Control Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm ಚಞ್ಚಲ("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32_t read_cpsr(ಶೂನ್ಯ)
{
	u32_t status;

	asm ಚಞ್ಚಲ("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	ಪ್ರತ್ಯಾಗಮ status;
}

/* Write Current Program Status Register */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_cpsr(u32_t status)
{
	asm ಚಞ್ಚಲ("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
