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

#यदि USE_UMAP || USE_UMAP_REMOTE

#यदि ! USE_UMAP_REMOTE
#अपरिभाषय do_umap_remote
#यदि_अन्तः

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
पूर्णाङ्क do_umap_remote(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  पूर्णाङ्क seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  पूर्णाङ्क seg_सूचकांक = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_सूचकांक;
  vir_अष्टकानि विस्थापनम् = m_ptr->m_lsys_krn_sys_umap.src_addr;
  पूर्णाङ्क गणनम् = m_ptr->m_lsys_krn_sys_umap.nr_अष्टकानि;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  पूर्णाङ्क proc_nr, proc_nr_grantee;
  phys_अष्टकानि phys_addr = 0, lin_addr = 0;
  संरचना proc *targetpr;

  /* Verify process number. */
  यदि (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अन्यथा
	यदि (! isokendpt(endpt, &proc_nr))
		प्रत्यागम(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  यदि (grantee == SELF) {
	grantee = caller->p_endpoint;
  } अन्यथा यदि (grantee == NONE ||
	grantee == ANY ||
	seg_सूचकांक != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	प्रत्यागम EINVAL;
  }

  /* See which mapping should be made. */
  विकल्प(seg_type) {
  पक्ष LOCAL_VM_SEG:
    यदि(seg_सूचकांक == MEM_GRANT) {
	vir_अष्टकानि newoffset;
	endpoint_t newep;
	पूर्णाङ्क new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) विस्थापनम्;

        यदि(verify_grant(targetpr->p_endpoint, grantee, grant, गणनम्,
                0, 0, &newoffset, &newep, रिक्त) != OK) {
                प्रारूप_लिख("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, विस्थापनम्, गणनम्, caller->p_name);
		proc_stacktrace(caller);
                प्रत्यागम EFAULT;
        }

        यदि(!isokendpt(newep, &new_proc_nr)) {
                प्रारूप_लिख("SYSTEM: do_umap: isokendpt failed\n");
                प्रत्यागम EFAULT;
        }

	/* New lookup. */
	विस्थापनम् = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_सूचकांक = VIR_ADDR;
      }

      यदि(seg_सूचकांक == VIR_ADDR) {
        phys_addr = lin_addr = विस्थापनम्;
      } अन्यथा {
	प्रारूप_लिख("SYSTEM: bogus seg type 0x%lx\n", seg_सूचकांक);
	प्रत्यागम EFAULT;
      }
      यदि(!lin_addr) {
	प्रारूप_लिख("SYSTEM:do_umap: umap_local failed\n");
	प्रत्यागम EFAULT;
      }
      यदि(vm_lookup(targetpr, lin_addr, &phys_addr, रिक्त) != OK) {
	प्रारूप_लिख("SYSTEM:do_umap: vm_lookup failed\n");
	प्रत्यागम EFAULT;
      }
      यदि(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      भङ्ग;
  स्वभाव:
  	प्रारूप_लिख("umap: peculiar type\n");
  	प्रत्यागम EINVAL;
  }

  यदि(vm_running && vm_lookup_range(targetpr, lin_addr, रिक्त, गणनम्) != गणनम्) {
	प्रारूप_लिख("SYSTEM:do_umap: not contiguous\n");
	प्रत्यागम EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  यदि(phys_addr == 0) {
	  प्रारूप_लिख("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, विस्थापनम्, phys_addr);
	प्रारूप_लिख("caller stack: ");
	proc_stacktrace(caller);
  }
  प्रत्यागम (phys_addr == 0) ? EFAULT: OK;
}

#यदि_अन्तः /* USE_UMAP || USE_UMAP_REMOTE */
