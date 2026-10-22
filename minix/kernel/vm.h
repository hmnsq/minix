
#اگر_غیر_تعریف_شدہ _VM_H
#تعریف_کرو _VM_H 1

/* Pseudo error codes */
#تعریف_کرو VMSUSPEND       (-996)
#تعریف_کرو EFAULT_SRC	(-995)
#تعریف_کرو EFAULT_DST	(-994)

#تعریف_کرو PHYS_COPY_CATCH(src, dst, سائز, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, سائز);		\
	catch_pagefaults--;			\
	}

#اگر_ختم


