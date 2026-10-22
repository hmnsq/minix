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
صحيح do_vumap(بنية proc *caller, پيغام *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, ذريعو, granter;
  بنية proc *procp;
  بنية vumap_vir vvec[MAPVEC_NR];
  بنية vumap_phys pvec[MAPVEC_NR];
  vir_بائيٽون vaddr, paddr, vir_addr;
  phys_بائيٽون phys_addr;
  صحيح i, r, proc_nr, vcount, pcount, pmax, access;
  size_t سائيز, chunk, وٿي;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  ذريعو = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  وٿي = m_ptr->m_lsys_krn_sys_vumap.وٿي;
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
  سائيز = vcount * حجم(vvec[0]);

  إذا (معلومات_copy(endpt, vaddr, KERNEL, (vir_بائيٽون) vvec, سائيز) != OK)
	أرجع EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  لكل (i = 0; i < vcount && pcount < pmax; i++) {
	سائيز = vvec[i].vv_سائيز;
	إذا (سائيز <= وٿي)
		أرجع EINVAL;
	سائيز -= وٿي;

	إذا (ذريعو != SELF) {
		r = verify_grant(ذريعو, endpt, vvec[i].vv_grant, سائيز, access,
			وٿي, &vir_addr, &granter, عدم);
		إذا (r != OK)
			أرجع r;
	} وإلا {
		vir_addr = vvec[i].vv_addr + وٿي;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	طالما (سائيز > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, سائيز);

		إذا (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			إذا (access & CPF_READ)
				أرجع EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			أرجع vm_check_range(caller, procp, vir_addr, سائيز, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_سائيز = chunk;
		pcount++;

		vir_addr += chunk;
		سائيز -= chunk;
	}

	وٿي = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  أكد(pcount > 0);

  سائيز = pcount * حجم(pvec[0]);

  r = معلومات_copy_vmcheck(caller, KERNEL, (vir_بائيٽون) pvec, endpt, paddr, سائيز);

  إذا (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  أرجع r;
}
