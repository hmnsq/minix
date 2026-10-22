#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_PADCONF

/* get arch specific arch_padconf_set() */
#অন্তর্ভাব "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
পূর্ণাঙ্ক do_padconf(সংরচনা proc *caller_ptr, message *m_ptr)
{
	প্রত্যাগম bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_মান);
}

#endif /* USE_PADCONF */
