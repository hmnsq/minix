#include "kernel/system.h"
#include <minix/endpoint.h>

#ᱡᱩᱫᱤ USE_PADCONF

/* get arch specific arch_padconf_set() */
#include "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
puur_naa_nka do_padconf(sa_nracanaa proc *caller_ptr, message *m_ptr)
{
	ᱨᱩᱣᱟᱹᱲ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_VALUE);
}

#endif /* USE_PADCONF */
