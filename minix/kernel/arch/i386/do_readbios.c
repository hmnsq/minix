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
صحيح do_readbios(بنية proc * caller, پیغام * m_ptr)
{
  بنية vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.اندازه;
  vir_بایټونه حد_2;

  src.دننه_واټن = m_ptr->m_lsys_krn_readbios.addr;
  dst.دننه_واټن = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_سرچینه;      

  حد_2 = src.دننه_واټن + len - 1;

#تعریف_کړه VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#تعریف_کړه SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#تعریف_کړه USERRANGE(a, b) SUBRANGE(src.دننه_واټن, حد_2, (a), (b))

  إذا(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_پای_ټکی) &&
     !USERRANGE(بنسټ_MEM_سر, پاسنی_MEM_پای_ټکی))
  	أرجع EPERM;

  أرجع virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.اندازه);
}
