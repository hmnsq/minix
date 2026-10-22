#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव "debugreg.h"

पूर्णाङ्क breakpoint_set(phys_bytes linaddr, पूर्णाङ्क bp, ध्रुव पूर्णाङ्क flags)
{
	अचिन्हित दीर्घ dr7, dr7flags;
	
	यदि (bp >= BREAKPOINT_गिनती)
		प्रत्यागम EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	विकल्प (flags & BREAKPOINT_FLAG_RW_MASK) {
		पक्ष BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  भङ्ग;
		पक्ष BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); भङ्ग;
		पक्ष BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    भङ्ग;
		स्वभाव: प्रत्यागम EINVAL;			
	}
	विकल्प (flags & BREAKPOINT_FLAG_LEN_MASK) {
		पक्ष BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); भङ्ग;
		पक्ष BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); भङ्ग;
		पक्ष BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); भङ्ग;
		स्वभाव: प्रत्यागम EINVAL;	
	}
	विकल्प (flags & BREAKPOINT_FLAG_MODE_MASK) {
		पक्ष BREAKPOINT_FLAG_MODE_OFF: भङ्ग;
		पक्ष BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); भङ्ग;
		पक्ष BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); भङ्ग;
		स्वभाव: प्रत्यागम EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	यदि ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		प्रत्यागम 0;
			
	/* set breakpoint address */
	विकल्प (bp) {
		पक्ष 0: ld_dr0(linaddr); भङ्ग;
		पक्ष 1: ld_dr1(linaddr); भङ्ग;
		पक्ष 2: ld_dr2(linaddr); भङ्ग;
		पक्ष 3: ld_dr3(linaddr); भङ्ग;
		स्वभाव: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	प्रत्यागम 0;
}

