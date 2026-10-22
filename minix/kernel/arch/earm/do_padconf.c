#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/endpoint.h>

#যদি USE_PADCONF

/* get arch specific arch_padconf_set() */
#অন্তর্ভাৱ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
পূর্ণাঙ্ক do_padconf(সংরচনা proc *caller_ptr, message *m_ptr)
{
	ঘূৰাই bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_মান);
}

#endif /* USE_PADCONF */
