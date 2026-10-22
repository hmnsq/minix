#شامل "kernel/system.h"
#شامل <minix/endpoint.h>

#اگر USE_PADCONF

/* get arch specific arch_padconf_set() */
#شامل "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحیح do_padconf(ساختار proc *caller_ptr, message *m_ptr)
{
	بازگشت bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_مقدار);
}

#endif /* USE_PADCONF */
