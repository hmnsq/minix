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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_vumap(ರಚನೆ proc *caller, ಸಂದೇಶ *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, ಮೂಲ, granter;
  ರಚನೆ proc *procp;
  ರಚನೆ vumap_vir vvec[MAPVEC_NR];
  ರಚನೆ vumap_phys pvec[MAPVEC_NR];
  vir_ಬೈಟುಗಳು vaddr, paddr, vir_addr;
  phys_ಬೈಟುಗಳು phys_addr;
  ಪೂರ್ಣಾಂಕ i, r, proc_nr, vcount, pcount, pmax, access;
  size_t ಗಾತ್ರ_2, chunk, ಸ್ಥಾನಾಂತರ;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  ಮೂಲ = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_sys_vumap.ಸ್ಥಾನಾಂತರ;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  ಆದರೆ (vcount <= 0 || pmax <= 0)
	ಹಿಂತಿರುಗಿ EINVAL;

  ಆದರೆ (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ಆದರೆ (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  ಆಯ್ಕೆ (access) {
  ಸ್ಥಿತಿ VUA_READ:		access = CPF_READ; ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VUA_WRITE:		access = CPF_WRITE; ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:			ಹಿಂತಿರುಗಿ EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  ಗಾತ್ರ_2 = vcount * ಗಾತ್ರ(vvec[0]);

  ಆದರೆ (ದತ್ತಾಂಶ_copy(endpt, vaddr, KERNEL, (vir_ಬೈಟುಗಳು) vvec, ಗಾತ್ರ_2) != OK)
	ಹಿಂತಿರುಗಿ EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  ಗಾಗಿ (i = 0; i < vcount && pcount < pmax; i++) {
	ಗಾತ್ರ_2 = vvec[i].vv_ಗಾತ್ರ;
	ಆದರೆ (ಗಾತ್ರ_2 <= ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ EINVAL;
	ಗಾತ್ರ_2 -= ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (ಮೂಲ != SELF) {
		r = verify_grant(ಮೂಲ, endpt, vvec[i].vv_grant, ಗಾತ್ರ_2, access,
			ಸ್ಥಾನಾಂತರ, &vir_addr, &granter, ಖಾಲಿ);
		ಆದರೆ (r != OK)
			ಹಿಂತಿರುಗಿ r;
	} ಇಲ್ಲದಿದ್ದರೆ {
		vir_addr = vvec[i].vv_addr + ಸ್ಥಾನಾಂತರ;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	ಆಗ (ಗಾತ್ರ_2 > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, ಗಾತ್ರ_2);

		ಆದರೆ (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ಆದರೆ (access & CPF_READ)
				ಹಿಂತಿರುಗಿ EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ಹಿಂತಿರುಗಿ vm_check_range(caller, procp, vir_addr, ಗಾತ್ರ_2, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_ಗಾತ್ರ = chunk;
		pcount++;

		vir_addr += chunk;
		ಗಾತ್ರ_2 -= chunk;
	}

	ಸ್ಥಾನಾಂತರ = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  ಪ್ರತಿಜ್ಞಾ(pcount > 0);

  ಗಾತ್ರ_2 = pcount * ಗಾತ್ರ(pvec[0]);

  r = ದತ್ತಾಂಶ_copy_vmcheck(caller, KERNEL, (vir_ಬೈಟುಗಳು) pvec, endpt, paddr, ಗಾತ್ರ_2);

  ಆದರೆ (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ಹಿಂತಿರುಗಿ r;
}
