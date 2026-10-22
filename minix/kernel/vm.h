
#अगर_अघोषित _VM_H
#घोषणा _VM_H 1

/* Pseudo error codes */
#घोषणा VMSUSPEND       (-996)
#घोषणा EFAULT_SRC	(-995)
#घोषणा EFAULT_DST	(-994)

#घोषणा PHYS_COPY_CATCH(src, dst, आकार, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, आकार);		\
	catch_pagefaults--;			\
	}

#पूर्ण_अगर


