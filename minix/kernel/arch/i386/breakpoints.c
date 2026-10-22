#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ଅନ୍ତର୍ଭାଵ "debugreg.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା breakpoint_set(phys_bytes linaddr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bp, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
	unsigned long dr7, dr7flags;
	
	ଯଦି (bp >= BREAKPOINT_ଗଣନା)
		ଫେରନ୍ତୁ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	ବାଛ (flags & BREAKPOINT_FLAG_RW_MASK) {
		ସ୍ଥିତି BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ EINVAL;			
	}
	ବାଛ (flags & BREAKPOINT_FLAG_LEN_MASK) {
		ସ୍ଥିତି BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ EINVAL;	
	}
	ବାଛ (flags & BREAKPOINT_FLAG_MODE_MASK) {
		ସ୍ଥିତି BREAKPOINT_FLAG_MODE_OFF: ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); ଭାଙ୍ଗ;
		ସ୍ଥିତି BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ଯଦି ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		ଫେରନ୍ତୁ 0;
			
	/* set breakpoint address */
	ବାଛ (bp) {
		ସ୍ଥିତି 0: ld_dr0(linaddr); ଭାଙ୍ଗ;
		ସ୍ଥିତି 1: ld_dr1(linaddr); ଭାଙ୍ଗ;
		ସ୍ଥିତି 2: ld_dr2(linaddr); ଭାଙ୍ଗ;
		ସ୍ଥିତି 3: ld_dr3(linaddr); ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ଫେରନ୍ତୁ 0;
}

