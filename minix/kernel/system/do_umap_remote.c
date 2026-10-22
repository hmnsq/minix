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
#تعریف_رد do_umap_remote
#جے_انت

/*==========================================================================*
 *				do_umap_remote				    *
 *==========================================================================*/
صحيح do_umap_remote(بنية proc * caller, سنیہا * m_ptr)
{
/* Map virtual address to physical, for non-kernel processes. */
  صحيح seg_type = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_TYPE;
  صحيح seg_اشاریہ = m_ptr->m_lsys_krn_sys_umap.segment & SEGMENT_اشاریہ;
  vir_بائٹاں وتھ = m_ptr->m_lsys_krn_sys_umap.src_addr;
  صحيح گنتی = m_ptr->m_lsys_krn_sys_umap.nr_بائٹاں;
  endpoint_t endpt = m_ptr->m_lsys_krn_sys_umap.src_endpt;
  endpoint_t grantee = m_ptr->m_lsys_krn_sys_umap.dst_endpt;
  صحيح proc_nr, proc_nr_grantee;
  phys_بائٹاں phys_addr = 0, lin_addr = 0;
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
	seg_اشاریہ != MEM_GRANT ||
	!isokendpt(grantee, &proc_nr_grantee)) {
	أرجع EINVAL;
  }

  /* See which mapping should be made. */
  بدل(seg_type) {
  حالة LOCAL_VM_SEG:
    إذا(seg_اشاریہ == MEM_GRANT) {
	vir_بائٹاں newoffset;
	endpoint_t newep;
	صحيح new_proc_nr;
	cp_grant_id_t grant = (cp_grant_id_t) وتھ;

        إذا(verify_grant(targetpr->p_endpoint, grantee, grant, گنتی,
                0, 0, &newoffset, &newep, عدم) != OK) {
                لکھو("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, وتھ, گنتی, caller->p_name);
		proc_stacktrace(caller);
                أرجع EFAULT;
        }

        إذا(!isokendpt(newep, &new_proc_nr)) {
                لکھو("SYSTEM: do_umap: isokendpt failed\n");
                أرجع EFAULT;
        }

	/* New lookup. */
	وتھ = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_اشاریہ = VIR_ADDR;
      }

      إذا(seg_اشاریہ == VIR_ADDR) {
        phys_addr = lin_addr = وتھ;
      } وإلا {
	لکھو("SYSTEM: bogus seg type 0x%lx\n", seg_اشاریہ);
	أرجع EFAULT;
      }
      إذا(!lin_addr) {
	لکھو("SYSTEM:do_umap: umap_local failed\n");
	أرجع EFAULT;
      }
      إذا(vm_lookup(targetpr, lin_addr, &phys_addr, عدم) != OK) {
	لکھو("SYSTEM:do_umap: vm_lookup failed\n");
	أرجع EFAULT;
      }
      إذا(phys_addr == 0)
	panic("vm_lookup returned zero physical address");
      اكسر;
  افتراضي:
  	لکھو("umap: peculiar type\n");
  	أرجع EINVAL;
  }

  إذا(vm_running && vm_lookup_range(targetpr, lin_addr, عدم, گنتی) != گنتی) {
	لکھو("SYSTEM:do_umap: not contiguous\n");
	أرجع EFAULT;
  }

  m_ptr->m_krn_lsys_sys_umap.dst_addr = phys_addr;
  إذا(phys_addr == 0) {
	  لکھو("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, caller->p_endpoint, caller->p_name,
		caller->p_reg.pc, وتھ, phys_addr);
	لکھو("caller stack: ");
	proc_stacktrace(caller);
  }
  أرجع (phys_addr == 0) ? EFAULT: OK;
}

#جے_انت /* USE_UMAP || USE_UMAP_REMOTE */
