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
صحيح do_vumap(بنية proc *caller, پیام *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, منبع, granter;
  بنية proc *procp;
  بنية vumap_vir vvec[MAPVEC_NR];
  بنية vumap_phys pvec[MAPVEC_NR];
  vir_بایتها vaddr, paddr, vir_addr;
  phys_بایتها phys_addr;
  صحيح i, r, proc_nr, vcount, pcount, pmax, access;
  size_t اندازه, chunk, جابه_جایی;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  منبع = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  جابه_جایی = m_ptr->m_lsys_krn_sys_vumap.جابه_جایی;
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
  اندازه = vcount * حجم(vvec[0]);

  إذا (داده_copy(endpt, vaddr, KERNEL, (vir_بایتها) vvec, اندازه) != OK)
	أرجع EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  لكل (i = 0; i < vcount && pcount < pmax; i++) {
	اندازه = vvec[i].vv_اندازه;
	إذا (اندازه <= جابه_جایی)
		أرجع EINVAL;
	اندازه -= جابه_جایی;

	إذا (منبع != SELF) {
		r = verify_grant(منبع, endpt, vvec[i].vv_grant, اندازه, access,
			جابه_جایی, &vir_addr, &granter, عدم);
		إذا (r != OK)
			أرجع r;
	} وإلا {
		vir_addr = vvec[i].vv_addr + جابه_جایی;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	طالما (اندازه > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, اندازه);

		إذا (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			إذا (access & CPF_READ)
				أرجع EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			أرجع vm_check_range(caller, procp, vir_addr, اندازه, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_اندازه = chunk;
		pcount++;

		vir_addr += chunk;
		اندازه -= chunk;
	}

	جابه_جایی = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  أكد(pcount > 0);

  اندازه = pcount * حجم(pvec[0]);

  r = داده_copy_vmcheck(caller, KERNEL, (vir_بایتها) pvec, endpt, paddr, اندازه);

  إذا (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  أرجع r;
}
