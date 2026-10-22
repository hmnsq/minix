#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ "debugreg.h"

ಪೂರ್ಣಾಂಕ breakpoint_ಗಣ(phys_ಬೈಟುಗಳು linaddr, ಪೂರ್ಣಾಂಕ bp, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ dr7, dr7flags;
	
	ಆದರೆ (bp >= BREAKPOINT_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	ಆಯ್ಕೆ (flags & BREAKPOINT_FLAG_RW_ಮುಖವಾಡ) {
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ EINVAL;			
	}
	ಆಯ್ಕೆ (flags & BREAKPOINT_FLAG_LEN_ಮುಖವಾಡ) {
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ EINVAL;	
	}
	ಆಯ್ಕೆ (flags & BREAKPOINT_FLAG_ಬಹುಲಕ_ಮುಖವಾಡ) {
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_ಬಹುಲಕ_OFF: ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_ಬಹುಲಕ_LOCAL: dr7flags |= DR7_L(bp); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BREAKPOINT_FLAG_ಬಹುಲಕ_GLOBAL: dr7flags |= DR7_G(bp); ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_ಮುಖವಾಡ(bp) | DR7_LN_ಮುಖವಾಡ(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	ಆದರೆ ((flags & BREAKPOINT_FLAG_ಬಹುಲಕ_ಮುಖವಾಡ) == BREAKPOINT_FLAG_ಬಹುಲಕ_OFF)
		ಹಿಂತಿರುಗಿ 0;
			
	/* set breakpoint address */
	ಆಯ್ಕೆ (bp) {
		ಸ್ಥಿತಿ 0: ld_dr0(linaddr); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1: ld_dr1(linaddr); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2: ld_dr2(linaddr); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 3: ld_dr3(linaddr); ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	ಹಿಂತಿರುಗಿ 0;
}

