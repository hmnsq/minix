#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/endpoint.h>

#यदि USE_PADCONF

/* get arch specific arch_padconf_set() */
#अन्तर्भाव "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
पूर्णाङ्क do_padconf(संरचना proc *caller_ptr, सन्देशः *m_ptr)
{
	प्रत्यागम bsp_padconf_समुच्चयः(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_आवरणम्,
	    m_ptr->PADCONF_मानम्);
}

#यदि_अन्तः /* USE_PADCONF */
