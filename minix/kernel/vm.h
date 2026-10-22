
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _VM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _VM_H 1

/* Pseudo error codes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMSUSPEND       (-996)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFAULT_SRC	(-995)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EFAULT_DST	(-994)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PHYS_COPY_CATCH(src, dst, ਮਾਪ, a) {	\
	catch_pagefaults++;			\
	a = phys_copy(src, dst, ਮਾਪ);		\
	catch_pagefaults--;			\
	}

#ਜੇ_ਅੰਤ


