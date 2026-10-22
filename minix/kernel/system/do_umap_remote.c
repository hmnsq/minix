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

#ಯದಿ USE_UMAP || USE_UMAP_REMOTE

#ಯದಿ ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_umap_remote(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  ಪೂರ್ಣಾಙ್ಕ seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  ಪೂರ್ಣಾಙ್ಕ seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  ಪೂರ್ಣಾಙ್ಕ proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  ಸಂರಚನಾ proc *targetpr;

  /* Verify process number. */
  ಯದಿ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ಅನ್ಯಥಾ
	ಯದಿ (! isokendpt(endpt, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ಯದಿ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ಅನ್ಯಥಾ ಯದಿ (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ಪ್ರತ್ಯಾಗಮ EINVAL;
  }

  /* See which mapping should be made. */
  ವಿಕಲ್ಪ(seg_type) {
  ಪಕ್ಷ LOCAL_VM_SEG:
    ಯದಿ(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	ಪೂರ್ಣಾಙ್ಕ new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        ಯದಿ(verify_grant(targetpr->p_endpoint, grantee, grant, ಎಣಿಕೆ,
                0, 0, &newoffset, &newep, ರಿಕ್ತ) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, ಎಣಿಕೆ, caller->p_name);
		proc_stacktrace(caller);
                ಪ್ರತ್ಯಾಗಮ EFAULT;
        }

        ಯದಿ(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                ಪ್ರತ್ಯಾಗಮ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      ಯದಿ(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } ಅನ್ಯಥಾ {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	ಪ್ರತ್ಯಾಗಮ EFAULT;
      }
      ಯದಿ(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	ಪ್ರತ್ಯಾಗಮ EFAULT;
      }
      ಯದಿ(vm_lookup(targetpr, lin_addr, &phys_addr, ರಿಕ್ತ) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	ಪ್ರತ್ಯಾಗಮ EFAULT;
      }
      ಯದಿ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      ಭಙ್ಗ;
  ಸ್ವಭಾವ:
  	printf("umap: peculiar type\n");
  	ಪ್ರತ್ಯಾಗಮ EINVAL;
  }

  ಯದಿ(vm_running && vm_lookup_range(targetpr, lin_addr, ರಿಕ್ತ, ಎಣಿಕೆ) != ಎಣಿಕೆ) {
	printf("SYSTEM:do_umap: not contiguous\n");
	ಪ್ರತ್ಯಾಗಮ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ಯದಿ(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  ಪ್ರತ್ಯಾಗಮ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
