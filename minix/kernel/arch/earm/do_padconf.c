#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

#إذا USE_PADCONF

/* get arch specific arch_padconf_set() */
#ضمن "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحيح do_padconf(بنية proc *caller_ptr, پیغام *m_ptr)
{
	أرجع bsp_padconf_سېټ(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_ماسک,
	    m_ptr->PADCONF_ارزښت);
}

#د_که_پای /* USE_PADCONF */
