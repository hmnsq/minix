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
ಪೂರ್ಣಾಙ್ಕ do_vumap(ಸಂರಚನಾ proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  ಸಂರಚನಾ proc *procp;
  ಸಂರಚನಾ vumap_vir vvec[MAPVEC_NR];
  ಸಂರಚನಾ vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  ಪೂರ್ಣಾಙ್ಕ i, r, proc_nr, vcount, pcount, pmax, access;
  size_t size, chunk, offset;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  source = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  offset = m_ptr->m_lsys_krn_sys_vumap.offset;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  ಯದಿ (vcount <= 0 || pmax <= 0)
	ಪ್ರತ್ಯಾಗಮ EINVAL;

  ಯದಿ (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ಯದಿ (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  ವಿಕಲ್ಪ (access) {
  ಪಕ್ಷ VUA_READ:		access = CPF_READ; ಭಙ್ಗ;
  ಪಕ್ಷ VUA_WRITE:		access = CPF_WRITE; ಭಙ್ಗ;
  ಪಕ್ಷ VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; ಭಙ್ಗ;
  ಸ್ವಭಾವ:			ಪ್ರತ್ಯಾಗಮ EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * ಪರಿಮಾಣ(vvec[0]);

  ಯದಿ (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	ಪ್ರತ್ಯಾಗಮ EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  ಪ್ರತಿ (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	ಯದಿ (size <= offset)
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	size -= offset;

	ಯದಿ (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, ರಿಕ್ತ);
		ಯದಿ (r != OK)
			ಪ್ರತ್ಯಾಗಮ r;
	} ಅನ್ಯಥಾ {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	ಯಾವತ್ (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		ಯದಿ (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ಯದಿ (access & CPF_READ)
				ಪ್ರತ್ಯಾಗಮ EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ಪ್ರತ್ಯಾಗಮ vm_check_range(caller, procp, vir_addr, size, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_size = chunk;
		pcount++;

		vir_addr += chunk;
		size -= chunk;
	}

	offset = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  ಪ್ರತಿಜ್ಞಾ(pcount > 0);

  size = pcount * ಪರಿಮಾಣ(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  ಯದಿ (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ಪ್ರತ್ಯಾಗಮ r;
}
