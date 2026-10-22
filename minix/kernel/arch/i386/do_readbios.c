/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
ਪੂਰਨ do_readbios(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਬਣਤਰ vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.ਮਾਪ;
  vir_ਬਾਈਟਾਂ ਹੱਦਬੰਦੀ_2;

  src.ਵਿੱਥ = m_ptr->m_lsys_krn_readbios.addr;
  dst.ਵਿੱਥ = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_ਸਰੋਤ;      

  ਹੱਦਬੰਦੀ_2 = src.ਵਿੱਥ + len - 1;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USERRANGE(a, b) SUBRANGE(src.ਵਿੱਥ, ਹੱਦਬੰਦੀ_2, (a), (b))

  ਜੇ(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_ਸਿਰਾ) &&
     !USERRANGE(ਅਧਾਰ_MEM_ਸਿਖਰ, ਉੱਤਲਾ_MEM_ਸਿਰਾ))
  	ਵਾਪਸ EPERM;

  ਵਾਪਸ virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.ਮਾਪ);
}
