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

#அந்தர்பாவ "kernel/system.h"

#அந்தர்பாவ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
முழு do_vumap(கட்டமைப்பு proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  கட்டமைப்பு proc *procp;
  கட்டமைப்பு vumap_vir vvec[MAPVEC_NR];
  கட்டமைப்பு vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  முழு i, r, proc_nr, vcount, pcount, pmax, access;
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

  எனில் (vcount <= 0 || pmax <= 0)
	திரும்பு EINVAL;

  எனில் (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  எனில் (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  தேர்வு (access) {
  நிலை VUA_READ:		access = CPF_READ; நிறுத்து;
  நிலை VUA_WRITE:		access = CPF_WRITE; நிறுத்து;
  நிலை VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; நிறுத்து;
  இயல்பு:			திரும்பு EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * அளவு(vvec[0]);

  எனில் (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	திரும்பு EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  க்கு (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	எனில் (size <= offset)
		திரும்பு EINVAL;
	size -= offset;

	எனில் (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, வெற்று);
		எனில் (r != OK)
			திரும்பு r;
	} இல்லையெனில் {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	போது (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		எனில் (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			எனில் (access & CPF_READ)
				திரும்பு EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			திரும்பு vm_check_range(caller, procp, vir_addr, size, 1);
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
  ப்ரதிஜ்ஞா(pcount > 0);

  size = pcount * அளவு(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  எனில் (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  திரும்பு r;
}
