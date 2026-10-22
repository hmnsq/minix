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

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <minix/endpoint.h>

#सचे USE_UMAP || USE_UMAP_REMOTE

#सचे ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
पूर्णाङ्क do_umap_remote(संरचना proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  पूर्णाङ्क seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  पूर्णाङ्क seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  पूर्णाङ्क गणनम् = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  पूर्णाङ्क proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  संरचना proc *targetpr;

  /* Verify process number. */
  सचे (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अञ्ञथा
	सचे (! isokendpt(endpt, &proc_nr))
		पटिनिवत्त(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  सचे (grantee == SELF) {
	grantee = caller->p_endpoint;
  } अञ्ञथा सचे (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	पटिनिवत्त EINVAL;
  }

  /* See which mapping should be made. */
  विकल्प(seg_type) {
  पक्ष LOCAL_VM_SEG:
    सचे(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	पूर्णाङ्क new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        सचे(verify_grant(targetpr->p_endpoint, grantee, grant, गणनम्,
                0, 0, &newoffset, &newep, रिक्त) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, गणनम्, caller->p_name);
		proc_stacktrace(caller);
                पटिनिवत्त EFAULT;
        }

        सचे(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                पटिनिवत्त EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      सचे(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } अञ्ञथा {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	पटिनिवत्त EFAULT;
      }
      सचे(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	पटिनिवत्त EFAULT;
      }
      सचे(vm_lookup(targetpr, lin_addr, &phys_addr, रिक्त) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	पटिनिवत्त EFAULT;
      }
      सचे(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      भङ्ग;
  स्वभाव:
  	printf("umap: peculiar type\n");
  	पटिनिवत्त EINVAL;
  }

  सचे(vm_running && vm_lookup_range(targetpr, lin_addr, रिक्त, गणनम्) != गणनम्) {
	printf("SYSTEM:do_umap: not contiguous\n");
	पटिनिवत्त EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  सचे(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  पटिनिवत्त (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
