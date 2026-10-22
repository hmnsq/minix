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

#समावेश "kernel/system.h"

#समावेश <minix/endpoint.h>

#अगर USE_UMAP

#अगर ! USE_UMAP_REMOTE
#अघोषित do_umap_remote
#पूर्ण_अगर

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
पूर्णांक do_umap(काष्ठा proc * caller, message * m_ptr)
{
  पूर्णांक seg_सूचकांक = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_सूचकांक;
  पूर्णांक endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;

  /* This call is a subset of umap_remote, it allows mapping virtual addresses
   * in the caller's address space and grants where the caller is specified as
   * grantee; after the security check we simply invoke do_umap_remote
   */
  अगर (seg_सूचकांक != MEM_GRANT && endpt != SELF) वापस EPERM;
  m_ptr->m_lsys_krn_sys_umap.dst_endpt = SELF;
  वापस do_umap_remote(caller, m_ptr);
}

#पूर्ण_अगर /* USE_UMAP */
