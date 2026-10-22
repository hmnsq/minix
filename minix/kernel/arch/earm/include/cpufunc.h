#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#എങ്കിൽ 0
/* check interrupt state */
സ്ഥിതം വരിയിൽ ശൂന്യം check_int(unsigned പൂർണ്ണസംഖ്യ state, പൂർണ്ണസംഖ്യ വരി)
{
	unsigned പൂർണ്ണസംഖ്യ cpsr = 0;

	asm അസ്ഥിര("mrs %0, cpsr" : "=r" (cpsr));

	എങ്കിൽ ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", വരി, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	എങ്കിൽ ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", വരി, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
സ്ഥിതം വരിയിൽ ശൂന്യം dmb(ശൂന്യം)
{
	asm അസ്ഥിര("dmb" : : : "memory");
}

/* Data synchronization barrier */
സ്ഥിതം വരിയിൽ ശൂന്യം dsb(ശൂന്യം)
{
	asm അസ്ഥിര("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
സ്ഥിതം വരിയിൽ ശൂന്യം isb(ശൂന്യം)
{
	asm അസ്ഥിര("isb" : : : "memory");
}

സ്ഥിതം വരിയിൽ ശൂന്യം barrier(ശൂന്യം)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
സ്ഥിതം വരിയിൽ u32_t read_clidr(ശൂന്യം){
	u32_t clidr;
	asm അസ്ഥിര("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	തിരികെ clidr;
}


/* Read CSSELR, Cache Size Selection Register */
സ്ഥിതം വരിയിൽ u32_t read_csselr(ശൂന്യം){
	u32_t csselr;
	asm അസ്ഥിര("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	തിരികെ csselr;
}

/* Write CSSELR, Cache Size Selection Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_csselr(u32_t csselr){
	asm അസ്ഥിര("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
സ്ഥിതം വരിയിൽ u32_t read_ccsidr(ശൂന്യം)
{
	u32_t ccsidr;
	asm അസ്ഥിര("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	തിരികെ ccsidr;
}

/* Read TLBTR, TLB Type Register */
സ്ഥിതം വരിയിൽ u32_t read_tlbtr(ശൂന്യം)
{
	u32_t tlbtr;
	asm അസ്ഥിര("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	തിരികെ tlbtr;
}

/* keesj:move these out */
സ്ഥിതം വരിയിൽ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	ആകുമ്പോൾ( (t = t >> 1) ) counter ++;
	തിരികെ counter;
}

/* keesj:move these out */
സ്ഥിതം വരിയിൽ u32_t ipow2(u32_t t)
{
	തിരികെ 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
സ്ഥിതം വരിയിൽ ശൂന്യം dcache_maint(പൂർണ്ണസംഖ്യ type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  വരി_size,വരി_length;
	u32_t  സംഖ്യ_of_sets,സംഖ്യ_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	വേണ്ടി (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		വരി_size = ccsidr & 0x7;
		വരി_length = 2 << (വരി_size + 1) ; /* 2**(line_size + 2) */
		സംഖ്യ_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		സംഖ്യ_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(സംഖ്യ_of_ways);
		എങ്കിൽ(ipow2(ilog2(സംഖ്യ_of_ways) < സംഖ്യ_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(വരി_length);
		വേണ്ടി (way =0 ; way < സംഖ്യ_of_ways; way++) {
			വേണ്ടി (set =0 ; set < സംഖ്യ_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				എങ്കിൽ (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm അസ്ഥിര("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} അല്ലെങ്കിൽ എങ്കിൽ (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm അസ്ഥിര("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
സ്ഥിതം വരിയിൽ ശൂന്യം dcache_clean(ശൂന്യം){
	dcache_maint(1);
}
സ്ഥിതം വരിയിൽ ശൂന്യം dcache_invalidate(ശൂന്യം){
	dcache_maint(2);
}

സ്ഥിതം വരിയിൽ ശൂന്യം refresh_tlb(ശൂന്യം)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm അസ്ഥിര("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#എങ്കിൽ 0
	/* Invalidate entire data TLB */
	asm അസ്ഥിര("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm അസ്ഥിര("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm അസ്ഥിര("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm അസ്ഥിര("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
സ്ഥിതം വരിയിൽ u32_t read_sctlr(ശൂന്യം)
{
	u32_t ctl;

	asm അസ്ഥിര("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	തിരികെ ctl;
}

/* Write System Control Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_sctlr(u32_t ctl)
{
	asm അസ്ഥിര("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
സ്ഥിതം വരിയിൽ u32_t read_ttbr0(ശൂന്യം)
{
	u32_t bar;

	asm അസ്ഥിര("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	തിരികെ bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm അസ്ഥിര("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
സ്ഥിതം വരിയിൽ ശൂന്യം reload_ttbr0(ശൂന്യം)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
സ്ഥിതം വരിയിൽ u32_t read_ttbr1(ശൂന്യം)
{
	u32_t bar;

	asm അസ്ഥിര("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	തിരികെ bar;
}

/* Write Translation Table Base Register 1 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_ttbr1(u32_t bar)
{
	barrier();

	asm അസ്ഥിര("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
സ്ഥിതം വരിയിൽ ശൂന്യം reload_ttbr1(ശൂന്യം)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
സ്ഥിതം വരിയിൽ u32_t read_ttbcr(ശൂന്യം)
{
	u32_t bcr;

	asm അസ്ഥിര("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	തിരികെ bcr;
}

/* Write Translation Table Base Control Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_ttbcr(u32_t bcr)
{
	asm അസ്ഥിര("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
സ്ഥിതം വരിയിൽ u32_t read_dacr(ശൂന്യം)
{
	u32_t dacr;

	asm അസ്ഥിര("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	തിരികെ dacr;
}

/* Write Domain Access Control Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_dacr(u32_t dacr)
{
	asm അസ്ഥിര("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
സ്ഥിതം വരിയിൽ u32_t read_dfsr(ശൂന്യം)
{
	u32_t fsr;

	asm അസ്ഥിര("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	തിരികെ fsr;
}

/* Write Data Fault Status Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_dfsr(u32_t fsr)
{
	asm അസ്ഥിര("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
സ്ഥിതം വരിയിൽ u32_t read_ifsr(ശൂന്യം)
{
	u32_t fsr;

	asm അസ്ഥിര("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	തിരികെ fsr;
}

/* Write Instruction Fault Status Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_ifsr(u32_t fsr)
{
	asm അസ്ഥിര("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
സ്ഥിതം വരിയിൽ u32_t read_dfar(ശൂന്യം)
{
	u32_t far;

	asm അസ്ഥിര("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	തിരികെ far;
}

/* Write Data Fault Address Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_dfar(u32_t far)
{
	asm അസ്ഥിര("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
സ്ഥിതം വരിയിൽ u32_t read_ifar(ശൂന്യം)
{
	u32_t far;

	asm അസ്ഥിര("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	തിരികെ far;
}

/* Write Instruction Fault Address Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_ifar(u32_t far)
{
	asm അസ്ഥിര("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
സ്ഥിതം വരിയിൽ u32_t read_vbar(ശൂന്യം)
{
	u32_t vbar;

	asm അസ്ഥിര("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	തിരികെ vbar;
}

/* Write Vector Base Address Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_vbar(u32_t vbar)
{
	asm അസ്ഥിര("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
സ്ഥിതം വരിയിൽ u32_t read_midr(ശൂന്യം)
{
	u32_t id;

	asm അസ്ഥിര("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	തിരികെ id;
}

/* Read Auxiliary Control Register */
സ്ഥിതം വരിയിൽ u32_t read_actlr(ശൂന്യം)
{
	u32_t ctl;

	asm അസ്ഥിര("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	തിരികെ ctl;
}

/* Write Auxiliary Control Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm അസ്ഥിര("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
സ്ഥിതം വരിയിൽ u32_t read_cpsr(ശൂന്യം)
{
	u32_t status;

	asm അസ്ഥിര("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	തിരികെ status;
}

/* Write Current Program Status Register */
സ്ഥിതം വരിയിൽ ശൂന്യം write_cpsr(u32_t status)
{
	asm അസ്ഥിര("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
