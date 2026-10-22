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

#شامل "kernel/system.h"

#شامل <minix/endpoint.h>

#اگر USE_UMAP

#اگر ! USE_UMAP_REMOTE
#تعریف_منسوخ do_umap_remote
#اگر_ختم

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
صحیح do_umap(ساخت proc * caller, پیغام * m_ptr)
{
  صحیح seg_اشاریہ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_اشاریہ;
  صحیح endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;

  /* This call is a subset of umap_remote, it allows mapping virtual addresses
   * in the caller's address space and grants where the caller is specified as
   * grantee; after the security check we simply invoke do_umap_remote
   */
  اگر (seg_اشاریہ != MEM_GRANT && endpt != SELF) واپس EPERM;
  m_ptr->m_lsys_krn_sys_umap.dst_endpt = SELF;
  واپس do_umap_remote(caller, m_ptr);
}

#اگر_ختم /* USE_UMAP */
