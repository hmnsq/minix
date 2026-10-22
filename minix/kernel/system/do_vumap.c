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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
ਪੂਰਨ do_vumap(ਬਣਤਰ proc *caller, ਸੁਨੇਹਾ *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, ਸਰੋਤ, granter;
  ਬਣਤਰ proc *procp;
  ਬਣਤਰ vumap_vir vvec[MAPVEC_NR];
  ਬਣਤਰ vumap_phys pvec[MAPVEC_NR];
  vir_ਬਾਈਟਾਂ vaddr, paddr, vir_addr;
  phys_ਬਾਈਟਾਂ phys_addr;
  ਪੂਰਨ i, r, proc_nr, vcount, pcount, pmax, access;
  size_t ਮਾਪ, chunk, ਵਿੱਥ;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  ਸਰੋਤ = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  ਵਿੱਥ = m_ptr->m_lsys_krn_sys_vumap.ਵਿੱਥ;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  ਜੇ (vcount <= 0 || pmax <= 0)
	ਵਾਪਸ EINVAL;

  ਜੇ (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ਜੇ (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  ਚੋਣ (access) {
  ਹਾਲਤ VUA_READ:		access = CPF_READ; ਰੋਕੋ;
  ਹਾਲਤ VUA_WRITE:		access = CPF_WRITE; ਰੋਕੋ;
  ਹਾਲਤ VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; ਰੋਕੋ;
  ਮੂਲ:			ਵਾਪਸ EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  ਮਾਪ = vcount * ਆਕਾਰ(vvec[0]);

  ਜੇ (ਅੰਕੜੇ_copy(endpt, vaddr, KERNEL, (vir_ਬਾਈਟਾਂ) vvec, ਮਾਪ) != OK)
	ਵਾਪਸ EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  ਲਈ (i = 0; i < vcount && pcount < pmax; i++) {
	ਮਾਪ = vvec[i].vv_ਮਾਪ;
	ਜੇ (ਮਾਪ <= ਵਿੱਥ)
		ਵਾਪਸ EINVAL;
	ਮਾਪ -= ਵਿੱਥ;

	ਜੇ (ਸਰੋਤ != SELF) {
		r = verify_grant(ਸਰੋਤ, endpt, vvec[i].vv_grant, ਮਾਪ, access,
			ਵਿੱਥ, &vir_addr, &granter, ਨਿਰਾ);
		ਜੇ (r != OK)
			ਵਾਪਸ r;
	} ਨਹੀਂਤਾਂ {
		vir_addr = vvec[i].vv_addr + ਵਿੱਥ;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	ਜਦੋਂ (ਮਾਪ > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, ਮਾਪ);

		ਜੇ (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ਜੇ (access & CPF_READ)
				ਵਾਪਸ EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ਵਾਪਸ vm_check_range(caller, procp, vir_addr, ਮਾਪ, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_ਮਾਪ = chunk;
		pcount++;

		vir_addr += chunk;
		ਮਾਪ -= chunk;
	}

	ਵਿੱਥ = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  ਪ੍ਰਤਿਜ੍ਞਾ(pcount > 0);

  ਮਾਪ = pcount * ਆਕਾਰ(pvec[0]);

  r = ਅੰਕੜੇ_copy_vmcheck(caller, KERNEL, (vir_ਬਾਈਟਾਂ) pvec, endpt, paddr, ਮਾਪ);

  ਜੇ (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ਵਾਪਸ r;
}
