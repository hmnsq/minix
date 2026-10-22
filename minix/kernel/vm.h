
#यदि_अपरिभाषितम् _VM_H
#परिभाषय _VM_H 1

/* Pseudo error codes */
#परिभाषय VMSUSPEND       (-996)
#परिभाषय EFAULT_SRC	(-995)
#परिभाषय EFAULT_DST	(-994)

#परिभाषय PHYS_COPY_CATCH(src, dst, आकार, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, आकार);		\
	catch_pagefaults--;			\
	}

#यदि_अन्तः


