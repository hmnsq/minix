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

#ပေႃး USE_UMAP || USE_UMAP_REMOTE

#ပေႃး ! USE_UMAP_REMOTE
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
  ပေႃး (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ပေႃးဢမ်ႇ
	ပေႃး (! isokendpt(endpt, &proc_nr))
		ပွၵ်ႈ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ပေႃး (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ပေႃးဢမ်ႇ ပေႃး (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ပွၵ်ႈ EINVAL;
  }

  /* See which mapping should be made. */
  switch(seg_type) {
  pak_sa LOCAL_VM_SEG:
    ပေႃး(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	puur_naa_nka new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ပေႃး(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, NULL) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                ပွၵ်ႈ EFAULT;
        }

        ပေႃး(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ပွၵ်ႈ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ပေႃး(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } ပေႃးဢမ်ႇ {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ပွၵ်ႈ EFAULT;
      }
      ပေႃး(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ပွၵ်ႈ EFAULT;
      }
      ပေႃး(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ပွၵ်ႈ EFAULT;
      }
      ပေႃး(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      bha_nga;
  default:
  	printf("umap: peculiar type\n");
  	ပွၵ်ႈ EINVAL;
  }

  ပေႃး(vm_running && vm_lookup_range(targetpr, lin_addr, NULL, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ပွၵ်ႈ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ပေႃး(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ပွၵ်ႈ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
