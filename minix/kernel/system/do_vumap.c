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

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
పూర్ణాంకం do_vumap(నిర్మాణం proc *caller, సందేశం *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, ఆధారం, granter;
  నిర్మాణం proc *procp;
  నిర్మాణం vumap_vir vvec[MAPVEC_NR];
  నిర్మాణం vumap_phys pvec[MAPVEC_NR];
  vir_బైట్లు vaddr, paddr, vir_addr;
  phys_బైట్లు phys_addr;
  పూర్ణాంకం i, r, proc_nr, vcount, pcount, pmax, access;
  size_t కొలత, chunk, స్థానభ్రంశం;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  ఆధారం = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  స్థానభ్రంశం = m_ptr->m_lsys_krn_sys_vumap.స్థానభ్రంశం;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  ఒకవేళ (vcount <= 0 || pmax <= 0)
	తిరిగి EINVAL;

  ఒకవేళ (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ఒకవేళ (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  ఎంపిక (access) {
  స్థితి VUA_READ:		access = CPF_READ; ఆపు;
  స్థితి VUA_WRITE:		access = CPF_WRITE; ఆపు;
  స్థితి VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; ఆపు;
  అప్రమేయ:			తిరిగి EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  కొలత = vcount * పరిమాణం(vvec[0]);

  ఒకవేళ (దత్తాంశం_copy(endpt, vaddr, KERNEL, (vir_బైట్లు) vvec, కొలత) != OK)
	తిరిగి EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  కొరకు (i = 0; i < vcount && pcount < pmax; i++) {
	కొలత = vvec[i].vv_కొలత;
	ఒకవేళ (కొలత <= స్థానభ్రంశం)
		తిరిగి EINVAL;
	కొలత -= స్థానభ్రంశం;

	ఒకవేళ (ఆధారం != SELF) {
		r = verify_grant(ఆధారం, endpt, vvec[i].vv_grant, కొలత, access,
			స్థానభ్రంశం, &vir_addr, &granter, ఖాళీ);
		ఒకవేళ (r != OK)
			తిరిగి r;
	} లేకపోతే {
		vir_addr = vvec[i].vv_addr + స్థానభ్రంశం;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	అయితే (కొలత > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, కొలత);

		ఒకవేళ (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ఒకవేళ (access & CPF_READ)
				తిరిగి EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			తిరిగి vm_check_range(caller, procp, vir_addr, కొలత, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_కొలత = chunk;
		pcount++;

		vir_addr += chunk;
		కొలత -= chunk;
	}

	స్థానభ్రంశం = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  ప్రతిజ్ఞా(pcount > 0);

  కొలత = pcount * పరిమాణం(pvec[0]);

  r = దత్తాంశం_copy_vmcheck(caller, KERNEL, (vir_బైట్లు) pvec, endpt, paddr, కొలత);

  ఒకవేళ (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  తిరిగి r;
}
