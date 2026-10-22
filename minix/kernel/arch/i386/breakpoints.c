#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ "debugreg.h"

ಪೂರ್ಣಾಙ್ಕ breakpoint_set(phys_bytes linaddr, ಪೂರ್ಣಾಙ್ಕ bp, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ flags)
{
	unsigned long dr7, dr7flags;
	
	ಯದಿ (bp >= BREAKPOINT_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	ವಿಕಲ್ಪ (flags & BREAKPOINT_FLAG_RW_MASK) {
		ಪಕ್ಷ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ಭಙ್ಗ;
		ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ EINVAL;			
	}
	ವಿಕಲ್ಪ (flags & BREAKPOINT_FLAG_LEN_MASK) {
		ಪಕ್ಷ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ಭಙ್ಗ;
		ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ EINVAL;	
	}
	ವಿಕಲ್ಪ (flags & BREAKPOINT_FLAG_MODE_MASK) {
		ಪಕ್ಷ BREAKPOINT_FLAG_MODE_OFF: ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); ಭಙ್ಗ;
		ಪಕ್ಷ BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); ಭಙ್ಗ;
		ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ಯದಿ ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		ಪ್ರತ್ಯಾಗಮ 0;
			
	/* set breakpoint address */
	ವಿಕಲ್ಪ (bp) {
		ಪಕ್ಷ 0: ld_dr0(linaddr); ಭಙ್ಗ;
		ಪಕ್ಷ 1: ld_dr1(linaddr); ಭಙ್ಗ;
		ಪಕ್ಷ 2: ld_dr2(linaddr); ಭಙ್ಗ;
		ಪಕ್ಷ 3: ld_dr3(linaddr); ಭಙ್ಗ;
		ಸ್ವಭಾವ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ಪ್ರತ್ಯಾಗಮ 0;
}

