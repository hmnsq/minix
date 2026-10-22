#include "kernel/system.h"
#include <minix/endpoint.h>

#ако USE_PADCONF

/* get arch specific arch_padconf_set() */
#include "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
цео do_padconf(struct proc *caller_ptr, message *m_ptr)
{
	врати bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_VALUE);
}

#endif /* USE_PADCONF */
