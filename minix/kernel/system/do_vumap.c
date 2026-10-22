/* The kernel call implemented in this file:
 *   m_type:	SYS_VUMAP
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_vumap.endpt		(grant owner, or SELF for local addresses)
 *   m_lsys_krn_sys_vumap.vaddr		(address of virtual (input) vector)
 *   m_lsys_krn_sys_vumap.vcount	(number of elements in virtual vector)
 *   m_lsys_krn_sys_vumap.offset	(offset into first entry of input vector)
 *   m_lsys_krn_sys_vumap.access	(safecopy access requested for input)
 *   m_lsys_krn_sys_vumap.paddr		(address of physical (output) vector)
 *   m_lsys_krn_sys_vumap.pmax		(maximum number of physical vector elements)
 *   m_krn_lsys_sys_vumap.pcount	(upon return: number of elements filled)
 */

#ضمن "kernel/system.h"

#ضمن <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
صحيح do_vumap(بنية proc *caller, سنیہا *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, سروت, granter;
  بنية proc *procp;
  بنية vumap_vir vvec[MAPVEC_NR];
  بنية vumap_phys pvec[MAPVEC_NR];
  vir_بائٹاں vaddr, paddr, vir_addr;
  phys_بائٹاں phys_addr;
  صحيح i, r, proc_nr, vcount, pcount, pmax, access;
  size_t ناپ, chunk, وتھ;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  سروت = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  وتھ = m_ptr->m_lsys_krn_sys_vumap.وتھ;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  إذا (vcount <= 0 || pmax <= 0)
	أرجع EINVAL;

  إذا (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  إذا (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  بدل (access) {
  حالة VUA_READ:		access = CPF_READ; اكسر;
  حالة VUA_WRITE:		access = CPF_WRITE; اكسر;
  حالة VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; اكسر;
  افتراضي:			أرجع EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  ناپ = vcount * حجم(vvec[0]);

  إذا (انکڑے_copy(endpt, vaddr, KERNEL, (vir_بائٹاں) vvec, ناپ) != OK)
	أرجع EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  لكل (i = 0; i < vcount && pcount < pmax; i++) {
	ناپ = vvec[i].vv_ناپ;
	إذا (ناپ <= وتھ)
		أرجع EINVAL;
	ناپ -= وتھ;

	إذا (سروت != SELF) {
		r = verify_grant(سروت, endpt, vvec[i].vv_grant, ناپ, access,
			وتھ, &vir_addr, &granter, عدم);
		إذا (r != OK)
			أرجع r;
	} وإلا {
		vir_addr = vvec[i].vv_addr + وتھ;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	طالما (ناپ > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, ناپ);

		إذا (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			إذا (access & CPF_READ)
				أرجع EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			أرجع vm_check_range(caller, procp, vir_addr, ناپ, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_ناپ = chunk;
		pcount++;

		vir_addr += chunk;
		ناپ -= chunk;
	}

	وتھ = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  أكد(pcount > 0);

  ناپ = pcount * حجم(pvec[0]);

  r = انکڑے_copy_vmcheck(caller, KERNEL, (vir_بائٹاں) pvec, endpt, paddr, ناپ);

  إذا (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  أرجع r;
}
