#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_PADCONF

/* get arch specific arch_padconf_set() */
#അന്തര്ഭാവ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_padconf(ഘടന proc *caller_ptr, message *m_ptr)
{
	തിരികെ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_മൂല്യം);
}

#endif /* USE_PADCONF */
