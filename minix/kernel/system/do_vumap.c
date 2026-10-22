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

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
पूर्णाङ्क do_vumap(संरचना proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  संरचना proc *procp;
  संरचना vumap_vir vvec[MAPVEC_NR];
  संरचना vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  पूर्णाङ्क i, r, proc_nr, vcount, pcount, pmax, access;
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

  यदि (vcount <= 0 || pmax <= 0)
	प्रत्यागम EINVAL;

  यदि (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  यदि (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  विकल्प (access) {
  पक्ष VUA_READ:		access = CPF_READ; भङ्ग;
  पक्ष VUA_WRITE:		access = CPF_WRITE; भङ्ग;
  पक्ष VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; भङ्ग;
  स्वभाव:			प्रत्यागम EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * परिमाण(vvec[0]);

  यदि (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	प्रत्यागम EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  प्रति (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	यदि (size <= offset)
		प्रत्यागम EINVAL;
	size -= offset;

	यदि (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, रिक्त);
		यदि (r != OK)
			प्रत्यागम r;
	} अन्यथा {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	यावत् (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		यदि (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			यदि (access & CPF_READ)
				प्रत्यागम EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			प्रत्यागम vm_check_range(caller, procp, vir_addr, size, 1);
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
  प्रतिज्ञा(pcount > 0);

  size = pcount * परिमाण(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  यदि (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  प्रत्यागम r;
}
