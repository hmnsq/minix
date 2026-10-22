#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#எனில் 0
/* check interrupt state */
நிலையான உள்வரி வெறுமை check_int(unsigned முழு state, முழு வரி)
{
	unsigned முழு cpsr = 0;

	asm நிலையற்ற("mrs %0, cpsr" : "=r" (cpsr));

	எனில் ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", வரி, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	எனில் ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", வரி, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
நிலையான உள்வரி வெறுமை dmb(வெறுமை)
{
	asm நிலையற்ற("dmb" : : : "memory");
}

/* Data synchronization barrier */
நிலையான உள்வரி வெறுமை dsb(வெறுமை)
{
	asm நிலையற்ற("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
நிலையான உள்வரி வெறுமை isb(வெறுமை)
{
	asm நிலையற்ற("isb" : : : "memory");
}

நிலையான உள்வரி வெறுமை barrier(வெறுமை)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
நிலையான உள்வரி u32_t read_clidr(வெறுமை){
	u32_t clidr;
	asm நிலையற்ற("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	திரும்பு clidr;
}


/* Read CSSELR, Cache Size Selection Register */
நிலையான உள்வரி u32_t read_csselr(வெறுமை){
	u32_t csselr;
	asm நிலையற்ற("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	திரும்பு csselr;
}

/* Write CSSELR, Cache Size Selection Register */
நிலையான உள்வரி வெறுமை write_csselr(u32_t csselr){
	asm நிலையற்ற("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
நிலையான உள்வரி u32_t read_ccsidr(வெறுமை)
{
	u32_t ccsidr;
	asm நிலையற்ற("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	திரும்பு ccsidr;
}

/* Read TLBTR, TLB Type Register */
நிலையான உள்வரி u32_t read_tlbtr(வெறுமை)
{
	u32_t tlbtr;
	asm நிலையற்ற("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	திரும்பு tlbtr;
}

/* keesj:move these out */
நிலையான உள்வரி u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	போது( (t = t >> 1) ) counter ++;
	திரும்பு counter;
}

/* keesj:move these out */
நிலையான உள்வரி u32_t ipow2(u32_t t)
{
	திரும்பு 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
நிலையான உள்வரி வெறுமை dcache_maint(முழு type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  வரி_size,வரி_length;
	u32_t  எண்_of_sets,எண்_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	க்கு (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		வரி_size = ccsidr & 0x7;
		வரி_length = 2 << (வரி_size + 1) ; /* 2**(line_size + 2) */
		எண்_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		எண்_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(எண்_of_ways);
		எனில்(ipow2(ilog2(எண்_of_ways) < எண்_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(வரி_length);
		க்கு (way =0 ; way < எண்_of_ways; way++) {
			க்கு (set =0 ; set < எண்_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				எனில் (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm நிலையற்ற("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} இல்லையெனில் எனில் (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm நிலையற்ற("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
நிலையான உள்வரி வெறுமை dcache_clean(வெறுமை){
	dcache_maint(1);
}
நிலையான உள்வரி வெறுமை dcache_invalidate(வெறுமை){
	dcache_maint(2);
}

நிலையான உள்வரி வெறுமை refresh_tlb(வெறுமை)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm நிலையற்ற("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#எனில் 0
	/* Invalidate entire data TLB */
	asm நிலையற்ற("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm நிலையற்ற("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm நிலையற்ற("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm நிலையற்ற("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
நிலையான உள்வரி u32_t read_sctlr(வெறுமை)
{
	u32_t ctl;

	asm நிலையற்ற("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	திரும்பு ctl;
}

/* Write System Control Register */
நிலையான உள்வரி வெறுமை write_sctlr(u32_t ctl)
{
	asm நிலையற்ற("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
நிலையான உள்வரி u32_t read_ttbr0(வெறுமை)
{
	u32_t bar;

	asm நிலையற்ற("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	திரும்பு bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
நிலையான உள்வரி வெறுமை write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm நிலையற்ற("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
நிலையான உள்வரி வெறுமை reload_ttbr0(வெறுமை)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
நிலையான உள்வரி u32_t read_ttbr1(வெறுமை)
{
	u32_t bar;

	asm நிலையற்ற("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	திரும்பு bar;
}

/* Write Translation Table Base Register 1 */
நிலையான உள்வரி வெறுமை write_ttbr1(u32_t bar)
{
	barrier();

	asm நிலையற்ற("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
நிலையான உள்வரி வெறுமை reload_ttbr1(வெறுமை)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
நிலையான உள்வரி u32_t read_ttbcr(வெறுமை)
{
	u32_t bcr;

	asm நிலையற்ற("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	திரும்பு bcr;
}

/* Write Translation Table Base Control Register */
நிலையான உள்வரி வெறுமை write_ttbcr(u32_t bcr)
{
	asm நிலையற்ற("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
நிலையான உள்வரி u32_t read_dacr(வெறுமை)
{
	u32_t dacr;

	asm நிலையற்ற("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	திரும்பு dacr;
}

/* Write Domain Access Control Register */
நிலையான உள்வரி வெறுமை write_dacr(u32_t dacr)
{
	asm நிலையற்ற("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
நிலையான உள்வரி u32_t read_dfsr(வெறுமை)
{
	u32_t fsr;

	asm நிலையற்ற("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	திரும்பு fsr;
}

/* Write Data Fault Status Register */
நிலையான உள்வரி வெறுமை write_dfsr(u32_t fsr)
{
	asm நிலையற்ற("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
நிலையான உள்வரி u32_t read_ifsr(வெறுமை)
{
	u32_t fsr;

	asm நிலையற்ற("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	திரும்பு fsr;
}

/* Write Instruction Fault Status Register */
நிலையான உள்வரி வெறுமை write_ifsr(u32_t fsr)
{
	asm நிலையற்ற("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
நிலையான உள்வரி u32_t read_dfar(வெறுமை)
{
	u32_t far;

	asm நிலையற்ற("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	திரும்பு far;
}

/* Write Data Fault Address Register */
நிலையான உள்வரி வெறுமை write_dfar(u32_t far)
{
	asm நிலையற்ற("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
நிலையான உள்வரி u32_t read_ifar(வெறுமை)
{
	u32_t far;

	asm நிலையற்ற("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	திரும்பு far;
}

/* Write Instruction Fault Address Register */
நிலையான உள்வரி வெறுமை write_ifar(u32_t far)
{
	asm நிலையற்ற("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
நிலையான உள்வரி u32_t read_vbar(வெறுமை)
{
	u32_t vbar;

	asm நிலையற்ற("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	திரும்பு vbar;
}

/* Write Vector Base Address Register */
நிலையான உள்வரி வெறுமை write_vbar(u32_t vbar)
{
	asm நிலையற்ற("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
நிலையான உள்வரி u32_t read_midr(வெறுமை)
{
	u32_t id;

	asm நிலையற்ற("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	திரும்பு id;
}

/* Read Auxiliary Control Register */
நிலையான உள்வரி u32_t read_actlr(வெறுமை)
{
	u32_t ctl;

	asm நிலையற்ற("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	திரும்பு ctl;
}

/* Write Auxiliary Control Register */
நிலையான உள்வரி வெறுமை write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm நிலையற்ற("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
நிலையான உள்வரி u32_t read_cpsr(வெறுமை)
{
	u32_t status;

	asm நிலையற்ற("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	திரும்பு status;
}

/* Write Current Program Status Register */
நிலையான உள்வரி வெறுமை write_cpsr(u32_t status)
{
	asm நிலையற்ற("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
