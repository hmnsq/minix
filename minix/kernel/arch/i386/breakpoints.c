#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"

#समावेश "debugreg.h"

पूर्णांक breakpoint_set(phys_bytes linaddr, पूर्णांक bp, स्थिर पूर्णांक flags)
{
	अचिन्हित दीर्घ dr7, dr7flags;
	
	अगर (bp >= BREAKPOINT_गिनती)
		वापस EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	चयन (flags & BREAKPOINT_FLAG_RW_MASK) {
		हाल BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  अवरोध;
		हाल BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); अवरोध;
		हाल BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    अवरोध;
		शेष: वापस EINVAL;			
	}
	चयन (flags & BREAKPOINT_FLAG_LEN_MASK) {
		हाल BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); अवरोध;
		हाल BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); अवरोध;
		हाल BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); अवरोध;
		शेष: वापस EINVAL;	
	}
	चयन (flags & BREAKPOINT_FLAG_MODE_MASK) {
		हाल BREAKPOINT_FLAG_MODE_OFF: अवरोध;
		हाल BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); अवरोध;
		हाल BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); अवरोध;
		शेष: वापस EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	अगर ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		वापस 0;
			
	/* set breakpoint address */
	चयन (bp) {
		हाल 0: ld_dr0(linaddr); अवरोध;
		हाल 1: ld_dr1(linaddr); अवरोध;
		हाल 2: ld_dr2(linaddr); अवरोध;
		हाल 3: ld_dr3(linaddr); अवरोध;
		शेष: panic("%s:%d: invalid breakpoint index", __खाता__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	वापस 0;
}

