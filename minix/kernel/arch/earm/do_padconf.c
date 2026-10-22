#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_PADCONF

/* get arch specific arch_padconf_set() */
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ਪੂਰਨ do_padconf(ਬਣਤਰ proc *caller_ptr, ਸੁਨੇਹਾ *m_ptr)
{
	ਵਾਪਸ bsp_padconf_ਸੈੱਟ(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_ਨਕਾਬ,
	    m_ptr->PADCONF_ਮੁੱਲ);
}

#ਜੇ_ਅੰਤ /* USE_PADCONF */
