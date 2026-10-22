#समावेश "kernel/system.h"
#समावेश <minix/endpoint.h>

#अगर USE_PADCONF

/* get arch specific arch_padconf_set() */
#समावेश "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
पूर्णांक do_padconf(काष्ठा proc *caller_ptr, message *m_ptr)
{
	वापस bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_मूल्य);
}

#पूर्ण_अगर /* USE_PADCONF */
