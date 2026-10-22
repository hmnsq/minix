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
#تعریف_منسوخ do_umap_remote
#اگر_ختم

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
صحیح do_umap_remote(ساخت proc * caller, پیغام * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  صحیح seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  صحیح seg_اشاریہ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_اشاریہ;
  vir_بائٹیں انحراف = m_ptr->m_lsys_krn_sys_umap.src_addr;
  صحیح گنتی = m_ptr->m_lsys_krn_sys_umap.nr_بائٹیں;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  صحیح proc_nr, proc_nr_grantee;
  phys_بائٹیں phys_addr = 0, lin_addr = 0;
  ساخت proc *targetpr;

  /* Verify process number. */
  اگر (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ورنہ
	اگر (! isokendpt(endpt, &proc_nr))
		واپس(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  اگر (grantee == SELF) {
	grantee = caller->p_endpoint;
  } ورنہ اگر (grantee == NONE ||
	grantee == ANY ||
	seg_اشاریہ != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	واپس EINVAL;
  }

  /* See which mapping should be made. */
  انتخاب(seg_type) {
  حالت LOCAL_VM_SEG:
    اگر(seg_اشاریہ == MEM_GRANT) {
	vir_بائٹیں newoffset;
	endpoint_t newep;
	صحیح new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) انحراف;

        اگر(verify_grant(targetpr->p_endpoint, grantee, grant, گنتی,
                0, 0, &newoffset, &newep, تہی) != OK) {
                لکھو("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, انحراف, گنتی, caller->p_name);
		proc_stacktrace(caller);
                واپس EFAULT;
        }

        اگر(!isokendpt(newep, &new_proc_nr)) {
                لکھو("SYSTEM: do_umap: isokendpt failed\n");
                واپس EFAULT;
        }

	/* New lookup. */
	انحراف = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_اشاریہ = VIR_ADDR;
      }

      اگر(seg_اشاریہ == VIR_ADDR) {
        phys_addr = lin_addr = انحراف;
      } ورنہ {
	لکھو("SYSTEM: bogus seg type 0x%lx\n", seg_اشاریہ);
	واپس EFAULT;
      }
      اگر(!lin_addr) {
	لکھو("SYSTEM:do_umap: umap_local failed\n");
	واپس EFAULT;
      }
      اگر(vm_lookup(targetpr, lin_addr, &phys_addr, تہی) != OK) {
	لکھو("SYSTEM:do_umap: vm_lookup failed\n");
	واپس EFAULT;
      }
      اگر(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      توڑو;
  طےشدہ:
  	لکھو("umap: peculiar type\n");
  	واپس EINVAL;
  }

  اگر(vm_running && vm_lookup_range(targetpr, lin_addr, تہی, گنتی) != گنتی) {
	لکھو("SYSTEM:do_umap: not contiguous\n");
	واپس EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  اگر(phys_addr == 0) {
	  لکھو("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, انحراف, phys_addr);
	لکھو("caller stack: ");
	proc_stacktrace(caller);
  }
  واپس (phys_addr == 0) ? EFAULT: OK;
}

#اگر_ختم /* USE_UMAP || USE_UMAP_REMOTE */
