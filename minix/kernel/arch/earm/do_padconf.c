#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_PADCONF

/* get arch specific arch_padconf_set() */
#અન્તર્ભાવ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
પૂર્ણાંક do_padconf(સંરચના proc *caller_ptr, message *m_ptr)
{
	પાછા bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_કિંમત);
}

#endif /* USE_PADCONF */
