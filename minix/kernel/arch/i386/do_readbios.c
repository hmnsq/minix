/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#समावेश "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
पूर्णांक do_readbios(काष्ठा proc * caller, message * m_ptr)
{
  काष्ठा vir_addr src, dst;
  माप_प्रकार len = m_ptr->m_lsys_krn_readbios.आकार;
  vir_bytes limit;

  src.offset = m_ptr->m_lsys_krn_readbios.addr;
  dst.offset = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_source;      

  limit = src.offset + len - 1;

#घोषणा VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#घोषणा SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#घोषणा USERRANGE(a, b) SUBRANGE(src.offset, limit, (a), (b))

  अगर(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_END) &&
     !USERRANGE(BASE_MEM_शीर्ष, UPPER_MEM_END))
  	वापस EPERM;

  वापस virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.आकार);
}
