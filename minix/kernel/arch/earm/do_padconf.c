#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_PADCONF

/* get arch specific arch_padconf_set() */
#ଅନ୍ତର୍ଭାଵ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_padconf(ସଂରଚନା proc *caller_ptr, message *m_ptr)
{
	ଫେରନ୍ତୁ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_ମୂଲ୍ୟ);
}

#endif /* USE_PADCONF */
