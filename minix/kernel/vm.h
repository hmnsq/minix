
#جے_بنا_تعریف _VM_H
#تعریف_کرو _VM_H 1

/* Pseudo error codes */
#تعریف_کرو VMSUSPEND       (-996)
#تعریف_کرو EFAULT_SRC	(-995)
#تعریف_کرو EFAULT_DST	(-994)

#تعریف_کرو PHYS_COPY_CATCH(src, dst, ناپ, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, ناپ);		\
	catch_pagefaults--;			\
	}

#جے_انت


