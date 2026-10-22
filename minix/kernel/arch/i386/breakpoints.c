#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ "debugreg.h"

పూర్ణాంకం breakpoint_సమితి(phys_బైట్లు linaddr, పూర్ణాంకం bp, స్థిరరాశి పూర్ణాంకం flags)
{
	గుర్తులేని దీర్ఘ dr7, dr7flags;
	
	ఒకవేళ (bp >= BREAKPOINT_లెక్క)
		తిరిగి EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	ఎంపిక (flags & BREAKPOINT_FLAG_RW_ముసుగు) {
		స్థితి BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ఆపు;
		స్థితి BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ఆపు;
		స్థితి BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ఆపు;
		అప్రమేయ: తిరిగి EINVAL;			
	}
	ఎంపిక (flags & BREAKPOINT_FLAG_LEN_ముసుగు) {
		స్థితి BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ఆపు;
		స్థితి BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ఆపు;
		స్థితి BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ఆపు;
		అప్రమేయ: తిరిగి EINVAL;	
	}
	ఎంపిక (flags & BREAKPOINT_FLAG_బహుళకం_ముసుగు) {
		స్థితి BREAKPOINT_FLAG_బహుళకం_OFF: ఆపు;
		స్థితి BREAKPOINT_FLAG_బహుళకం_LOCAL: dr7flags |= DR7_L(bp); ఆపు;
		స్థితి BREAKPOINT_FLAG_బహుళకం_GLOBAL: dr7flags |= DR7_G(bp); ఆపు;
		అప్రమేయ: తిరిగి EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_ముసుగు(bp) | DR7_LN_ముసుగు(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ఒకవేళ ((flags & BREAKPOINT_FLAG_బహుళకం_ముసుగు) == BREAKPOINT_FLAG_బహుళకం_OFF)
		తిరిగి 0;
			
	/* set breakpoint address */
	ఎంపిక (bp) {
		స్థితి 0: ld_dr0(linaddr); ఆపు;
		స్థితి 1: ld_dr1(linaddr); ఆపు;
		స్థితి 2: ld_dr2(linaddr); ఆపు;
		స్థితి 3: ld_dr3(linaddr); ఆపు;
		అప్రమేయ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	తిరిగి 0;
}

