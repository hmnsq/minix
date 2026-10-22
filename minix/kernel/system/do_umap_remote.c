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

#නම් USE_UMAP || USE_UMAP_REMOTE

#නම් ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
පූර්ණ_සංඛ්‍යා do_umap_remote(sa_nracanaa proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  පූර්ණ_සංඛ්‍යා seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  පූර්ණ_සංඛ්‍යා seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  පූර්ණ_සංඛ්‍යා count = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  පූර්ණ_සංඛ්‍යා proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  sa_nracanaa proc *targetpr;

  /* Verify process number. */
  නම් (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  නැතහොත්
	නම් (! isokendpt(endpt, &proc_nr))
		ආපසු(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  නම් (grantee == SELF) {
	grantee = caller->p_endpoint;
  } නැතහොත් නම් (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ආපසු EINVAL;
  }

  /* See which mapping should be made. */
  switch(seg_type) {
  pak_sa LOCAL_VM_SEG:
    නම්(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	පූර්ණ_සංඛ්‍යා new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        නම්(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, NULL) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                ආපසු EFAULT;
        }

        නම්(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ආපසු EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      නම්(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } නැතහොත් {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ආපසු EFAULT;
      }
      නම්(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ආපසු EFAULT;
      }
      නම්(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ආපසු EFAULT;
      }
      නම්(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      bha_nga;
  default:
  	printf("umap: peculiar type\n");
  	ආපසු EINVAL;
  }

  නම්(vm_running && vm_lookup_range(targetpr, lin_addr, NULL, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ආපසු EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  නම්(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ආපසු (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
