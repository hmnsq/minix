#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಯದಿ USE_PADCONF

/* get arch specific arch_padconf_set() */
#ಅನ್ತರ್ಭಾವ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_padconf(ಸಂರಚನಾ proc *caller_ptr, message *m_ptr)
{
	ಪ್ರತ್ಯಾಗಮ bsp_padconf_set(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_MASK,
	    m_ptr->PADCONF_ಮೌಲ್ಯ);
}

#endif /* USE_PADCONF */
