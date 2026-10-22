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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"

#ଅନ୍ତର୍ଭାଵ <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vumap(ସଂରଚନା proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  ସଂରଚନା proc *procp;
  ସଂରଚନା vumap_vir vvec[MAPVEC_NR];
  ସଂରଚନା vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r, proc_nr, vcount, pcount, pmax, access;
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

  ଯଦି (vcount <= 0 || pmax <= 0)
	ଫେରନ୍ତୁ EINVAL;

  ଯଦି (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ଯଦି (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  ବାଛ (access) {
  ସ୍ଥିତି VUA_READ:		access = CPF_READ; ଭାଙ୍ଗ;
  ସ୍ଥିତି VUA_WRITE:		access = CPF_WRITE; ଭାଙ୍ଗ;
  ସ୍ଥିତି VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:			ଫେରନ୍ତୁ EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * ଆକାର(vvec[0]);

  ଯଦି (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	ଫେରନ୍ତୁ EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  ପାଇଁ (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	ଯଦି (size <= offset)
		ଫେରନ୍ତୁ EINVAL;
	size -= offset;

	ଯଦି (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, ଖାଲି);
		ଯଦି (r != OK)
			ଫେରନ୍ତୁ r;
	} ଅନ୍ୟଥା {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	ଯେତେବେଳେ (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		ଯଦି (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ଯଦି (access & CPF_READ)
				ଫେରନ୍ତୁ EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ଫେରନ୍ତୁ vm_check_range(caller, procp, vir_addr, size, 1);
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
  ପ୍ରତିଜ୍ଞା(pcount > 0);

  size = pcount * ଆକାର(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  ଯଦି (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ଫେରନ୍ତୁ r;
}
