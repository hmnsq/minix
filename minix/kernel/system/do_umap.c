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

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_UMAP

#எனில் ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
முழு do_umap(கட்டமைப்பு proc * caller, message * m_ptr)
{
  முழு seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  முழு endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;

  /* This call is a subset of umap_remote, it allows mapping virtual addresses
   * in the caller's address space and grants where the caller is specified as
   * grantee; after the security check we simply invoke do_umap_remote
   */
  எனில் (seg_index != MEM_GRANT && endpt != SELF) திரும்பு EPERM;
  m_ptr->m_lsys_krn_sys_umap.dst_endpt = SELF;
  திரும்பு do_umap_remote(caller, m_ptr);
}

#endif /* USE_UMAP */
