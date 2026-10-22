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

#અન્તર્ભાવ "kernel/system.h"

#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_UMAP || USE_UMAP_REMOTE

#જો ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
પૂર્ણાંક do_umap_remote(સંરચના proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  પૂર્ણાંક seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  પૂર્ણાંક seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  પૂર્ણાંક ગણતરી = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  પૂર્ણાંક proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  સંરચના proc *targetpr;

  /* Verify process number. */
  જો (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  નહીંતર
	જો (! isokendpt(endpt, &proc_nr))
		પાછા(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  જો (grantee == SELF) {
	grantee = caller->p_endpoint;
  } નહીંતર જો (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	પાછા EINVAL;
  }

  /* See which mapping should be made. */
  પસંદગી(seg_type) {
  સ્થિતિ LOCAL_VM_SEG:
    જો(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	પૂર્ણાંક new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        જો(verify_grant(targetpr->p_endpoint, grantee, grant, ગણતરી,
                0, 0, &newoffset, &newep, ખાલી) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, ગણતરી, caller->p_name);
		proc_stacktrace(caller);
                પાછા EFAULT;
        }

        જો(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                પાછા EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      જો(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } નહીંતર {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	પાછા EFAULT;
      }
      જો(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	પાછા EFAULT;
      }
      જો(vm_lookup(targetpr, lin_addr, &phys_addr, ખાલી) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	પાછા EFAULT;
      }
      જો(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      તોડો;
  મૂળભૂત:
  	printf("umap: peculiar type\n");
  	પાછા EINVAL;
  }

  જો(vm_running && vm_lookup_range(targetpr, lin_addr, ખાલી, ગણતરી) != ગણતરી) {
	printf("SYSTEM:do_umap: not contiguous\n");
	પાછા EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  જો(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  પાછા (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
