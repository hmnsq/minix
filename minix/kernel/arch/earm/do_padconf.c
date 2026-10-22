#include "kernel/system.h"
#include <minix/endpoint.h>

#නම් USE_PADCONF

/* get arch specific arch_padconf_set() */
#include "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
පූර්ණ_සංඛ්‍යා do_padconf(sa_nracanaa proc *caller_ptr, message *m_ptr)
{
	ආපසු bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_VALUE);
}

#endif /* USE_PADCONF */
