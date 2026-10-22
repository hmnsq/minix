/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *	m_lsys_krn_readbios.size	number of bytes to copy
 *	m_lsys_krn_readbios.addr	absolute address in BIOS area
 *	m_lsys_krn_readbios.buf		buffer address in requesting process
 */

#అన్తర్భావ "kernel/system.h"

/*===========================================================================*
 *				do_readbios				     *
 *===========================================================================*/
పూర్ణాంకం do_readbios(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  నిర్మాణం vir_addr src, dst;
  size_t len = m_ptr->m_lsys_krn_readbios.కొలత;
  vir_బైట్లు హద్దు_2;

  src.స్థానభ్రంశం = m_ptr->m_lsys_krn_readbios.addr;
  dst.స్థానభ్రంశం = m_ptr->m_lsys_krn_readbios.buf;
  src.proc_nr_e = NONE;
  dst.proc_nr_e = m_ptr->m_ఆధారం;      

  హద్దు_2 = src.స్థానభ్రంశం + len - 1;

#నిర్వచించు VINRANGE(v, a, b) ((a) <= (v) && (v) <= (b))
#నిర్వచించు SUBRANGE(a,b,c,d) (VINRANGE((a), (c), (d)) && VINRANGE((b),(c),(d)))
#నిర్వచించు USERRANGE(a, b) SUBRANGE(src.స్థానభ్రంశం, హద్దు_2, (a), (b))

  ఒకవేళ(!USERRANGE(BIOS_MEM_BEGIN, BIOS_MEM_చివర) &&
     !USERRANGE(పునాది_MEM_శిఖరం, పై_MEM_చివర))
  	తిరిగి EPERM;

  తిరిగి virtual_copy_vmcheck(caller, &src, &dst, m_ptr->m_lsys_krn_readbios.కొలత);
}
