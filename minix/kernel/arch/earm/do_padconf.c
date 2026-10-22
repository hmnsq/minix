#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_PADCONF

/* get arch specific arch_padconf_set() */
#అన్తర్భావ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
పూర్ణాంకం do_padconf(నిర్మాణం proc *caller_ptr, సందేశం *m_ptr)
{
	తిరిగి bsp_padconf_సమితి(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_ముసుగు,
	    m_ptr->PADCONF_విలువ);
}

#అయితే_ముగింపు /* USE_PADCONF */
