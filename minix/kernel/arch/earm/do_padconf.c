#شامل "kernel/system.h"
#شامل <minix/endpoint.h>

#اگر USE_PADCONF

/* get arch specific arch_padconf_set() */
#شامل "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
صحیح do_padconf(ساخت proc *caller_ptr, پیغام *m_ptr)
{
	واپس bsp_padconf_سیٹ(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_نقاب,
	    m_ptr->PADCONF_قدر);
}

#اگر_ختم /* USE_PADCONF */
