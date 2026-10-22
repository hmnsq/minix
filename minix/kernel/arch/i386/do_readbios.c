/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_readbios(ସଂରଚନା proc * caller, message * m_ptr)
{
  ସଂରଚନା vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.size;
  vir_bytes limit;

  src.offset = m_ptr->m_lsys_krn_readbios.addr;
  dst.offset = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_source;      

  limit = src.offset + len - 1;

#define VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#define SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#define USERRANGE(a, b) SUBRANGE(src.offset, limit, (a), (b))

  ଯଦି(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_END) &&
     !USERRANGE(BASE_MEM_ଶୀର୍ଷ, UPPER_MEM_END))
  	ଫେରନ୍ତୁ EPERM;

  ଫେରନ୍ତୁ virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.size);
}
