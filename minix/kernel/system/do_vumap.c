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

#شامل "kernel/system.h"

#شامل <assert.h>

/*===========================================================================*
 *				do_vumap				     *
 *===========================================================================*/
صحیح do_vumap(ساخت proc *caller, پیغام *m_ptr)
{
/* Map a vector of grants or local virtual addresses to physical addresses.
 * Designed to be used by drivers to perform an efficient lookup of physical
 * addresses for the purpose of direct DMA from/to a remote process.
 */
  endpoint_t endpt, ماخذ, granter;
  ساخت proc *procp;
  ساخت vumap_vir vvec[MAPVEC_NR];
  ساخت vumap_phys pvec[MAPVEC_NR];
  vir_بائٹیں vaddr, paddr, vir_addr;
  phys_بائٹیں phys_addr;
  صحیح i, r, proc_nr, vcount, pcount, pmax, access;
  size_t سائز, chunk, انحراف;

  endpt = caller->p_endpoint;

  /* Retrieve and check input parameters. */
  ماخذ = m_ptr->m_lsys_krn_sys_vumap.endpt;
  vaddr = m_ptr->m_lsys_krn_sys_vumap.vaddr;
  vcount = m_ptr->m_lsys_krn_sys_vumap.vcount;
  انحراف = m_ptr->m_lsys_krn_sys_vumap.انحراف;
  access = m_ptr->m_lsys_krn_sys_vumap.access;
  paddr = m_ptr->m_lsys_krn_sys_vumap.paddr;
  pmax = m_ptr->m_lsys_krn_sys_vumap.pmax;

  اگر (vcount <= 0 || pmax <= 0)
	واپس EINVAL;

  اگر (vcount > MAPVEC_NR) vcount = MAPVEC_NR;
  اگر (pmax > MAPVEC_NR) pmax = MAPVEC_NR;

  /* Convert access to safecopy access flags. */
  انتخاب (access) {
  حالت VUA_READ:		access = CPF_READ; توڑو;
  حالت VUA_WRITE:		access = CPF_WRITE; توڑو;
  حالت VUA_READ|VUA_WRITE:	access = CPF_READ|CPF_WRITE; توڑو;
  طےشدہ:			واپس EINVAL;
  }

  /* Copy in the vector of virtual addresses. */
  سائز = vcount * حجم(vvec[0]);

  اگر (ڈیٹا_copy(endpt, vaddr, KERNEL, (vir_بائٹیں) vvec, سائز) != OK)
	واپس EFAULT;

  pcount = 0;

  /* Go through the input entries, one at a time. Stop early in case the output
   * vector has filled up.
   */
  برائے (i = 0; i < vcount && pcount < pmax; i++) {
	سائز = vvec[i].vv_سائز;
	اگر (سائز <= انحراف)
		واپس EINVAL;
	سائز -= انحراف;

	اگر (ماخذ != SELF) {
		r = verify_grant(ماخذ, endpt, vvec[i].vv_grant, سائز, access,
			انحراف, &vir_addr, &granter, تہی);
		اگر (r != OK)
			واپس r;
	} ورنہ {
		vir_addr = vvec[i].vv_addr + انحراف;
		granter = endpt;
	}

	okendpt(granter, &proc_nr);
	procp = proc_addr(proc_nr);

	/* Each virtual range is made up of one or more physical ranges. */
	جب (سائز > 0 && pcount < pmax) {
		chunk = vm_lookup_range(procp, vir_addr, &phys_addr, سائز);

		اگر (!chunk) {
			/* Try to get the memory allocated, unless the memory
			 * is supposed to be there to be read from.
			 */
			اگر (access & CPF_READ)
				واپس EFAULT;

			/* This call may suspend the current call, or return an
			 * error for a previous invocation.
			 */
			واپس vm_check_range(caller, procp, vir_addr, سائز, 1);
		}

		pvec[pcount].vp_addr = phys_addr;
		pvec[pcount].vp_سائز = chunk;
		pcount++;

		vir_addr += chunk;
		سائز -= chunk;
	}

	انحراف = 0;
  }

  /* Copy out the resulting vector of physical addresses. */
  أكد(pcount > 0);

  سائز = pcount * حجم(pvec[0]);

  r = ڈیٹا_copy_vmcheck(caller, KERNEL, (vir_بائٹیں) pvec, endpt, paddr, سائز);

  اگر (r == OK)
	m_ptr->m_krn_lsys_sys_vumap.pcount = pcount;

  واپس r;
}
