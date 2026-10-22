#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

#यदि USE_PADCONF

/* get arch specific arch_padconf_set() */
#अन्तर्भाव "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
पूर्णाङ्क do_padconf(संरचना proc *caller_ptr, message *m_ptr)
{
	फर्काऊ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_मान);
}

#endif /* USE_PADCONF */
