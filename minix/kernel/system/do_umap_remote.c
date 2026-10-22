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

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_UMAP || USE_UMAP_REMOTE

#ఒకవేళ ! USE_UMAP_REMOTE
#నిర్వచనం_రద్దు do_umap_remote
#అయితే_ముగింపు

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
పూర్ణాంకం do_umap_remote(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  పూర్ణాంకం seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  పూర్ణాంకం seg_సూచీ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_సూచీ;
  vir_బైట్లు స్థానభ్రంశం = m_ptr->m_lsys_krn_sys_umap.src_addr;
  పూర్ణాంకం లెక్క = m_ptr->m_lsys_krn_sys_umap.nr_బైట్లు;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  పూర్ణాంకం proc_nr, proc_nr_grantee;
  phys_బైట్లు phys_addr = 0, lin_addr = 0;
  నిర్మాణం proc *targetpr;

  /* Verify process number. */
  ఒకవేళ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  లేకపోతే
	ఒకవేళ (! isokendpt(endpt, &proc_nr))
		తిరిగి(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ఒకవేళ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } లేకపోతే ఒకవేళ (grantee == NONE ||
	grantee == ANY ||
	seg_సూచీ != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	తిరిగి EINVAL;
  }

  /* See which mapping should be made. */
  ఎంపిక(seg_type) {
  స్థితి LOCAL_VM_SEG:
    ఒకవేళ(seg_సూచీ == MEM_GRANT) {
	vir_బైట్లు newoffset;
	endpoint_t newep;
	పూర్ణాంకం new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) స్థానభ్రంశం;

        ఒకవేళ(verify_grant(targetpr->p_endpoint, grantee, grant, లెక్క,
                0, 0, &newoffset, &newep, ఖాళీ) != OK) {
                రాయి("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, స్థానభ్రంశం, లెక్క, caller->p_name);
		proc_stacktrace(caller);
                తిరిగి EFAULT;
        }

        ఒకవేళ(!isokendpt(newep, &new_proc_nr)) {
                రాయి("SYSTEM: do_umap: isokendpt failed\n");
                తిరిగి EFAULT;
        }

	/* New lookup. */
	స్థానభ్రంశం = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_సూచీ = VIR_ADDR;
      }

      ఒకవేళ(seg_సూచీ == VIR_ADDR) {
        phys_addr = lin_addr = స్థానభ్రంశం;
      } లేకపోతే {
	రాయి("SYSTEM: bogus seg type 0x%lx\n", seg_సూచీ);
	తిరిగి EFAULT;
      }
      ఒకవేళ(!lin_addr) {
	రాయి("SYSTEM:do_umap: umap_local failed\n");
	తిరిగి EFAULT;
      }
      ఒకవేళ(vm_lookup(targetpr, lin_addr, &phys_addr, ఖాళీ) != OK) {
	రాయి("SYSTEM:do_umap: vm_lookup failed\n");
	తిరిగి EFAULT;
      }
      ఒకవేళ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      ఆపు;
  అప్రమేయ:
  	రాయి("umap: peculiar type\n");
  	తిరిగి EINVAL;
  }

  ఒకవేళ(vm_running && vm_lookup_range(targetpr, lin_addr, ఖాళీ, లెక్క) != లెక్క) {
	రాయి("SYSTEM:do_umap: not contiguous\n");
	తిరిగి EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ఒకవేళ(phys_addr == 0) {
	  రాయి("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, స్థానభ్రంశం, phys_addr);
	రాయి("caller stack: ");
	proc_stacktrace(caller);
  }
  తిరిగి (phys_addr == 0) ? EFAULT: OK;
}

#అయితే_ముగింపు /* USE_UMAP || USE_UMAP_REMOTE */
