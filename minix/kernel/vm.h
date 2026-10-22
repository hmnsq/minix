
#اگر_تعریف_نشده _VM_H
#تعریف_کن _VM_H 1

/* Pseudo error codes */
#تعریف_کن VMSUSPEND       (-996)
#تعریف_کن EFAULT_SRC	(-995)
#تعریف_کن EFAULT_DST	(-994)

#تعریف_کن PHYS_COPY_CATCH(src, dst, اندازه, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, اندازه);		\
	catch_pagefaults--;			\
	}

#پایان_اگر


