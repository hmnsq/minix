#include "kernel/system.h"
#include <minix/endpoint.h>

#ຖ້າ USE_PADCONF

/* get arch specific arch_padconf_set() */
#include "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ຈຳນວນເຕັມ do_padconf(sa_nracanaa proc *caller_ptr, message *m_ptr)
{
	ສົ່ງຄືນ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_VALUE);
}

#endif /* USE_PADCONF */
