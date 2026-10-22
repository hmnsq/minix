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

#ប្រសិនបើ USE_UMAP || USE_UMAP_REMOTE

#ប្រសិនបើ ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
ចំនួនគត់ do_umap_remote(sa_nracanaa proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  ចំនួនគត់ seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  ចំនួនគត់ seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  ចំនួនគត់ count = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  ចំនួនគត់ proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  sa_nracanaa proc *targetpr;

  /* Verify process number. */
  ប្រសិនបើ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  បើមិនដូច្នេះ
	ប្រសិនបើ (! isokendpt(endpt, &proc_nr))
		ត្រឡប់(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ប្រសិនបើ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } បើមិនដូច្នេះ ប្រសិនបើ (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ត្រឡប់ EINVAL;
  }

  /* See which mapping should be made. */
  switch(seg_type) {
  pak_sa LOCAL_VM_SEG:
    ប្រសិនបើ(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	ចំនួនគត់ new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ប្រសិនបើ(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, NULL) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                ត្រឡប់ EFAULT;
        }

        ប្រសិនបើ(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ត្រឡប់ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ប្រសិនបើ(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } បើមិនដូច្នេះ {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ត្រឡប់ EFAULT;
      }
      ប្រសិនបើ(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ត្រឡប់ EFAULT;
      }
      ប្រសិនបើ(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ត្រឡប់ EFAULT;
      }
      ប្រសិនបើ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      bha_nga;
  default:
  	printf("umap: peculiar type\n");
  	ត្រឡប់ EINVAL;
  }

  ប្រសិនបើ(vm_running && vm_lookup_range(targetpr, lin_addr, NULL, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ត្រឡប់ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ប្រសិនបើ(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ត្រឡប់ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
