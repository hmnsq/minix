#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_PADCONF

/* get arch specific arch_padconf_set() */
#அந்தர்பாவ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
முழு do_padconf(கட்டமைப்பு proc *caller_ptr, message *m_ptr)
{
	திரும்பு bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_மதிப்பு);
}

#endif /* USE_PADCONF */
