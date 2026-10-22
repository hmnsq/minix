#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#اگر 0
/* check interrupt state */
ایستا درونخطی تهی check_int(unsigned صحیح state, صحیح سطر)
{
	unsigned صحیح cpsr = 0;

	asm فرار("mrs %0, cpsr" : "=r" (cpsr));

	اگر ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", سطر, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	اگر ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", سطر, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
ایستا درونخطی تهی dmb(تهی)
{
	asm فرار("dmb" : : : "memory");
}

/* Data synchronization barrier */
ایستا درونخطی تهی dsb(تهی)
{
	asm فرار("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
ایستا درونخطی تهی isb(تهی)
{
	asm فرار("isb" : : : "memory");
}

ایستا درونخطی تهی barrier(تهی)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
ایستا درونخطی u32_t read_clidr(تهی){
	u32_t clidr;
	asm فرار("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	بازگشت clidr;
}


/* Read CSSELR, Cache Size Selection Register */
ایستا درونخطی u32_t read_csselr(تهی){
	u32_t csselr;
	asm فرار("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	بازگشت csselr;
}

/* Write CSSELR, Cache Size Selection Register */
ایستا درونخطی تهی write_csselr(u32_t csselr){
	asm فرار("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
ایستا درونخطی u32_t read_ccsidr(تهی)
{
	u32_t ccsidr;
	asm فرار("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	بازگشت ccsidr;
}

/* Read TLBTR, TLB Type Register */
ایستا درونخطی u32_t read_tlbtr(تهی)
{
	u32_t tlbtr;
	asm فرار("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	بازگشت tlbtr;
}

/* keesj:move these out */
ایستا درونخطی u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	وقتی( (t = t >> 1) ) counter ++;
	بازگشت counter;
}

/* keesj:move these out */
ایستا درونخطی u32_t ipow2(u32_t t)
{
	بازگشت 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
ایستا درونخطی تهی dcache_maint(صحیح type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  سطر_size,سطر_length;
	u32_t  عدد_of_sets,عدد_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	برای (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		سطر_size = ccsidr & 0x7;
		سطر_length = 2 << (سطر_size + 1) ; /* 2**(line_size + 2) */
		عدد_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		عدد_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(عدد_of_ways);
		اگر(ipow2(ilog2(عدد_of_ways) < عدد_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(سطر_length);
		برای (way =0 ; way < عدد_of_ways; way++) {
			برای (set =0 ; set < عدد_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				اگر (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm فرار("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} وگرنه اگر (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm فرار("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
ایستا درونخطی تهی dcache_clean(تهی){
	dcache_maint(1);
}
ایستا درونخطی تهی dcache_invalidate(تهی){
	dcache_maint(2);
}

ایستا درونخطی تهی refresh_tlb(تهی)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm فرار("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#اگر 0
	/* Invalidate entire data TLB */
	asm فرار("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm فرار("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm فرار("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm فرار("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
ایستا درونخطی u32_t read_sctlr(تهی)
{
	u32_t ctl;

	asm فرار("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	بازگشت ctl;
}

/* Write System Control Register */
ایستا درونخطی تهی write_sctlr(u32_t ctl)
{
	asm فرار("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
ایستا درونخطی u32_t read_ttbr0(تهی)
{
	u32_t bar;

	asm فرار("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	بازگشت bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
ایستا درونخطی تهی write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm فرار("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
ایستا درونخطی تهی reload_ttbr0(تهی)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
ایستا درونخطی u32_t read_ttbr1(تهی)
{
	u32_t bar;

	asm فرار("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	بازگشت bar;
}

/* Write Translation Table Base Register 1 */
ایستا درونخطی تهی write_ttbr1(u32_t bar)
{
	barrier();

	asm فرار("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
ایستا درونخطی تهی reload_ttbr1(تهی)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
ایستا درونخطی u32_t read_ttbcr(تهی)
{
	u32_t bcr;

	asm فرار("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	بازگشت bcr;
}

/* Write Translation Table Base Control Register */
ایستا درونخطی تهی write_ttbcr(u32_t bcr)
{
	asm فرار("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
ایستا درونخطی u32_t read_dacr(تهی)
{
	u32_t dacr;

	asm فرار("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	بازگشت dacr;
}

/* Write Domain Access Control Register */
ایستا درونخطی تهی write_dacr(u32_t dacr)
{
	asm فرار("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
ایستا درونخطی u32_t read_dfsr(تهی)
{
	u32_t fsr;

	asm فرار("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	بازگشت fsr;
}

/* Write Data Fault Status Register */
ایستا درونخطی تهی write_dfsr(u32_t fsr)
{
	asm فرار("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
ایستا درونخطی u32_t read_ifsr(تهی)
{
	u32_t fsr;

	asm فرار("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	بازگشت fsr;
}

/* Write Instruction Fault Status Register */
ایستا درونخطی تهی write_ifsr(u32_t fsr)
{
	asm فرار("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
ایستا درونخطی u32_t read_dfar(تهی)
{
	u32_t far;

	asm فرار("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	بازگشت far;
}

/* Write Data Fault Address Register */
ایستا درونخطی تهی write_dfar(u32_t far)
{
	asm فرار("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
ایستا درونخطی u32_t read_ifar(تهی)
{
	u32_t far;

	asm فرار("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	بازگشت far;
}

/* Write Instruction Fault Address Register */
ایستا درونخطی تهی write_ifar(u32_t far)
{
	asm فرار("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
ایستا درونخطی u32_t read_vbar(تهی)
{
	u32_t vbar;

	asm فرار("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	بازگشت vbar;
}

/* Write Vector Base Address Register */
ایستا درونخطی تهی write_vbar(u32_t vbar)
{
	asm فرار("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
ایستا درونخطی u32_t read_midr(تهی)
{
	u32_t id;

	asm فرار("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	بازگشت id;
}

/* Read Auxiliary Control Register */
ایستا درونخطی u32_t read_actlr(تهی)
{
	u32_t ctl;

	asm فرار("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	بازگشت ctl;
}

/* Write Auxiliary Control Register */
ایستا درونخطی تهی write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm فرار("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
ایستا درونخطی u32_t read_cpsr(تهی)
{
	u32_t status;

	asm فرار("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	بازگشت status;
}

/* Write Current Program Status Register */
ایستا درونخطی تهی write_cpsr(u32_t status)
{
	asm فرار("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
