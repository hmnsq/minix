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

#ако USE_UMAP || USE_UMAP_REMOTE

#ако ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
цео do_umap_remote(struct proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  цео seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  цео seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  цео count = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  цео proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  struct proc *targetpr;

  /* Verify process number. */
  ако (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  инако
	ако (! isokendpt(endpt, &proc_nr))
		врати(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ако (grantee == SELF) {
	grantee = caller->p_endpoint;
  } инако ако (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	врати EINVAL;
  }

  /* See which mapping should be made. */
  switch(seg_type) {
  case LOCAL_VM_SEG:
    ако(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	цео new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ако(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, NULL) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                врати EFAULT;
        }

        ако(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                врати EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ако(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } инако {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	врати EFAULT;
      }
      ако(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	врати EFAULT;
      }
      ако(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	врати EFAULT;
      }
      ако(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      break;
  default:
  	printf("umap: peculiar type\n");
  	врати EINVAL;
  }

  ако(vm_running && vm_lookup_range(targetpr, lin_addr, NULL, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	врати EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ако(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  врати (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
