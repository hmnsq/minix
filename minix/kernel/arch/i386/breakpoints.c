#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"

#અન્તર્ભાવ "debugreg.h"

પૂર્ણાંક breakpoint_set(phys_bytes linaddr, પૂર્ણાંક bp, અચલ પૂર્ણાંક flags)
{
	unsigned long dr7, dr7flags;
	
	જો (bp >= BREAKPOINT_ગણતરી)
		પાછા EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	પસંદગી (flags & BREAKPOINT_FLAG_RW_MASK) {
		સ્થિતિ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  તોડો;
		સ્થિતિ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); તોડો;
		સ્થિતિ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    તોડો;
		મૂળભૂત: પાછા EINVAL;			
	}
	પસંદગી (flags & BREAKPOINT_FLAG_LEN_MASK) {
		સ્થિતિ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); તોડો;
		સ્થિતિ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); તોડો;
		સ્થિતિ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); તોડો;
		મૂળભૂત: પાછા EINVAL;	
	}
	પસંદગી (flags & BREAKPOINT_FLAG_MODE_MASK) {
		સ્થિતિ BREAKPOINT_FLAG_MODE_OFF: તોડો;
		સ્થિતિ BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); તોડો;
		સ્થિતિ BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); તોડો;
		મૂળભૂત: પાછા EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	જો ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		પાછા 0;
			
	/* set breakpoint address */
	પસંદગી (bp) {
		સ્થિતિ 0: ld_dr0(linaddr); તોડો;
		સ્થિતિ 1: ld_dr1(linaddr); તોડો;
		સ્થિતિ 2: ld_dr2(linaddr); તોડો;
		સ્થિતિ 3: ld_dr3(linaddr); તોડો;
		મૂળભૂત: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	પાછા 0;
}

