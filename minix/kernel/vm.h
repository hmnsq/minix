
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _VM_H
#ವ್ಯಾಖ್ಯಾನಿಸು _VM_H 1

/* Pseudo error codes */
#ವ್ಯಾಖ್ಯಾನಿಸು VMSUSPEND       (-996)
#ವ್ಯಾಖ್ಯಾನಿಸು EFAULT_SRC	(-995)
#ವ್ಯಾಖ್ಯಾನಿಸು EFAULT_DST	(-994)

#ವ್ಯಾಖ್ಯಾನಿಸು PHYS_COPY_CATCH(src, dst, ಗಾತ್ರ_2, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, ಗಾತ್ರ_2);		\
	catch_pagefaults--;			\
	}

#ಆದರೆ_ಅಂತ್ಯ


