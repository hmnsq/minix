#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#যদি 0
/* check interrupt state */
স্থির অন্তঃস্থ শূন্য check_int(unsigned পূর্ণাঙ্ক state, পূর্ণাঙ্ক সাৰি)
{
	unsigned পূর্ণাঙ্ক cpsr = 0;

	asm চঞ্চল("mrs %0, cpsr" : "=r" (cpsr));

	যদি ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", সাৰি, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	যদি ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", সাৰি, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
স্থির অন্তঃস্থ শূন্য dmb(শূন্য)
{
	asm চঞ্চল("dmb" : : : "memory");
}

/* Data synchronization barrier */
স্থির অন্তঃস্থ শূন্য dsb(শূন্য)
{
	asm চঞ্চল("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
স্থির অন্তঃস্থ শূন্য isb(শূন্য)
{
	asm চঞ্চল("isb" : : : "memory");
}

স্থির অন্তঃস্থ শূন্য barrier(শূন্য)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
স্থির অন্তঃস্থ u32_t read_clidr(শূন্য){
	u32_t clidr;
	asm চঞ্চল("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ঘূৰাই clidr;
}


/* Read CSSELR, Cache Size Selection Register */
স্থির অন্তঃস্থ u32_t read_csselr(শূন্য){
	u32_t csselr;
	asm চঞ্চল("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ঘূৰাই csselr;
}

/* Write CSSELR, Cache Size Selection Register */
স্থির অন্তঃস্থ শূন্য write_csselr(u32_t csselr){
	asm চঞ্চল("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
স্থির অন্তঃস্থ u32_t read_ccsidr(শূন্য)
{
	u32_t ccsidr;
	asm চঞ্চল("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ঘূৰাই ccsidr;
}

/* Read TLBTR, TLB Type Register */
স্থির অন্তঃস্থ u32_t read_tlbtr(শূন্য)
{
	u32_t tlbtr;
	asm চঞ্চল("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ঘূৰাই tlbtr;
}

/* keesj:move these out */
স্থির অন্তঃস্থ u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	যেতিয়ালৈ( (t = t >> 1) ) counter ++;
	ঘূৰাই counter;
}

/* keesj:move these out */
স্থির অন্তঃস্থ u32_t ipow2(u32_t t)
{
	ঘূৰাই 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
স্থির অন্তঃস্থ শূন্য dcache_maint(পূর্ণাঙ্ক type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  সাৰি_size,সাৰি_length;
	u32_t  সংখ্যা_of_sets,সংখ্যা_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	বাবে (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		সাৰি_size = ccsidr & 0x7;
		সাৰি_length = 2 << (সাৰি_size + 1) ; /* 2**(line_size + 2) */
		সংখ্যা_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		সংখ্যা_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(সংখ্যা_of_ways);
		যদি(ipow2(ilog2(সংখ্যা_of_ways) < সংখ্যা_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(সাৰি_length);
		বাবে (way =0 ; way < সংখ্যা_of_ways; way++) {
			বাবে (set =0 ; set < সংখ্যা_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				যদি (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm চঞ্চল("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} অন্যথা যদি (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm চঞ্চল("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
স্থির অন্তঃস্থ শূন্য dcache_clean(শূন্য){
	dcache_maint(1);
}
স্থির অন্তঃস্থ শূন্য dcache_invalidate(শূন্য){
	dcache_maint(2);
}

স্থির অন্তঃস্থ শূন্য refresh_tlb(শূন্য)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm চঞ্চল("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#যদি 0
	/* Invalidate entire data TLB */
	asm চঞ্চল("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm চঞ্চল("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm চঞ্চল("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm চঞ্চল("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
স্থির অন্তঃস্থ u32_t read_sctlr(শূন্য)
{
	u32_t ctl;

	asm চঞ্চল("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ঘূৰাই ctl;
}

/* Write System Control Register */
স্থির অন্তঃস্থ শূন্য write_sctlr(u32_t ctl)
{
	asm চঞ্চল("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
স্থির অন্তঃস্থ u32_t read_ttbr0(শূন্য)
{
	u32_t bar;

	asm চঞ্চল("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ঘূৰাই bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
স্থির অন্তঃস্থ শূন্য write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm চঞ্চল("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
স্থির অন্তঃস্থ শূন্য reload_ttbr0(শূন্য)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
স্থির অন্তঃস্থ u32_t read_ttbr1(শূন্য)
{
	u32_t bar;

	asm চঞ্চল("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ঘূৰাই bar;
}

/* Write Translation Table Base Register 1 */
স্থির অন্তঃস্থ শূন্য write_ttbr1(u32_t bar)
{
	barrier();

	asm চঞ্চল("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
স্থির অন্তঃস্থ শূন্য reload_ttbr1(শূন্য)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
স্থির অন্তঃস্থ u32_t read_ttbcr(শূন্য)
{
	u32_t bcr;

	asm চঞ্চল("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ঘূৰাই bcr;
}

/* Write Translation Table Base Control Register */
স্থির অন্তঃস্থ শূন্য write_ttbcr(u32_t bcr)
{
	asm চঞ্চল("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
স্থির অন্তঃস্থ u32_t read_dacr(শূন্য)
{
	u32_t dacr;

	asm চঞ্চল("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ঘূৰাই dacr;
}

/* Write Domain Access Control Register */
স্থির অন্তঃস্থ শূন্য write_dacr(u32_t dacr)
{
	asm চঞ্চল("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
স্থির অন্তঃস্থ u32_t read_dfsr(শূন্য)
{
	u32_t fsr;

	asm চঞ্চল("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ঘূৰাই fsr;
}

/* Write Data Fault Status Register */
স্থির অন্তঃস্থ শূন্য write_dfsr(u32_t fsr)
{
	asm চঞ্চল("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
স্থির অন্তঃস্থ u32_t read_ifsr(শূন্য)
{
	u32_t fsr;

	asm চঞ্চল("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ঘূৰাই fsr;
}

/* Write Instruction Fault Status Register */
স্থির অন্তঃস্থ শূন্য write_ifsr(u32_t fsr)
{
	asm চঞ্চল("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
স্থির অন্তঃস্থ u32_t read_dfar(শূন্য)
{
	u32_t far;

	asm চঞ্চল("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ঘূৰাই far;
}

/* Write Data Fault Address Register */
স্থির অন্তঃস্থ শূন্য write_dfar(u32_t far)
{
	asm চঞ্চল("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
স্থির অন্তঃস্থ u32_t read_ifar(শূন্য)
{
	u32_t far;

	asm চঞ্চল("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ঘূৰাই far;
}

/* Write Instruction Fault Address Register */
স্থির অন্তঃস্থ শূন্য write_ifar(u32_t far)
{
	asm চঞ্চল("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
স্থির অন্তঃস্থ u32_t read_vbar(শূন্য)
{
	u32_t vbar;

	asm চঞ্চল("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ঘূৰাই vbar;
}

/* Write Vector Base Address Register */
স্থির অন্তঃস্থ শূন্য write_vbar(u32_t vbar)
{
	asm চঞ্চল("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
স্থির অন্তঃস্থ u32_t read_midr(শূন্য)
{
	u32_t id;

	asm চঞ্চল("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ঘূৰাই id;
}

/* Read Auxiliary Control Register */
স্থির অন্তঃস্থ u32_t read_actlr(শূন্য)
{
	u32_t ctl;

	asm চঞ্চল("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ঘূৰাই ctl;
}

/* Write Auxiliary Control Register */
স্থির অন্তঃস্থ শূন্য write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm চঞ্চল("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
স্থির অন্তঃস্থ u32_t read_cpsr(শূন্য)
{
	u32_t status;

	asm চঞ্চল("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	ঘূৰাই status;
}

/* Write Current Program Status Register */
স্থির অন্তঃস্থ শূন্য write_cpsr(u32_t status)
{
	asm চঞ্চল("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
