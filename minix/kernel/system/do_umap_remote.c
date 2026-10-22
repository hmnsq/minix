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

#شامل "kernel/system.h"

#شامل <minix/endpoint.h>

#اگر USE_UMAP || USE_UMAP_REMOTE

#اگر ! USE_UMAP_REMOTE
#undef do_umap_remote
#endif

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
صحیح do_umap_remote(ساختار proc * caller, message * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  صحیح seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  صحیح seg_index = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->m_lsys_krn_sys_umap.src_addr;
  صحیح شمار = m_ptr->m_lsys_krn_sys_umap.nr_bytes;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  صحیح proc_nr, proc_nr_grantee;
  phys_bytes phys_addr = 0, lin_addr = 0;
  ساختار proc *targetpr;

  /* Verify process number. */
  اگر (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  وگرنه
	اگر (! isokendpt(endpt, &proc_nr))
		بازگشت(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  اگر (grantee == SELF) {
	grantee = caller->p_endpoint;
  } وگرنه اگر (grantee == NONE ||
	grantee == ANY ||
	seg_index != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	بازگشت EINVAL;
  }

  /* See which mapping should be made. */
  انتخاب(seg_type) {
  حالت LOCAL_VM_SEG:
    اگر(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	صحیح new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) offset;

        اگر(verify_grant(targetpr->p_endpoint, grantee, grant, شمار,
                0, 0, &newoffset, &newep, پوچ) != OK) {
                printf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, شمار, caller->p_name);
		proc_stacktrace(caller);
                بازگشت EFAULT;
        }

        اگر(!isokendpt(newep, &new_proc_nr)) {
                printf("SYSTEM: do_umap: isokendpt failed\n");
                بازگشت EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = VIR_ADDR;
      }

      اگر(seg_index == VIR_ADDR) {
        phys_addr = lin_addr = offset;
      } وگرنه {
	printf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	بازگشت EFAULT;
      }
      اگر(!lin_addr) {
	printf("SYSTEM:do_umap: umap_local failed\n");
	بازگشت EFAULT;
      }
      اگر(vm_lookup(targetpr, lin_addr, &phys_addr, پوچ) != OK) {
	printf("SYSTEM:do_umap: vm_lookup failed\n");
	بازگشت EFAULT;
      }
      اگر(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      بشکن;
  پیشفرض:
  	printf("umap: peculiar type\n");
  	بازگشت EINVAL;
  }

  اگر(vm_running && vm_lookup_range(targetpr, lin_addr, پوچ, شمار) != شمار) {
	printf("SYSTEM:do_umap: not contiguous\n");
	بازگشت EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  اگر(phys_addr == 0) {
	  printf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, offset, phys_addr);
	printf("caller stack: ");
	proc_stacktrace(caller);
  }
  بازگشت (phys_addr == 0) ? EFAULT: OK;
}

#endif /* USE_UMAP || USE_UMAP_REMOTE */
