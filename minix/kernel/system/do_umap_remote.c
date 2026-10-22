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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_UMAP || USE_UMAP_REMOTE

#ଯଦି ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_umap_remote(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  ସଂରଚନା proc *targetpr;

  /* Verify process number. */
  ଯଦି (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ଅନ୍ୟଥା
	ଯଦି (! isokendpt(endpt, &proc_nr))
		ଫେରନ୍ତୁ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ଯଦି (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ଅନ୍ୟଥା ଯଦି (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ଫେରନ୍ତୁ EINVAL;
  }

  /* See which mapping should be made. */
  ବାଛ(seg_type) {
  ସ୍ଥିତି LOCAL_VM_SEG:
    ଯଦି(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ଯଦି(verify_grant(targetpr->p_endpoint, grantee, grant, ଗଣନା_2,
                0, 0, &newoffset, &newep, ଖାଲି) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, ଗଣନା_2, caller->p_name);
		proc_stacktrace(caller);
                ଫେରନ୍ତୁ EFAULT;
        }

        ଯଦି(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ଫେରନ୍ତୁ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ଯଦି(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } ଅନ୍ୟଥା {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ଫେରନ୍ତୁ EFAULT;
      }
      ଯଦି(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ଫେରନ୍ତୁ EFAULT;
      }
      ଯଦି(vm_lookup(targetpr, lin_addr, &phys_addr, ଖାଲି) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ଫେରନ୍ତୁ EFAULT;
      }
      ଯଦି(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
  	printf("umap: peculiar type\n");
  	ଫେରନ୍ତୁ EINVAL;
  }

  ଯଦି(vm_running && vm_lookup_range(targetpr, lin_addr, ଖାଲି, ଗଣନା_2) != ଗଣନା_2) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ଫେରନ୍ତୁ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ଯଦି(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ଫେରନ୍ତୁ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
