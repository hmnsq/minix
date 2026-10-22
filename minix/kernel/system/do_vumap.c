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

#অন্তর্ভাব "kernel/system.h"

#অন্তর্ভাব <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_vumap(কাঠামো proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  কাঠামো proc *procp;
  কাঠামো vumap_vir vvec[MAPVEC_NR];
  কাঠামো vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  পূর্ণসংখ্যা i, r, proc_nr, vcount, pcount, pmax, access;
  size_t আকার_2, chunk, offset;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  source = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  offset = m_ptr->m_lsys_krn_sys_vumap.offset;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  যদি (vcount <= 0 || pmax <= 0)
	ফেরত EINVAL;

  যদি (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  যদি (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  নির্বাচন (access) {
  ক্ষেত্র VUA_READ:		access = CPF_READ; বিরতি;
  ক্ষেত্র VUA_WRITE:		access = CPF_WRITE; বিরতি;
  ক্ষেত্র VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; বিরতি;
  পূর্বনির্ধারিত:			ফেরত EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  আকার_2 = vcount * আকার(vvec[0]);

  যদি (তথ্য_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, আকার_2) != OK)
	ফেরত EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  জন্য (i = 0; i < vcount && pcount < pmax; i++) {
	আকার_2 = vvec[i].vv_আকার;
	যদি (আকার_2 <= offset)
		ফেরত EINVAL;
	আকার_2 -= offset;

	যদি (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, আকার_2, access,
			offset, &vir_addr, &granter, ফাঁকা);
		যদি (r != OK)
			ফেরত r;
	} অন্যথা {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	যখন (আকার_2 > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, আকার_2);

		যদি (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			যদি (access & CPF_READ)
				ফেরত EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ফেরত vm_check_range(caller, procp, vir_addr, আকার_2, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_আকার = chunk;
		pcount++;

		vir_addr += chunk;
		আকার_2 -= chunk;
	}

	offset = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  প্রতিজ্ঞা(pcount > 0);

  আকার_2 = pcount * আকার(pvec[0]);

  r = তথ্য_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, আকার_2);

  যদি (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ফেরত r;
}
