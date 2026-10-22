#include "kernel/system.h"
#include <minix/endpoint.h>

#якщо USE_PADCONF

/* get arch specific arch_padconf_set() */
#include "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ціле do_padconf(struct proc *caller_ptr, message *m_ptr)
{
	повернути bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_VALUE);
}

#endif /* USE_PADCONF */
