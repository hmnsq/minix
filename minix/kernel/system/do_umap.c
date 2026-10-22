/* The kernel call implemented in this file:
 *   m_type:	SYS_UMAP
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_umap.src_endpt	(process number)
 *   m_lsys_krn_sys_umap.segment	(segment where address is: T, D, or S)
 *   m_lsys_krn_sys_umap.src_addr	(virtual address)
 *   m_krn_lsys_sys_umap.dst_addr	(returns physical address)
 *   m_lsys_krn_sys_umap.nr_bytes	(size of datastructure)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_UMAP

#ਜੇ ! USE_UMAP_REMOTE
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ do_umap_remote
#ਜੇ_ਅੰਤ

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
ਪੂਰਨ do_umap(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
  ਪੂਰਨ seg_ਸੂਚਕਾਂਕ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_ਸੂਚਕਾਂਕ;
  ਪੂਰਨ endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;

  /* This call is a subset of umap_remote, it allows mapping virtual addresses
   * in the caller's address space and grants where the caller is specified as
   * grantee; after the security check we simply invoke do_umap_remote
   */
  ਜੇ (seg_ਸੂਚਕਾਂਕ != MEM_GRANT && endpt != SELF) ਵਾਪਸ EPERM;
  m_ptr->m_lsys_krn_sys_umap.dst_endpt = SELF;
  ਵਾਪਸ do_umap_remote(caller, m_ptr);
}

#ਜੇ_ਅੰਤ /* USE_UMAP */
