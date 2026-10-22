#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव "debugreg.h"

पूर्णांक breakpoint_set(phys_bytes linaddr, पूर्णांक bp, स्थिरांक पूर्णांक flags)
{
	अचिन्हित दीर्घ dr7, dr7flags;
	
	जर (bp >= BREAKPOINT_मोजणी)
		परत EINVAL;
	
	/* convert flags */
	dr7flags = 0;
	निवड (flags & BREAKPOINT_FLAG_RW_MASK) {
		स्थिती BREAKPOINT_FLAG_RW_EXEC:  dr7flags |= DR7_RW_EXEC(bp);  थांबा;
		स्थिती BREAKPOINT_FLAG_RW_WRITE: dr7flags |= DR7_RW_WRITE(bp); थांबा;
		स्थिती BREAKPOINT_FLAG_RW_RW:    dr7flags |= DR7_RW_RW(bp);    थांबा;
		पूर्वनिर्धारित: परत EINVAL;			
	}
	निवड (flags & BREAKPOINT_FLAG_LEN_MASK) {
		स्थिती BREAKPOINT_FLAG_LEN_1: dr7flags |= DR7_LN_1(bp); थांबा;
		स्थिती BREAKPOINT_FLAG_LEN_2: dr7flags |= DR7_LN_2(bp); थांबा;
		स्थिती BREAKPOINT_FLAG_LEN_4: dr7flags |= DR7_LN_4(bp); थांबा;
		पूर्वनिर्धारित: परत EINVAL;	
	}
	निवड (flags & BREAKPOINT_FLAG_MODE_MASK) {
		स्थिती BREAKPOINT_FLAG_MODE_OFF: थांबा;
		स्थिती BREAKPOINT_FLAG_MODE_LOCAL: dr7flags |= DR7_L(bp); थांबा;
		स्थिती BREAKPOINT_FLAG_MODE_GLOBAL: dr7flags |= DR7_G(bp); थांबा;
		पूर्वनिर्धारित: परत EINVAL;	
	}
	
	/* disable breakpoint before setting address */
	dr7 = st_dr7();
	dr7 &= ~(DR7_L(bp) | DR7_G(bp) | DR7_RW_MASK(bp) | DR7_LN_MASK(bp));
	ld_dr7(dr7);

	/* need to set new breakpoint? */	
	जर ((flags & BREAKPOINT_FLAG_MODE_MASK) == BREAKPOINT_FLAG_MODE_OFF)
		परत 0;
			
	/* set breakpoint address */
	निवड (bp) {
		स्थिती 0: ld_dr0(linaddr); थांबा;
		स्थिती 1: ld_dr1(linaddr); थांबा;
		स्थिती 2: ld_dr2(linaddr); थांबा;
		स्थिती 3: ld_dr3(linaddr); थांबा;
		पूर्वनिर्धारित: panic("%s:%d: invalid breakpoint index", __FILE__,  __LINE__);
	}
	
	/* set new flags */
	dr7 |= dr7flags;
	ld_dr7(dr7);
	परत 0;
}

