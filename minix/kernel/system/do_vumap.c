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

#અન્તર્ભાવ "kernel/system.h"

#અન્તર્ભાવ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
પૂર્ણાંક do_vumap(સંરચના proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  સંરચના proc *procp;
  સંરચના vumap_vir vvec[MAPVEC_NR];
  સંરચના vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  પૂર્ણાંક i, r, proc_nr, vcount, pcount, pmax, access;
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

  જો (vcount <= 0 || pmax <= 0)
	પાછા EINVAL;

  જો (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  જો (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  પસંદગી (access) {
  સ્થિતિ VUA_READ:		access = CPF_READ; તોડો;
  સ્થિતિ VUA_WRITE:		access = CPF_WRITE; તોડો;
  સ્થિતિ VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; તોડો;
  મૂળભૂત:			પાછા EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * કદ(vvec[0]);

  જો (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	પાછા EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  માટે (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	જો (size <= offset)
		પાછા EINVAL;
	size -= offset;

	જો (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, ખાલી);
		જો (r != OK)
			પાછા r;
	} નહીંતર {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	જ્યારે (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		જો (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			જો (access & CPF_READ)
				પાછા EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			પાછા vm_check_range(caller, procp, vir_addr, size, 1);
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
  પ્રતિજ્ઞા(pcount > 0);

  size = pcount * કદ(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  જો (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  પાછા r;
}
