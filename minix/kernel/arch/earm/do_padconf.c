#ضمن "kernel/system.h"
#ضمن <minix/endpoint.h>

#إذا USE_PADCONF

/* get arch specific arch_padconf_set() */
#ضمن "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحيح do_padconf(بنية proc *caller_ptr, پیام *m_ptr)
{
	أرجع bsp_padconf_مجموعه(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_ماسک,
	    m_ptr->PADCONF_مقدار);
}

#پایان_اگر /* USE_PADCONF */
