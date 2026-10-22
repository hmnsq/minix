/* The kernel call implemented in this file:
 *   m_type:	SYS_UMAP_REMOTE
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_umap.src_endpt	(process number)
 *   m_lsys_krn_sys_umap.segment	(segment where address is: T, D, or S)
 *   m_lsys_krn_sys_umap.src_addr	(virtual address)
 *   m_lsys_krn_sys_umap.dst_endpt	(process number of grantee to check access for)
 *   m_krn_lsys_sys_umap.dst_addr	(returns physical address)
 *   m_lsys_krn_sys_umap.nr_bytes	(size of datastructure)
 */

#include "kernel/system.h"

#include <minix/endpoint.h>

#ꦪꦼꦤ꧀ USE_UMAP || USE_UMAP_REMOTE

#ꦪꦼꦤ꧀ ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
puur_naa_nka do_umap_remote(sa_nracanaa proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  puur_naa_nka seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  puur_naa_nka seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  puur_naa_nka count = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  puur_naa_nka proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  sa_nracanaa proc *targetpr;

  /* Verify process number. */
  ꦪꦼꦤ꧀ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ꦭꦶꦪ
	ꦪꦼꦤ꧀ (! isokendpt(endpt, &proc_nr))
		ꦮꦭꦶ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ꦪꦼꦤ꧀ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ꦭꦶꦪ ꦪꦼꦤ꧀ (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ꦮꦭꦶ EINVAL;
  }

  /* See which mapping should be made. */
  switch(seg_type) {
  pak_sa LOCAL_VM_SEG:
    ꦪꦼꦤ꧀(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	puur_naa_nka new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ꦪꦼꦤ꧀(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, NULL) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                ꦮꦭꦶ EFAULT;
        }

        ꦪꦼꦤ꧀(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ꦮꦭꦶ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ꦪꦼꦤ꧀(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } ꦭꦶꦪ {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ꦮꦭꦶ EFAULT;
      }
      ꦪꦼꦤ꧀(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ꦮꦭꦶ EFAULT;
      }
      ꦪꦼꦤ꧀(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ꦮꦭꦶ EFAULT;
      }
      ꦪꦼꦤ꧀(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      bha_nga;
  default:
  	printf("umap: peculiar type\n");
  	ꦮꦭꦶ EINVAL;
  }

  ꦪꦼꦤ꧀(vm_running && vm_lookup_range(targetpr, lin_addr, NULL, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ꦮꦭꦶ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ꦪꦼꦤ꧀(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ꦮꦭꦶ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
