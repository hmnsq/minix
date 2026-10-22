/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_readbios(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
  ರಚನೆ vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.ಗಾತ್ರ_2;
  vir_ಬೈಟುಗಳು ಎಲ್ಲೆ_2;

  src.ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_readbios.addr;
  dst.ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_ಮೂಲ;      

  ಎಲ್ಲೆ_2 = src.ಸ್ಥಾನಾಂತರ + len - 1;

#ವ್ಯಾಖ್ಯಾನಿಸು VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#ವ್ಯಾಖ್ಯಾನಿಸು SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#ವ್ಯಾಖ್ಯಾನಿಸು USERRANGE(a, b) SUBRANGE(src.ಸ್ಥಾನಾಂತರ, ಎಲ್ಲೆ_2, (a), (b))

  ಆದರೆ(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_ಕೊನೆ) &&
     !USERRANGE(ತಳ_MEM_ತುದಿ, ಊರ್ಧ್ವ_MEM_ಕೊನೆ))
  	ಹಿಂತಿರುಗಿ EPERM;

  ಹಿಂತಿರುಗಿ virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.ಗಾತ್ರ_2);
}
