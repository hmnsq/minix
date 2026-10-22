#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#ଯଦି 0
/* check interrupt state */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ check_int(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpsr = 0;

	asm ଅସ୍ଥିର("mrs %0, cpsr" : "=r" (cpsr));

	ଯଦି ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", ଧାଡ଼ି, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	ଯଦି ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", ଧାଡ଼ି, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ dmb(ଶୂନ୍ୟ)
{
	asm ଅସ୍ଥିର("dmb" : : : "memory");
}

/* Data synchronization barrier */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ dsb(ଶୂନ୍ୟ)
{
	asm ଅସ୍ଥିର("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ isb(ଶୂନ୍ୟ)
{
	asm ଅସ୍ଥିର("isb" : : : "memory");
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ barrier(ଶୂନ୍ୟ)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_clidr(ଶୂନ୍ୟ){
	u32_t clidr;
	asm ଅସ୍ଥିର("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ଫେରନ୍ତୁ clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_csselr(ଶୂନ୍ୟ){
	u32_t csselr;
	asm ଅସ୍ଥିର("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ଫେରନ୍ତୁ csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_csselr(u32_t csselr){
	asm ଅସ୍ଥିର("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ccsidr(ଶୂନ୍ୟ)
{
	u32_t ccsidr;
	asm ଅସ୍ଥିର("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ଫେରନ୍ତୁ ccsidr;
}

/* Read TLBTR, TLB Type Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_tlbtr(ଶୂନ୍ୟ)
{
	u32_t tlbtr;
	asm ଅସ୍ଥିର("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ଫେରନ୍ତୁ tlbtr;
}

/* keesj:move these out */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	ଯେତେବେଳେ( (t = t >> 1) ) counter ++;
	ଫେରନ୍ତୁ counter;
}

/* keesj:move these out */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t ipow2(u32_t t)
{
	ଫେରନ୍ତୁ 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ dcache_maint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  ଧାଡ଼ି_size,ଧାଡ଼ି_length;
	u32_t  ସଂଖ୍ୟା_of_sets,ସଂଖ୍ୟା_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	ପାଇଁ (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		ଧାଡ଼ି_size = ccsidr & 0x7;
		ଧାଡ଼ି_length = 2 << (ଧାଡ଼ି_size + 1) ; /* 2**(line_size + 2) */
		ସଂଖ୍ୟା_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		ସଂଖ୍ୟା_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(ସଂଖ୍ୟା_of_ways);
		ଯଦି(ipow2(ilog2(ସଂଖ୍ୟା_of_ways) < ସଂଖ୍ୟା_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(ଧାଡ଼ି_length);
		ପାଇଁ (way =0 ; way < ସଂଖ୍ୟା_of_ways; way++) {
			ପାଇଁ (set =0 ; set < ସଂଖ୍ୟା_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				ଯଦି (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm ଅସ୍ଥିର("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} ଅନ୍ୟଥା ଯଦି (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm ଅସ୍ଥିର("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ dcache_clean(ଶୂନ୍ୟ){
	dcache_maint(1);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ dcache_invalidate(ଶୂନ୍ୟ){
	dcache_maint(2);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ refresh_tlb(ଶୂନ୍ୟ)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm ଅସ୍ଥିର("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#ଯଦି 0
	/* Invalidate entire data TLB */
	asm ଅସ୍ଥିର("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm ଅସ୍ଥିର("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm ଅସ୍ଥିର("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm ଅସ୍ଥିର("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_sctlr(ଶୂନ୍ୟ)
{
	u32_t ctl;

	asm ଅସ୍ଥିର("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ଫେରନ୍ତୁ ctl;
}

/* Write System Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_sctlr(u32_t ctl)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ttbr0(ଶୂନ୍ୟ)
{
	u32_t bar;

	asm ଅସ୍ଥିର("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ଫେରନ୍ତୁ bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm ଅସ୍ଥିର("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ reload_ttbr0(ଶୂନ୍ୟ)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ttbr1(ଶୂନ୍ୟ)
{
	u32_t bar;

	asm ଅସ୍ଥିର("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ଫେରନ୍ତୁ bar;
}

/* Write Translation Table Base Register 1 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_ttbr1(u32_t bar)
{
	barrier();

	asm ଅସ୍ଥିର("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ reload_ttbr1(ଶୂନ୍ୟ)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ttbcr(ଶୂନ୍ୟ)
{
	u32_t bcr;

	asm ଅସ୍ଥିର("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ଫେରନ୍ତୁ bcr;
}

/* Write Translation Table Base Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_ttbcr(u32_t bcr)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_dacr(ଶୂନ୍ୟ)
{
	u32_t dacr;

	asm ଅସ୍ଥିର("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ଫେରନ୍ତୁ dacr;
}

/* Write Domain Access Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_dacr(u32_t dacr)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_dfsr(ଶୂନ୍ୟ)
{
	u32_t fsr;

	asm ଅସ୍ଥିର("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ଫେରନ୍ତୁ fsr;
}

/* Write Data Fault Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_dfsr(u32_t fsr)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ifsr(ଶୂନ୍ୟ)
{
	u32_t fsr;

	asm ଅସ୍ଥିର("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ଫେରନ୍ତୁ fsr;
}

/* Write Instruction Fault Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_ifsr(u32_t fsr)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_dfar(ଶୂନ୍ୟ)
{
	u32_t far;

	asm ଅସ୍ଥିର("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ଫେରନ୍ତୁ far;
}

/* Write Data Fault Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_dfar(u32_t far)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_ifar(ଶୂନ୍ୟ)
{
	u32_t far;

	asm ଅସ୍ଥିର("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ଫେରନ୍ତୁ far;
}

/* Write Instruction Fault Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_ifar(u32_t far)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_vbar(ଶୂନ୍ୟ)
{
	u32_t vbar;

	asm ଅସ୍ଥିର("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ଫେରନ୍ତୁ vbar;
}

/* Write Vector Base Address Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_vbar(u32_t vbar)
{
	asm ଅସ୍ଥିର("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_midr(ଶୂନ୍ୟ)
{
	u32_t id;

	asm ଅସ୍ଥିର("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ଫେରନ୍ତୁ id;
}

/* Read Auxiliary Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_actlr(ଶୂନ୍ୟ)
{
	u32_t ctl;

	asm ଅସ୍ଥିର("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ଫେରନ୍ତୁ ctl;
}

/* Write Auxiliary Control Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm ଅସ୍ଥିର("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ u32_t read_cpsr(ଶୂନ୍ୟ)
{
	u32_t status;

	asm ଅସ୍ଥିର("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	ଫେରନ୍ତୁ status;
}

/* Write Current Program Status Register */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_cpsr(u32_t status)
{
	asm ଅସ୍ଥିର("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
