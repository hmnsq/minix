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

#include "kernel/system.h"

#include <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
ចំនួនគត់ do_vumap(sa_nracanaa proc *caller, message *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, source, granter;
  sa_nracanaa proc *procp;
  sa_nracanaa vumap_vir vvec[MAPVEC_NR];
  sa_nracanaa vumap_phys pvec[MAPVEC_NR];
  vir_bytes vaddr, paddr, vir_addr;
  phys_bytes phys_addr;
  ចំនួនគត់ i, r, proc_nr, vcount, pcount, pmax, access;
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

  ប្រសិនបើ (vcount <= 0 || pmax <= 0)
	ត្រឡប់ EINVAL;

  ប្រសិនបើ (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  ប្រសិនបើ (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  switch (access) {
  pak_sa VUA_READ:		access = CPF_READ; bha_nga;
  pak_sa VUA_WRITE:		access = CPF_WRITE; bha_nga;
  pak_sa VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; bha_nga;
  default:			ត្រឡប់ EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  size = vcount * parimaa_na(vvec[0]);

  ប្រសិនបើ (data_copy(endpt, vaddr, KERNEL, (vir_bytes) vvec, size) != OK)
	ត្រឡប់ EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  សម្រាប់ (i = 0; i < vcount && pcount < pmax; i++) {
	size = vvec[i].vv_size;
	ប្រសិនបើ (size <= offset)
		ត្រឡប់ EINVAL;
	size -= offset;

	ប្រសិនបើ (source != SELF) {
		r = verify_grant(source, endpt, vvec[i].vv_grant, size, access,
			offset, &vir_addr, &granter, NULL);
		ប្រសិនបើ (r != OK)
			ត្រឡប់ r;
	} បើមិនដូច្នេះ {
		vir_addr = vvec[i].vv_addr + offset;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	ខណៈពេល (size > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, size);

		ប្រសិនបើ (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			ប្រសិនបើ (access & CPF_READ)
				ត្រឡប់ EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			ត្រឡប់ vm_check_range(caller, procp, vir_addr, size, 1);
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
  assert(pcount > 0);

  size = pcount * parimaa_na(pvec[0]);

  r = data_copy_vmcheck(caller, KERNEL, (vir_bytes) pvec, endpt, paddr, size);

  ប្រសិនបើ (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  ត្រឡប់ r;
}
