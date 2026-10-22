#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_PADCONF

/* get arch specific arch_padconf_set() */
#ಅನ್ತರ್ಭಾವ "bsp_padconf.h"

/*===========================================================================*
 *                                do_padconf                                 *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_padconf(ರಚನೆ proc *caller_ptr, ಸಂದೇಶ *m_ptr)
{
	ಹಿಂತಿರುಗಿ bsp_padconf_ಗಣ(m_ptr->PADCONF_PADCONF, m_ptr->PADCONF_ಮುಖವಾಡ,
	    m_ptr->PADCONF_ಮೌಲ್ಯ);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_PADCONF */
