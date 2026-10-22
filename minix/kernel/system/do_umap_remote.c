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

#जर USE_UMAP || USE_UMAP_REMOTE

#जर ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
पूर्णांक do_umap_remote(संरचना proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  पूर्णांक seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  पूर्णांक seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  पूर्णांक मोजणी = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  पूर्णांक proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  संरचना proc *targetpr;

  /* Verify process number. */
  जर (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अन्यथा
	जर (! isokendpt(endpt, &proc_nr))
		परत(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  जर (grantee == SELF) {
	grantee = caller->p_endpoint;
  } अन्यथा जर (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	परत EINVAL;
  }

  /* See which mapping should be made. */
  निवड(seg_type) {
  स्थिती LOCAL_VM_SEG:
    जर(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	पूर्णांक new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        जर(verify_grant(targetpr->p_endpoint, grantee, grant, मोजणी,
                0, 0, &newoffset, &newep, रिक्त) != OK) {
                म_लिखो("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, मोजणी, caller->p_name);
		proc_stacktrace(caller);
                परत EFAULT;
        }

        जर(!isokendpt(newep, &new_proc_nr)) {
                म_लिखो("SYSTEM: do_umap: isokendpt failed\n");
                परत EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      जर(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } अन्यथा {
	म_लिखो("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	परत EFAULT;
      }
      जर(!lin_addr) {
	म_लिखो("SYSTEM:do_umap: umap_local failed\n");
	परत EFAULT;
      }
      जर(vm_lookup(targetpr, lin_addr, &phys_addr, रिक्त) != OK) {
	म_लिखो("SYSTEM:do_umap: vm_lookup failed\n");
	परत EFAULT;
      }
      जर(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      थांबा;
  पूर्वनिर्धारित:
  	म_लिखो("umap: peculiar type\n");
  	परत EINVAL;
  }

  जर(vm_running && vm_lookup_range(targetpr, lin_addr, रिक्त, मोजणी) != मोजणी) {
	म_लिखो("SYSTEM:do_umap: not contiguous\n");
	परत EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  जर(phys_addr == 0) {
	  म_लिखो("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	म_लिखो("caller stack: ");
	proc_stacktrace(caller);
  }
  परत (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
