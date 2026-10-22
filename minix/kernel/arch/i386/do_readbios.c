/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#অন্তর্ভাব "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_readbios(কাঠামো proc * caller, message * m_ptr)
{
  কাঠামো vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.আকার_2;
  vir_bytes limit;

  src.offset = m_ptr->m_lsys_krn_readbios.addr;
  dst.offset = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_source;      

  limit = src.offset + len - 1;

#define VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#define SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#define USERRANGE(a, b) SUBRANGE(src.offset, limit, (a), (b))

  যদি(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_END) &&
     !USERRANGE(BASE_MEM_শীর্ষ, UPPER_MEM_END))
  	ফেরত EPERM;

  ফেরত virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.আকার_2);
}
