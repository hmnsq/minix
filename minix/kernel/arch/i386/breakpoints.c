#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_proto.h"

#അന്തര്ഭാവ "debugreg.h"

പൂർണ്ണസംഖ്യ breakpoint_set(phys_bytes linaddr, പൂർണ്ണസംഖ്യ bp, സ്ഥിരം പൂർണ്ണസംഖ്യ flags)
{
	unsigned long dr7, dr7flags;
	
	എങ്കിൽ (bp >= BREAKPOINT_എണ്ണം)
		തിരികെ EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	തിരഞ്ഞെടുപ്പ് (flags & BREAKPOINT_FLAG_RW_MASK) {
		അവസ്ഥ BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    നിർത്തുക;
		സ്ഥിരസ്ഥിതി: തിരികെ EINVAL;			
	}
	തിരഞ്ഞെടുപ്പ് (flags & BREAKPOINT_FLAG_LEN_MASK) {
		അവസ്ഥ BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); നിർത്തുക;
		സ്ഥിരസ്ഥിതി: തിരികെ EINVAL;	
	}
	തിരഞ്ഞെടുപ്പ് (flags & BREAKPOINT_FLAG_MODE_MASK) {
		അവസ്ഥ BREAKPOINT_FLAG_MODE_OFF: നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); നിർത്തുക;
		അവസ്ഥ BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); നിർത്തുക;
		സ്ഥിരസ്ഥിതി: തിരികെ EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	എങ്കിൽ ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		തിരികെ 0;
			
	/* set breakpoint address */
	തിരഞ്ഞെടുപ്പ് (bp) {
		അവസ്ഥ 0: ld_dr0(linaddr); നിർത്തുക;
		അവസ്ഥ 1: ld_dr1(linaddr); നിർത്തുക;
		അവസ്ഥ 2: ld_dr2(linaddr); നിർത്തുക;
		അവസ്ഥ 3: ld_dr3(linaddr); നിർത്തുക;
		സ്ഥിരസ്ഥിതി: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	തിരികെ 0;
}

