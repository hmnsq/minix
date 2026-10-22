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

#ضمن "kernel/system.h"

#ضمن <minix/endpoint.h>

#אם USE_UMAP || USE_UMAP_REMOTE

#אם ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
صحيح do_umap_remote(بنية proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  صحيح seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  صحيح seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  صحيح count = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  صحيح proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  بنية proc *targetpr;

  /* Verify process number. */
  אם (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  אחרת
	אם (! isokendpt(endpt, &proc_nr))
		החזר(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  אם (grantee == SELF) {
	grantee = caller->p_endpoint;
  } אחרת אם (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	החזר EINVAL;
  }

  /* See which mapping should be made. */
  بدل(seg_type) {
  حالة LOCAL_VM_SEG:
    אם(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	صحيح new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        אם(verify_grant(targetpr->p_endpoint, grantee, grant, count,
                0, 0, &newoffset, &newep, عدم) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                החזר EFAULT;
        }

        אם(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                החזר EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      אם(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } אחרת {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	החזר EFAULT;
      }
      אם(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	החזר EFAULT;
      }
      אם(vm_lookup(targetpr, lin_addr, &phys_addr, عدم) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	החזר EFAULT;
      }
      אם(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      اكسر;
  افتراضي:
  	printf("umap: peculiar type\n");
  	החזר EINVAL;
  }

  אם(vm_running && vm_lookup_range(targetpr, lin_addr, عدم, count) != count) {
	printf("SYSTEM:do_umap: not contiguous\n");
	החזר EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  אם(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  החזר (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
