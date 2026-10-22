
#که_نه_تعریف_شوی _VM_H
#تعریف_کړه _VM_H 1

/* Pseudo error codes */
#تعریف_کړه VMSUSPEND       (-996)
#تعریف_کړه EFAULT_SRC	(-995)
#تعریف_کړه EFAULT_DST	(-994)

#تعریف_کړه PHYS_COPY_CATCH(src, dst, اندازه, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, اندازه);		\
	catch_pagefaults--;			\
	}

#د_که_پای


