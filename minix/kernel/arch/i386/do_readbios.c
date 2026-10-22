/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#شامل "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
صحیح do_readbios(ساخت proc * caller, پیغام * m_ptr)
{
  ساخت vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.سائز;
  vir_بائٹیں حد_بندی_2;

  src.انحراف = m_ptr->m_lsys_krn_readbios.addr;
  dst.انحراف = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_ماخذ;      

  حد_بندی_2 = src.انحراف + len - 1;

#تعریف_کرو VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#تعریف_کرو SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#تعریف_کرو USERRANGE(a, b) SUBRANGE(src.انحراف, حد_بندی_2, (a), (b))

  اگر(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_انتہا) &&
     !USERRANGE(بنیاد_MEM_چوٹی, بالائی_MEM_انتہا))
  	واپس EPERM;

  واپس virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.سائز);
}
