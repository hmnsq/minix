
#నిర్వచితం_కాకపోతే _VM_H
#నిర్వచించు _VM_H 1

/* Pseudo error codes */
#నిర్వచించు VMSUSPEND       (-996)
#నిర్వచించు EFAULT_SRC	(-995)
#నిర్వచించు EFAULT_DST	(-994)

#నిర్వచించు PHYS_COPY_CATCH(src, dst, కొలత, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, కొలత);		\
	catch_pagefaults--;			\
	}

#అయితే_ముగింపు


