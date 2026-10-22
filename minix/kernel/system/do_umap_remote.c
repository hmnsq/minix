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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_UMAP || USE_UMAP_REMOTE

#ಆದರೆ ! USE_UMAP_REMOTE
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು do_umap_remote
#ಆದರೆ_ಅಂತ್ಯ

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
ಪೂರ್ಣಾಂಕ do_umap_remote(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  ಪೂರ್ಣಾಂಕ seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  ಪೂರ್ಣಾಂಕ seg_ಸೂಚ್ಯಂಕ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_ಸೂಚ್ಯಂಕ;
  vir_ಬೈಟುಗಳು ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_sys_umap.src_addr;
  ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = m_ptr->m_lsys_krn_sys_umap.nr_ಬೈಟುಗಳು;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  ಪೂರ್ಣಾಂಕ proc_nr, proc_nr_grantee;
  phys_ಬೈಟುಗಳು phys_addr = 0, lin_addr = 0;
  ರಚನೆ proc *targetpr;

  /* Verify process number. */
  ಆದರೆ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (! isokendpt(endpt, &proc_nr))
		ಹಿಂತಿರುಗಿ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ಆದರೆ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (grantee == NONE ||
	grantee == ANY ||
	seg_ಸೂಚ್ಯಂಕ != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ಹಿಂತಿರುಗಿ EINVAL;
  }

  /* See which mapping should be made. */
  ಆಯ್ಕೆ(seg_type) {
  ಸ್ಥಿತಿ LOCAL_VM_SEG:
    ಆದರೆ(seg_ಸೂಚ್ಯಂಕ == MEM_GRANT) {
	vir_ಬೈಟುಗಳು newoffset;
	endpoint_t newep;
	ಪೂರ್ಣಾಂಕ new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) ಸ್ಥಾನಾಂತರ;

        ಆದರೆ(verify_grant(targetpr->p_endpoint, grantee, grant, ಎಣಿಕೆ_2,
                0, 0, &newoffset, &newep, ಖಾಲಿ) != OK) {
                ಬರೆ("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, ಸ್ಥಾನಾಂತರ, ಎಣಿಕೆ_2, caller->p_name);
		proc_stacktrace(caller);
                ಹಿಂತಿರುಗಿ EFAULT;
        }

        ಆದರೆ(!isokendpt(newep, &new_proc_nr)) {
                ಬರೆ("SYSTEM: do_umap: isokendpt failed\n");
                ಹಿಂತಿರುಗಿ EFAULT;
        }

	/* New lookup. */
	ಸ್ಥಾನಾಂತರ = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_ಸೂಚ್ಯಂಕ = VIR_ADDR;
      }

      ಆದರೆ(seg_ಸೂಚ್ಯಂಕ == VIR_ADDR) {
        phys_addr = lin_addr = ಸ್ಥಾನಾಂತರ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	ಬರೆ("SYSTEM: bogus seg type 0x%lx\n", seg_ಸೂಚ್ಯಂಕ);
	ಹಿಂತಿರುಗಿ EFAULT;
      }
      ಆದರೆ(!lin_addr) {
	ಬರೆ("SYSTEM:do_umap: umap_local failed\n");
	ಹಿಂತಿರುಗಿ EFAULT;
      }
      ಆದರೆ(vm_lookup(targetpr, lin_addr, &phys_addr, ಖಾಲಿ) != OK) {
	ಬರೆ("SYSTEM:do_umap: vm_lookup failed\n");
	ಹಿಂತಿರುಗಿ EFAULT;
      }
      ಆದರೆ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
  	ಬರೆ("umap: peculiar type\n");
  	ಹಿಂತಿರುಗಿ EINVAL;
  }

  ಆದರೆ(vm_running && vm_lookup_range(targetpr, lin_addr, ಖಾಲಿ, ಎಣಿಕೆ_2) != ಎಣಿಕೆ_2) {
	ಬರೆ("SYSTEM:do_umap: not contiguous\n");
	ಹಿಂತಿರುಗಿ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ಆದರೆ(phys_addr == 0) {
	  ಬರೆ("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, ಸ್ಥಾನಾಂತರ, phys_addr);
	ಬರೆ("caller stack: ");
	proc_stacktrace(caller);
  }
  ಹಿಂತಿರುಗಿ (phys_addr == 0) ? EFAULT: OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_UMAP || USE_UMAP_REMOTE */
