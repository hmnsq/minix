#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#જો 0
/* check interrupt state */
સ્થિર અંતઃરેખ શૂન્ય check_int(unsigned પૂર્ણાંક state, પૂર્ણાંક લીટી)
{
	unsigned પૂર્ણાંક cpsr = 0;

	asm અસ્થિર("mrs %0, cpsr" : "=r" (cpsr));

	જો ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", લીટી, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	જો ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", લીટી, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
સ્થિર અંતઃરેખ શૂન્ય dmb(શૂન્ય)
{
	asm અસ્થિર("dmb" : : : "memory");
}

/* Data synchronization barrier */
સ્થિર અંતઃરેખ શૂન્ય dsb(શૂન્ય)
{
	asm અસ્થિર("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
સ્થિર અંતઃરેખ શૂન્ય isb(શૂન્ય)
{
	asm અસ્થિર("isb" : : : "memory");
}

સ્થિર અંતઃરેખ શૂન્ય barrier(શૂન્ય)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
સ્થિર અંતઃરેખ u32_t read_clidr(શૂન્ય){
	u32_t clidr;
	asm અસ્થિર("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	પાછા clidr;
}


/* Read CSSELR, Cache Size Selection Register */
સ્થિર અંતઃરેખ u32_t read_csselr(શૂન્ય){
	u32_t csselr;
	asm અસ્થિર("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	પાછા csselr;
}

/* Write CSSELR, Cache Size Selection Register */
સ્થિર અંતઃરેખ શૂન્ય write_csselr(u32_t csselr){
	asm અસ્થિર("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
સ્થિર અંતઃરેખ u32_t read_ccsidr(શૂન્ય)
{
	u32_t ccsidr;
	asm અસ્થિર("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	પાછા ccsidr;
}

/* Read TLBTR, TLB Type Register */
સ્થિર અંતઃરેખ u32_t read_tlbtr(શૂન્ય)
{
	u32_t tlbtr;
	asm અસ્થિર("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	પાછા tlbtr;
}

/* keesj:move these out */
સ્થિર અંતઃરેખ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	જ્યારે( (t = t >> 1) ) counter ++;
	પાછા counter;
}

/* keesj:move these out */
સ્થિર અંતઃરેખ u32_t ipow2(u32_t t)
{
	પાછા 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
સ્થિર અંતઃરેખ શૂન્ય dcache_maint(પૂર્ણાંક type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  લીટી_size,લીટી_length;
	u32_t  સંખ્યા_of_sets,સંખ્યા_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	માટે (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		લીટી_size = ccsidr & 0x7;
		લીટી_length = 2 << (લીટી_size + 1) ; /* 2**(line_size + 2) */
		સંખ્યા_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		સંખ્યા_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(સંખ્યા_of_ways);
		જો(ipow2(ilog2(સંખ્યા_of_ways) < સંખ્યા_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(લીટી_length);
		માટે (way =0 ; way < સંખ્યા_of_ways; way++) {
			માટે (set =0 ; set < સંખ્યા_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				જો (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm અસ્થિર("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} નહીંતર જો (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm અસ્થિર("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
સ્થિર અંતઃરેખ શૂન્ય dcache_clean(શૂન્ય){
	dcache_maint(1);
}
સ્થિર અંતઃરેખ શૂન્ય dcache_invalidate(શૂન્ય){
	dcache_maint(2);
}

સ્થિર અંતઃરેખ શૂન્ય refresh_tlb(શૂન્ય)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm અસ્થિર("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#જો 0
	/* Invalidate entire data TLB */
	asm અસ્થિર("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm અસ્થિર("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm અસ્થિર("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm અસ્થિર("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
સ્થિર અંતઃરેખ u32_t read_sctlr(શૂન્ય)
{
	u32_t ctl;

	asm અસ્થિર("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	પાછા ctl;
}

/* Write System Control Register */
સ્થિર અંતઃરેખ શૂન્ય write_sctlr(u32_t ctl)
{
	asm અસ્થિર("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
સ્થિર અંતઃરેખ u32_t read_ttbr0(શૂન્ય)
{
	u32_t bar;

	asm અસ્થિર("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	પાછા bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
સ્થિર અંતઃરેખ શૂન્ય write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm અસ્થિર("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
સ્થિર અંતઃરેખ શૂન્ય reload_ttbr0(શૂન્ય)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
સ્થિર અંતઃરેખ u32_t read_ttbr1(શૂન્ય)
{
	u32_t bar;

	asm અસ્થિર("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	પાછા bar;
}

/* Write Translation Table Base Register 1 */
સ્થિર અંતઃરેખ શૂન્ય write_ttbr1(u32_t bar)
{
	barrier();

	asm અસ્થિર("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
સ્થિર અંતઃરેખ શૂન્ય reload_ttbr1(શૂન્ય)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
સ્થિર અંતઃરેખ u32_t read_ttbcr(શૂન્ય)
{
	u32_t bcr;

	asm અસ્થિર("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	પાછા bcr;
}

/* Write Translation Table Base Control Register */
સ્થિર અંતઃરેખ શૂન્ય write_ttbcr(u32_t bcr)
{
	asm અસ્થિર("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
સ્થિર અંતઃરેખ u32_t read_dacr(શૂન્ય)
{
	u32_t dacr;

	asm અસ્થિર("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	પાછા dacr;
}

/* Write Domain Access Control Register */
સ્થિર અંતઃરેખ શૂન્ય write_dacr(u32_t dacr)
{
	asm અસ્થિર("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
સ્થિર અંતઃરેખ u32_t read_dfsr(શૂન્ય)
{
	u32_t fsr;

	asm અસ્થિર("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	પાછા fsr;
}

/* Write Data Fault Status Register */
સ્થિર અંતઃરેખ શૂન્ય write_dfsr(u32_t fsr)
{
	asm અસ્થિર("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
સ્થિર અંતઃરેખ u32_t read_ifsr(શૂન્ય)
{
	u32_t fsr;

	asm અસ્થિર("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	પાછા fsr;
}

/* Write Instruction Fault Status Register */
સ્થિર અંતઃરેખ શૂન્ય write_ifsr(u32_t fsr)
{
	asm અસ્થિર("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
સ્થિર અંતઃરેખ u32_t read_dfar(શૂન્ય)
{
	u32_t far;

	asm અસ્થિર("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	પાછા far;
}

/* Write Data Fault Address Register */
સ્થિર અંતઃરેખ શૂન્ય write_dfar(u32_t far)
{
	asm અસ્થિર("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
સ્થિર અંતઃરેખ u32_t read_ifar(શૂન્ય)
{
	u32_t far;

	asm અસ્થિર("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	પાછા far;
}

/* Write Instruction Fault Address Register */
સ્થિર અંતઃરેખ શૂન્ય write_ifar(u32_t far)
{
	asm અસ્થિર("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
સ્થિર અંતઃરેખ u32_t read_vbar(શૂન્ય)
{
	u32_t vbar;

	asm અસ્થિર("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	પાછા vbar;
}

/* Write Vector Base Address Register */
સ્થિર અંતઃરેખ શૂન્ય write_vbar(u32_t vbar)
{
	asm અસ્થિર("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
સ્થિર અંતઃરેખ u32_t read_midr(શૂન્ય)
{
	u32_t id;

	asm અસ્થિર("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	પાછા id;
}

/* Read Auxiliary Control Register */
સ્થિર અંતઃરેખ u32_t read_actlr(શૂન્ય)
{
	u32_t ctl;

	asm અસ્થિર("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	પાછા ctl;
}

/* Write Auxiliary Control Register */
સ્થિર અંતઃરેખ શૂન્ય write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm અસ્થિર("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
સ્થિર અંતઃરેખ u32_t read_cpsr(શૂન્ય)
{
	u32_t status;

	asm અસ્થિર("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	પાછા status;
}

/* Write Current Program Status Register */
સ્થિર અંતઃરેખ શૂન્ય write_cpsr(u32_t status)
{
	asm અસ્થિર("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
