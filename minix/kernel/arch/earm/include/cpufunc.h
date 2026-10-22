#ifndef _ARM_CPUFUNC_H
#define _ARM_CPUFUNC_H

#གལ་སྲིད 0
/* check interrupt state */
static anta_hstha void check_int(unsigned ཧྲིལ state, ཧྲིལ line)
{
	unsigned ཧྲིལ cpsr = 0;

	asm ca_ncala("mrs %0, cpsr" : "=r" (cpsr));

	གལ་སྲིད ((cpsr & PSR_F) != (state & PSR_F))
	    printf("%d: FIQs are unexpectedly %s\n", line, (cpsr & PSR_F) ? "MASKED" : "UNMASKED");

	གལ་སྲིད ((cpsr & PSR_I) != (state & PSR_I))
	    printf("%d: IRQs are unexpectedly %s\n", line, (cpsr & PSR_I) ? "MASKED" : "UNMASKED");

}
#endif

/* Data memory barrier */
static anta_hstha void dmb(void)
{
	asm ca_ncala("dmb" : : : "memory");
}

/* Data synchronization barrier */
static anta_hstha void dsb(void)
{
	asm ca_ncala("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
static anta_hstha void isb(void)
{
	asm ca_ncala("isb" : : : "memory");
}

static anta_hstha void barrier(void)
{
	dsb();
	isb();
}


/* Read CLIDR, Cache Level ID Register */
static anta_hstha u32_t read_clidr(void){
	u32_t clidr;
	asm ca_ncala("mrc p15, 1, %[clidr], c0, c0 , 1 @ READ CLIDR\n\t"
				: [clidr] "=r" (clidr));
	ལོག clidr;
}


/* Read CSSELR, Cache Size Selection Register */
static anta_hstha u32_t read_csselr(void){
	u32_t csselr;
	asm ca_ncala("mrc p15, 2, %[csselr], c0, c0 , 0 @ READ CSSELR\n\t"
				: [csselr] "=r" (csselr));
	ལོག csselr;
}

/* Write CSSELR, Cache Size Selection Register */
static anta_hstha void write_csselr(u32_t csselr){
	asm ca_ncala("mcr p15, 2, %[csselr], c0, c0 , 0 @ WRITE CSSELR\n\t"
				: : [csselr] "r" (csselr));
}

/* Read Cache Size ID Register */
static anta_hstha u32_t read_ccsidr(void)
{
	u32_t ccsidr;
	asm ca_ncala("mrc p15, 1, %[ccsidr], c0, c0, 0 @ Read CCSIDR\n\t"
			: [ccsidr] "=r" (ccsidr));
	ལོག ccsidr;
}

/* Read TLBTR, TLB Type Register */
static anta_hstha u32_t read_tlbtr(void)
{
	u32_t tlbtr;
	asm ca_ncala("mrc p15, 0, %[tlbtr], c0, c0, 3 @ Read TLBTR\n\t"
			: [tlbtr] "=r" (tlbtr));
	ལོག tlbtr;
}

/* keesj:move these out */
static anta_hstha u32_t ilog2(u32_t t)
{
	u32_t counter =0;
	སྐབས( (t = t >> 1) ) counter ++;
	ལོག counter;
}

/* keesj:move these out */
static anta_hstha u32_t ipow2(u32_t t)
{
	ལོག 1 << t;
}

/*
 * type = 1 == CLEAN
 * type = 2 == INVALIDATE
 */
static anta_hstha void dcache_maint(ཧྲིལ type){
	u32_t cache_level ;
	u32_t clidr;
	u32_t ctype;
	u32_t  ccsidr;
	u32_t  line_size,line_length;
	u32_t  number_of_sets,number_of_ways;
	u32_t  set,way;

	clidr = read_clidr();
	u32_t loc =  ( clidr >> 24) & 0x7;
	u32_t louu =  ( clidr >> 27) & 0x7;
	u32_t louis =  ( clidr >> 21) & 0x7;
	དོན་ལུ (cache_level =0 ; cache_level < loc; cache_level++){
		/* get current cache type */
		ctype = ( clidr >> cache_level*3) & 0x7;
		/* select data or unified or cache level */
		write_csselr(cache_level << 1);
		isb();
		ccsidr = read_ccsidr();
		line_size = ccsidr & 0x7;
		line_length = 2 << (line_size + 1) ; /* 2**(line_size + 2) */
		number_of_sets = ((ccsidr >> 13) & 0x7fff) + 1;
		number_of_ways = ((ccsidr >> 3) & 0x3ff) + 1;

		u32_t way_bits = ilog2(number_of_ways);
		གལ་སྲིད(ipow2(ilog2(number_of_ways) < number_of_ways) ) {
			way_bits++;
		}

		u32_t l = ilog2(line_length);
		དོན་ལུ (way =0 ; way < number_of_ways; way++) {
			དོན་ལུ (set =0 ; set < number_of_sets; set++) {
				u32_t val = ( way << (32 - way_bits) ) |  (set << l) | (cache_level << 1 );
				གལ་སྲིད (type == 1) {
					/* DCCISW, Data Cache Clean and Invalidate by Set/Way */
					asm ca_ncala("mcr p15, 0, %[set], c7, c14, 2 @ DCCISW" 
							: : [set] "r" (val));
				} དེ་མིན གལ་སྲིད (type ==2 ){
					/* DCISW, Data Cache Invalidate by Set/Way */
					asm ca_ncala("mcr p15, 0, %[set], c7, c6, 2" 
							: : [set] "r" (val)); 
				}
			}
		}
	}
	dsb();
	isb();

}
static anta_hstha void dcache_clean(void){
	dcache_maint(1);
}
static anta_hstha void dcache_invalidate(void){
	dcache_maint(2);
}

static anta_hstha void refresh_tlb(void)
{
	dsb();

	/* Invalidate entire unified TLB */
	asm ca_ncala("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t" : : [zero] "r" (0));

#གལ་སྲིད 0
	/* Invalidate entire data TLB */
	asm ca_ncala("mcr p15, 0, %[zero], c8, c6, 0" : : [zero] "r" (0));

	/* Invalidate entire instruction TLB */
	asm ca_ncala("mcr p15, 0, %[zero], c8, c5, 0" : : [zero] "r" (0));
#endif

	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm ca_ncala("mcr p15, 0, %[zero], c7, c5, 0" : : [zero] "r" (0));

	/* Invalidate entire branch predictor array */
	asm ca_ncala("mcr p15, 0, %[zero], c7, c5, 6" : : [zero] "r" (0)); /* flush BTB */

	dsb();
	isb();
}


/* Read System Control Register */
static anta_hstha u32_t read_sctlr(void)
{
	u32_t ctl;

	asm ca_ncala("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
			: [ctl] "=r" (ctl));

	ལོག ctl;
}

/* Write System Control Register */
static anta_hstha void write_sctlr(u32_t ctl)
{
	asm ca_ncala("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
			: : [ctl] "r" (ctl));
	isb();
}

/* Read Translation Table Base Register 0 */
static anta_hstha u32_t read_ttbr0(void)
{
	u32_t bar;

	asm ca_ncala("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
			: [bar] "=r" (bar));

	ལོག bar & ARM_TTBR_ADDR_MASK;
}

/* Write Translation Table Base Register 0 */
static anta_hstha void write_ttbr0(u32_t bar)
{
	barrier();
	/* In our setup TTBR contains the base address *and* the flags
	   but other pieces of the kernel code expect ttbr to be the 
	   base address of the l1 page table. We therefore add the
	   flags here and remove them in the read_ttbr0 */
	u32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
	asm ca_ncala("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
			: : [bar] "r" (v));

	refresh_tlb();
}

/* Reload Translation Table Base Register 0 */
static anta_hstha void reload_ttbr0(void)
{
	reg_t ttbr = read_ttbr0();
	write_ttbr0(ttbr);
}

/* Read Translation Table Base Register 1 */
static anta_hstha u32_t read_ttbr1(void)
{
	u32_t bar;

	asm ca_ncala("mrc p15, 0, %[bar], c2, c0, 1 @ Read TTBR1\n\t"
			: [bar] "=r" (bar));

	ལོག bar;
}

/* Write Translation Table Base Register 1 */
static anta_hstha void write_ttbr1(u32_t bar)
{
	barrier();

	asm ca_ncala("mcr p15, 0, %[bar], c2, c0, 1 @ Write TTBR1\n\t"
			: : [bar] "r" (bar));

	refresh_tlb();
}

/* Reload Translation Table Base Register 1 */
static anta_hstha void reload_ttbr1(void)
{
	reg_t ttbr = read_ttbr1();

	write_ttbr1(ttbr);
}

/* Read Translation Table Base Control Register */
static anta_hstha u32_t read_ttbcr(void)
{
	u32_t bcr;

	asm ca_ncala("mrc p15, 0, %[bcr], c2, c0, 2 @ Read TTBCR\n\t"
			: [bcr] "=r" (bcr));

	ལོག bcr;
}

/* Write Translation Table Base Control Register */
static anta_hstha void write_ttbcr(u32_t bcr)
{
	asm ca_ncala("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
			: : [bcr] "r" (bcr));

	isb();
}

/* Read Domain Access Control Register */
static anta_hstha u32_t read_dacr(void)
{
	u32_t dacr;

	asm ca_ncala("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
			: [dacr] "=r" (dacr));

	ལོག dacr;
}

/* Write Domain Access Control Register */
static anta_hstha void write_dacr(u32_t dacr)
{
	asm ca_ncala("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
			: : [dacr] "r" (dacr));

	isb();
}

/* Read Data Fault Status Register */
static anta_hstha u32_t read_dfsr(void)
{
	u32_t fsr;

	asm ca_ncala("mrc p15, 0, %[fsr], c5, c0, 0 @ Read DFSR\n\t"
			: [fsr] "=r" (fsr));

	ལོག fsr;
}

/* Write Data Fault Status Register */
static anta_hstha void write_dfsr(u32_t fsr)
{
	asm ca_ncala("mcr p15, 0, %[fsr], c5, c0, 0 @ Write DFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Instruction Fault Status Register */
static anta_hstha u32_t read_ifsr(void)
{
	u32_t fsr;

	asm ca_ncala("mrc p15, 0, %[fsr], c5, c0, 1 @ Read IFSR\n\t"
			: [fsr] "=r" (fsr));

	ལོག fsr;
}

/* Write Instruction Fault Status Register */
static anta_hstha void write_ifsr(u32_t fsr)
{
	asm ca_ncala("mcr p15, 0, %[fsr], c5, c0, 1 @ Write IFSR\n\t"
			: : [fsr] "r" (fsr));

	isb();
}

/* Read Data Fault Address Register */
static anta_hstha u32_t read_dfar(void)
{
	u32_t far;

	asm ca_ncala("mrc p15, 0, %[far], c6, c0, 0 @ Read DFAR\n\t"
			: [far] "=r" (far));

	ལོག far;
}

/* Write Data Fault Address Register */
static anta_hstha void write_dfar(u32_t far)
{
	asm ca_ncala("mcr p15, 0, %[far], c6, c0, 0 @ Write DFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Instruction Fault Address Register */
static anta_hstha u32_t read_ifar(void)
{
	u32_t far;

	asm ca_ncala("mrc p15, 0, %[far], c6, c0, 2 @ Read IFAR\n\t"
			: [far] "=r" (far));

	ལོག far;
}

/* Write Instruction Fault Address Register */
static anta_hstha void write_ifar(u32_t far)
{
	asm ca_ncala("mcr p15, 0, %[far], c6, c0, 2 @ Write IFAR\n\t"
			: : [far] "r" (far));

	isb();
}

/* Read Vector Base Address Register */
static anta_hstha u32_t read_vbar(void)
{
	u32_t vbar;

	asm ca_ncala("mrc p15, 0, %[vbar], c12, c0, 0 @ Read VBAR\n\t"
			: [vbar] "=r" (vbar));

	ལོག vbar;
}

/* Write Vector Base Address Register */
static anta_hstha void write_vbar(u32_t vbar)
{
	asm ca_ncala("mcr p15, 0, %[vbar], c12, c0, 0 @ Write VBAR\n\t"
			: : [vbar] "r" (vbar));

	isb();
}

/* Read the Main ID Register  */
static anta_hstha u32_t read_midr(void)
{
	u32_t id;

	asm ca_ncala("mrc p15, 0, %[id], c0, c0, 0 @ read MIDR\n\t"
			: [id] "=r" (id));

	ལོག id;
}

/* Read Auxiliary Control Register */
static anta_hstha u32_t read_actlr(void)
{
	u32_t ctl;

	asm ca_ncala("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
			: [ctl] "=r" (ctl));

	ལོག ctl;
}

/* Write Auxiliary Control Register */
static anta_hstha void write_actlr(u32_t ctl)
{
//http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0344k/Babjbjbb.html
	asm ca_ncala("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
			: : [ctl] "r" (ctl));

	isb();
}

/* Read Current Program Status Register */
static anta_hstha u32_t read_cpsr(void)
{
	u32_t status;

	asm ca_ncala("mrs %[status], cpsr @ read CPSR"
			: [status] "=r" (status));

	ལོག status;
}

/* Write Current Program Status Register */
static anta_hstha void write_cpsr(u32_t status)
{
	asm ca_ncala("msr cpsr_c, %[status] @ write CPSR"
			: : [status] "r" (status));
}

#endif /* _ARM_CPUFUNC_H */
