/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#ضمن "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
صحيح do_readbios(بنية proc * caller, پيغام * m_ptr)
{
  بنية vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.سائيز;
  vir_بائيٽون حد_بندي_2;

  src.وٿي = m_ptr->m_lsys_krn_readbios.addr;
  dst.وٿي = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_ذريعو;      

  حد_بندي_2 = src.وٿي + len - 1;

#وصف_ڪريو VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#وصف_ڪريو SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#وصف_ڪريو USERRANGE(a, b) SUBRANGE(src.وٿي, حد_بندي_2, (a), (b))

  إذا(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_پڇاڙي) &&
     !USERRANGE(بنياد_MEM_چوٽي, مٿانهون_MEM_پڇاڙي))
  	أرجع EPERM;

  أرجع virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.سائيز);
}
