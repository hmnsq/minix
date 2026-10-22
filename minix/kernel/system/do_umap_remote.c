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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_UMAP || USE_UMAP_REMOTE

#ਜੇ ! USE_UMAP_REMOTE
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ do_umap_remote
#ਜੇ_ਅੰਤ

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
ਪੂਰਨ do_umap_remote(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  ਪੂਰਨ seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  ਪੂਰਨ seg_ਸੂਚਕਾਂਕ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_ਸੂਚਕਾਂਕ;
  vir_ਬਾਈਟਾਂ ਵਿੱਥ = m_ptr->m_lsys_krn_sys_umap.src_addr;
  ਪੂਰਨ ਗਿਣਤੀ_2 = m_ptr->m_lsys_krn_sys_umap.nr_ਬਾਈਟਾਂ;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  ਪੂਰਨ proc_nr, proc_nr_grantee;
  phys_ਬਾਈਟਾਂ phys_addr = 0, lin_addr = 0;
  ਬਣਤਰ proc *targetpr;

  /* Verify process number. */
  ਜੇ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ਨਹੀਂਤਾਂ
	ਜੇ (! isokendpt(endpt, &proc_nr))
		ਵਾਪਸ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  ਜੇ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ਨਹੀਂਤਾਂ ਜੇ (grantee == NONE ||
	grantee == ANY ||
	seg_ਸੂਚਕਾਂਕ != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	ਵਾਪਸ EINVAL;
  }

  /* See which mapping should be made. */
  ਚੋਣ(seg_type) {
  ਹਾਲਤ LOCAL_VM_SEG:
    ਜੇ(seg_ਸੂਚਕਾਂਕ == MEM_GRANT) {
	vir_ਬਾਈਟਾਂ newoffset;
	endpoint_t newep;
	ਪੂਰਨ new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) ਵਿੱਥ;

        ਜੇ(verify_grant(targetpr->p_endpoint, grantee, grant, ਗਿਣਤੀ_2,
                0, 0, &newoffset, &newep, ਨਿਰਾ) != OK) {
                ਲਿਖੋ("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, ਵਿੱਥ, ਗਿਣਤੀ_2, caller->p_name);
		proc_stacktrace(caller);
                ਵਾਪਸ EFAULT;
        }

        ਜੇ(!isokendpt(newep, &new_proc_nr)) {
                ਲਿਖੋ("SYSTEM: do_umap: isokendpt failed\n");
                ਵਾਪਸ EFAULT;
        }

	/* New lookup. */
	ਵਿੱਥ = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_ਸੂਚਕਾਂਕ = VIR_ADDR;
      }

      ਜੇ(seg_ਸੂਚਕਾਂਕ == VIR_ADDR) {
        phys_addr = lin_addr = ਵਿੱਥ;
      } ਨਹੀਂਤਾਂ {
	ਲਿਖੋ("SYSTEM: bogus seg type 0x%lx\n", seg_ਸੂਚਕਾਂਕ);
	ਵਾਪਸ EFAULT;
      }
      ਜੇ(!lin_addr) {
	ਲਿਖੋ("SYSTEM:do_umap: umap_local failed\n");
	ਵਾਪਸ EFAULT;
      }
      ਜੇ(vm_lookup(targetpr, lin_addr, &phys_addr, ਨਿਰਾ) != OK) {
	ਲਿਖੋ("SYSTEM:do_umap: vm_lookup failed\n");
	ਵਾਪਸ EFAULT;
      }
      ਜੇ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      ਰੋਕੋ;
  ਮੂਲ:
  	ਲਿਖੋ("umap: peculiar type\n");
  	ਵਾਪਸ EINVAL;
  }

  ਜੇ(vm_running && vm_lookup_range(targetpr, lin_addr, ਨਿਰਾ, ਗਿਣਤੀ_2) != ਗਿਣਤੀ_2) {
	ਲਿਖੋ("SYSTEM:do_umap: not contiguous\n");
	ਵਾਪਸ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  ਜੇ(phys_addr == 0) {
	  ਲਿਖੋ("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, ਵਿੱਥ, phys_addr);
	ਲਿਖੋ("caller stack: ");
	proc_stacktrace(caller);
  }
  ਵਾਪਸ (phys_addr == 0) ? EFAULT: OK;
}

#ਜੇ_ਅੰਤ /* USE_UMAP || USE_UMAP_REMOTE */
