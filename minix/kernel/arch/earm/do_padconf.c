#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

#إذا USE_PADCONF

/* get arch specific arch_padconf_set() */
#ضمن "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحيح do_padconf(بنية proc *caller_ptr, پيغام *m_ptr)
{
	أرجع bsp_padconf_سيٽ(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_نقاب,
	    m_ptr->PADCONF_قدر);
}

#جيڪڏهن_پورو /* USE_PADCONF */
