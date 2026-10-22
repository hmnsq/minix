
#جيڪڏهن_وصف_نه_ٿيل _VM_H
#وصف_ڪريو _VM_H 1

/* Pseudo error codes */
#وصف_ڪريو VMSUSPEND       (-996)
#وصف_ڪريو EFAULT_SRC	(-995)
#وصف_ڪريو EFAULT_DST	(-994)

#وصف_ڪريو PHYS_COPY_CATCH(src, dst, سائيز, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, سائيز);		\
	catch_pagefaults--;			\
	}

#جيڪڏهن_پورو


