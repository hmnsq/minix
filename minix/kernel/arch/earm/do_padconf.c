#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

#إذا USE_PADCONF

/* get arch specific arch_padconf_set() */
#ضمن "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحيح do_padconf(بنية proc *caller_ptr, message *m_ptr)
{
	إرجاع bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_قيمة);
}

#endif /* USE_PADCONF */
