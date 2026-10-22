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

#समावेश "kernel/system.h"

#समावेश <minix/endpoint.h>

#अगर USE_UMAP || USE_UMAP_REMOTE

#अगर ! USE_UMAP_REMOTE
#अघोषित do_umap_remote
#पूर्ण_अगर

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
पूर्णांक do_umap_remote(काष्ठा proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  पूर्णांक seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  पूर्णांक seg_सूचकांक = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_सूचकांक;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  पूर्णांक गिनती = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  पूर्णांक proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  काष्ठा proc *targetpr;

  /* Verify process number. */
  अगर (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अन्यथा
	अगर (! isokendpt(endpt, &proc_nr))
		वापस(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  अगर (grantee == SELF) {
	grantee = caller->p_endpoint;
  } अन्यथा अगर (grantee == NONE ||
	grantee == ANY ||
	seg_सूचकांक != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	वापस EINVAL;
  }

  /* See which mapping should be made. */
  चयन(seg_type) {
  हाल LOCAL_VM_SEG:
    अगर(seg_सूचकांक == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	पूर्णांक new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        अगर(verify_grant(targetpr->p_endpoint, grantee, grant, गिनती,
                0, 0, &newoffset, &newep, शून्य) != OK) {
                म_लिखो("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, गिनती, caller->p_name);
		proc_stacktrace(caller);
                वापस EFAULT;
        }

        अगर(!isokendpt(newep, &new_proc_nr)) {
                म_लिखो("SYSTEM: do_umap: isokendpt failed\n");
                वापस EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_सूचकांक = VIR_ADDR;
      }

      अगर(seg_सूचकांक == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } अन्यथा {
	म_लिखो("SYSTEM: bogus seg type 0x%lx\n", seg_सूचकांक);
	वापस EFAULT;
      }
      अगर(!lin_addr) {
	म_लिखो("SYSTEM:do_umap: umap_local failed\n");
	वापस EFAULT;
      }
      अगर(vm_lookup(targetpr, lin_addr, &phys_addr, शून्य) != OK) {
	म_लिखो("SYSTEM:do_umap: vm_lookup failed\n");
	वापस EFAULT;
      }
      अगर(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      अवरोध;
  शेष:
  	म_लिखो("umap: peculiar type\n");
  	वापस EINVAL;
  }

  अगर(vm_running && vm_lookup_range(targetpr, lin_addr, शून्य, गिनती) != गिनती) {
	म_लिखो("SYSTEM:do_umap: not contiguous\n");
	वापस EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  अगर(phys_addr == 0) {
	  म_लिखो("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	म_लिखो("caller stack: ");
	proc_stacktrace(caller);
  }
  वापस (phys_addr == 0) ? EFAULT: OK;
}

#पूर्ण_अगर /* USE_UMAP || USE_UMAP_REMOTE */
