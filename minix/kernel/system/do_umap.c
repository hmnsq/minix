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

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_UMAP

#ఒకవేళ ! USE_UMAP_REMOTE
#నిర్వచనం_రద్దు do_umap_remote
#అయితే_ముగింపు

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
పూర్ణాంకం do_umap(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
  పూర్ణాంకం seg_సూచీ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_సూచీ;
  పూర్ణాంకం endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;

  /* This call is a subset of umap_remote, it allows mapping virtual addresses
   * in the caller's address space and grants where the caller is specified as
   * grantee; after the security check we simply invoke do_umap_remote
   */
  ఒకవేళ (seg_సూచీ != MEM_GRANT && endpt != SELF) తిరిగి EPERM;
  m_ptr->m_lsys_krn_sys_umap.dst_endpt = SELF;
  తిరిగి do_umap_remote(caller, m_ptr);
}

#అయితే_ముగింపు /* USE_UMAP */
