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

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_UMAP || USE_UMAP_REMOTE

#எனில் ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
முழு do_umap_remote(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  முழு seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  முழு seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  முழு மொத்தம் = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  முழு proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  கட்டமைப்பு proc *targetpr;

  /* Verify process number. */
  எனில் (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  இல்லையெனில்
	எனில் (! isokendpt(endpt, &proc_nr))
		திரும்பு(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  எனில் (grantee == SELF) {
	grantee = caller->p_endpoint;
  } இல்லையெனில் எனில் (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	திரும்பு EINVAL;
  }

  /* See which mapping should be made. */
  தேர்வு(seg_type) {
  நிலை LOCAL_VM_SEG:
    எனில்(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	முழு new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        எனில்(verify_grant(targetpr->p_endpoint, grantee, grant, மொத்தம்,
                0, 0, &newoffset, &newep, வெற்று) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, மொத்தம், caller->p_name);
		proc_stacktrace(caller);
                திரும்பு EFAULT;
        }

        எனில்(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                திரும்பு EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      எனில்(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } இல்லையெனில் {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	திரும்பு EFAULT;
      }
      எனில்(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	திரும்பு EFAULT;
      }
      எனில்(vm_lookup(targetpr, lin_addr, &phys_addr, வெற்று) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	திரும்பு EFAULT;
      }
      எனில்(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      நிறுத்து;
  இயல்பு:
  	printf("umap: peculiar type\n");
  	திரும்பு EINVAL;
  }

  எனில்(vm_running && vm_lookup_range(targetpr, lin_addr, வெற்று, மொத்தம்) != மொத்தம்) {
	printf("SYSTEM:do_umap: not contiguous\n");
	திரும்பு EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  எனில்(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  திரும்பு (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
