/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#अन्तर्भाव "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
पूर्णाङ्क do_readbios(संरचना proc * caller, सन्देशः * m_ptr)
{
  संरचना vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.आकार;
  vir_अष्टकानि सीमा_2;

  src.विस्थापनम् = m_ptr->m_lsys_krn_readbios.addr;
  dst.विस्थापनम् = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_source;      

  सीमा_2 = src.विस्थापनम् + len - 1;

#परिभाषय VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#परिभाषय SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#परिभाषय USERRANGE(a, b) SUBRANGE(src.विस्थापनम्, सीमा_2, (a), (b))

  यदि(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_अन्तः) &&
     !USERRANGE(आधारः_MEM_शिखरम्, ऊर्ध्वस्थम्_MEM_अन्तः))
  	प्रत्यागम EPERM;

  प्रत्यागम virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.आकार);
}
