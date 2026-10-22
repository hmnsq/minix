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

#समावेश "kernel/system.h"

#समावेश <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
पूर्णांक do_vumap(काष्ठा proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  काष्ठा proc *procp;
  काष्ठा vumap_vir vvec[MAPVEC_NR];
  काष्ठा vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  पूर्णांक i, r, proc_nr, vcount, pcount, pmax, access;
  माप_प्रकार आकार, chunk, offset;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  source = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  offset = m_ptr->m_lsys_krn_sys_vumap.offset;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  अगर (vcount <= 0 || pmax <= 0)
	वापस EINVAL;

  अगर (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  अगर (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  चयन (access) {
  हाल VUA_READ:		access = CPF_READ; अवरोध;
  हाल VUA_WRITE:		access = CPF_WRITE; अवरोध;
  हाल VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; अवरोध;
  शेष:			वापस EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  आकार = vcount * माप(vvec[0]);

  अगर (तथ्य_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, आकार) != OK)
	वापस EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  क्रम (i = 0; i < vcount && pcount < pmax; i++) {
	आकार = vvec[i].vv_आकार;
	अगर (आकार <= offset)
		वापस EINVAL;
	आकार -= offset;

	अगर (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, आकार, access,
			offset, &vir_addr, &granter, शून्य);
		अगर (r != OK)
			वापस r;
	} अन्यथा {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	जबतक (आकार > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, आकार);

		अगर (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			अगर (access & CPF_READ)
				वापस EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			वापस vm_check_range(caller, procp, vir_addr, आकार, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_आकार = chunk;
		pcount++;

		vir_addr += chunk;
		आकार -= chunk;
	}

	offset = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  निश्चित(pcount > 0);

  आकार = pcount * माप(pvec[0]);

  r = तथ्य_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, आकार);

  अगर (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  वापस r;
}
