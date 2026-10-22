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

#ضمن "kernel/system.h"

#ضمن <minix/endpoint.h>

#إذا USE_UMAP || USE_UMAP_REMOTE

#إذا ! USE_UMAP_REMOTE
#لغو_تعریف do_umap_remote
#پایان_اگر

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
صحيح do_umap_remote(بنية proc * caller, پیام * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  صحيح seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  صحيح seg_شاخص = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_شاخص;
  vir_بایتها جابه_جایی = m_ptr->m_lsys_krn_sys_umap.src_addr;
  صحيح شمار = m_ptr->m_lsys_krn_sys_umap.nr_بایتها;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  صحيح proc_nr, proc_nr_grantee;
  phys_بایتها phys_addr = 0, lin_addr = 0;
  بنية proc *targetpr;

  /* Verify process number. */
  إذا (endpt == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  وإلا
	إذا (! isokendpt(endpt, &proc_nr))
		أرجع(EINVAL);
  targetpr = proc_addr(proc_nr);

  /* Verify grantee endpoint */
  إذا (grantee == SELF) {
	grantee = caller->p_endpoint;
  } وإلا إذا (grantee == NONE ||
	grantee == ANY ||
	seg_شاخص != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	أرجع EINVAL;
  }

  /* See which mapping should be made. */
  بدل(seg_type) {
  حالة LOCAL_VM_SEG:
    إذا(seg_شاخص == MEM_GRANT) {
	vir_بایتها newoffset;
	endpoint_t newep;
	صحيح new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) جابه_جایی;

        إذا(verify_grant(targetpr->p_endpoint, grantee, grant, شمار,
                0, 0, &newoffset, &newep, عدم) != OK) {
                چاپ_کن("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, جابه_جایی, شمار, caller->p_name);
		proc_stacktrace(caller);
                أرجع EFAULT;
        }

        إذا(!isokendpt(newep, &new_proc_nr)) {
                چاپ_کن("SYSTEM: do_umap: isokendpt failed\n");
                أرجع EFAULT;
        }

	/* New lookup. */
	جابه_جایی = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_شاخص = VIR_ADDR;
      }

      إذا(seg_شاخص == VIR_ADDR) {
        phys_addr = lin_addr = جابه_جایی;
      } وإلا {
	چاپ_کن("SYSTEM: bogus seg type 0x%lx\n", seg_شاخص);
	أرجع EFAULT;
      }
      إذا(!lin_addr) {
	چاپ_کن("SYSTEM:do_umap: umap_local failed\n");
	أرجع EFAULT;
      }
      إذا(vm_lookup(targetpr, lin_addr, &phys_addr, عدم) != OK) {
	چاپ_کن("SYSTEM:do_umap: vm_lookup failed\n");
	أرجع EFAULT;
      }
      إذا(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      اكسر;
  افتراضي:
  	چاپ_کن("umap: peculiar type\n");
  	أرجع EINVAL;
  }

  إذا(vm_running && vm_lookup_range(targetpr, lin_addr, عدم, شمار) != شمار) {
	چاپ_کن("SYSTEM:do_umap: not contiguous\n");
	أرجع EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  إذا(phys_addr == 0) {
	  چاپ_کن("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, جابه_جایی, phys_addr);
	چاپ_کن("caller stack: ");
	proc_stacktrace(caller);
  }
  أرجع (phys_addr == 0) ? EFAULT: OK;
}

#پایان_اگر /* USE_UMAP || USE_UMAP_REMOTE */
