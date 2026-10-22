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

#അന്തര്ഭാവ "kernel/system.h"

#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_UMAP || USE_UMAP_REMOTE

#എങ്കിൽ ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
പൂർണ്ണസംഖ്യ do_umap_remote(ഘടന proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  പൂർണ്ണസംഖ്യ seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  പൂർണ്ണസംഖ്യ seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  പൂർണ്ണസംഖ്യ എണ്ണം = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  പൂർണ്ണസംഖ്യ proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  ഘടന proc *targetpr;

  /* Verify process number. */
  എങ്കിൽ (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  അല്ലെങ്കിൽ
	എങ്കിൽ (! isokendpt(endpt, &proc_nr))
		തിരികെ(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  എങ്കിൽ (grantee == SELF) {
	grantee = caller->p_endpoint;
  } അല്ലെങ്കിൽ എങ്കിൽ (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	തിരികെ EINVAL;
  }

  /* See which mapping should be made. */
  തിരഞ്ഞെടുപ്പ്(seg_type) {
  അവസ്ഥ LOCAL_VM_SEG:
    എങ്കിൽ(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	പൂർണ്ണസംഖ്യ new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        എങ്കിൽ(verify_grant(targetpr->p_endpoint, grantee, grant, എണ്ണം,
                0, 0, &newoffset, &newep, ശൂന്യ) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, എണ്ണം, caller->p_name);
		proc_stacktrace(caller);
                തിരികെ EFAULT;
        }

        എങ്കിൽ(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                തിരികെ EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      എങ്കിൽ(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } അല്ലെങ്കിൽ {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	തിരികെ EFAULT;
      }
      എങ്കിൽ(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	തിരികെ EFAULT;
      }
      എങ്കിൽ(vm_lookup(targetpr, lin_addr, &phys_addr, ശൂന്യ) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	തിരികെ EFAULT;
      }
      എങ്കിൽ(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
  	printf("umap: peculiar type\n");
  	തിരികെ EINVAL;
  }

  എങ്കിൽ(vm_running && vm_lookup_range(targetpr, lin_addr, ശൂന്യ, എണ്ണം) != എണ്ണം) {
	printf("SYSTEM:do_umap: not contiguous\n");
	തിരികെ EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  എങ്കിൽ(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  തിരികെ (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
