/* The kernel call implemented in this file:
 *   m_type:	SYS_VIRCOPY, SYS_PHYSCOPY
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_copy.src_addr		source offset within segment
 *   m_lsys_krn_sys_copy.src_endpt		source process number
 *   m_lsys_krn_sys_copy.dst_addr		destination offset within segment
 *   m_lsys_krn_sys_copy.dst_endpt		destination process number
 *   m_lsys_krn_sys_copy.nr_bytes		number of bytes to copy
 *   m_lsys_krn_sys_copy.flags
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਜੇ (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
ਪੂਰਨ do_copy(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  ਬਣਤਰ vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_ਬਾਈਟਾਂ ਬਾਈਟਾਂ;		/* number of bytes to copy */
  ਪੂਰਨ i;

#ਜੇ 0
  ਜੇ (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ=1;
	ਜੇ (ਪਹਿਲਾ)
	{
		ਪਹਿਲਾ= 0;
		ਲਿਖੋ(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#ਜੇ_ਅੰਤ

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].ਵਿੱਥ = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].ਵਿੱਥ = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  ਬਾਈਟਾਂ = m_ptr->m_lsys_krn_sys_copy.nr_ਬਾਈਟਾਂ;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  ਲਈ (i=_SRC_; i<=_DST_; i++) {
	ਪੂਰਨ p;
      /* Check if process number was given implicitly with SELF and is valid. */
      ਜੇ (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      ਜੇ (vir_addr[i].proc_nr_e != NONE) {
	ਜੇ(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  ਲਿਖੋ("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          ਵਾਪਸ(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  ਜੇ (ਬਾਈਟਾਂ != (phys_ਬਾਈਟਾਂ) (vir_ਬਾਈਟਾਂ) ਬਾਈਟਾਂ) ਵਾਪਸ(E2BIG);

  /* Now try to make the actual virtual copy. */
  ਜੇ(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	ਪੂਰਨ r;
	ਪ੍ਰਤਿਜ੍ਞਾ(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], ਬਾਈਟਾਂ);
	ਜੇ(r == EFAULT_SRC || r == EFAULT_DST) ਵਾਪਸ r = EFAULT;
	ਵਾਪਸ r;
  } ਨਹੀਂਤਾਂ {
	ਵਾਪਸ( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], ਬਾਈਟਾਂ) );
  }
}
#ਜੇ_ਅੰਤ /* (USE_VIRCOPY || USE_PHYSCOPY) */
