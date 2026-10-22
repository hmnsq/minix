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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ <assert.h>

#ಆದರೆ (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_copy(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  ರಚನೆ vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_ಬೈಟುಗಳು ಬೈಟುಗಳು;		/* number of bytes to copy */
  ಪೂರ್ಣಾಂಕ i;

#ಆದರೆ 0
  ಆದರೆ (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ=1;
	ಆದರೆ (ಮೊದಲ)
	{
		ಮೊದಲ= 0;
		ಬರೆ(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].ಸ್ಥಾನಾಂತರ = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  ಬೈಟುಗಳು = m_ptr->m_lsys_krn_sys_copy.nr_ಬೈಟುಗಳು;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  ಗಾಗಿ (i=_SRC_; i<=_DST_; i++) {
	ಪೂರ್ಣಾಂಕ p;
      /* Check if process number was given implicitly with SELF and is valid. */
      ಆದರೆ (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      ಆದರೆ (vir_addr[i].proc_nr_e != NONE) {
	ಆದರೆ(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  ಬರೆ("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          ಹಿಂತಿರುಗಿ(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  ಆದರೆ (ಬೈಟುಗಳು != (phys_ಬೈಟುಗಳು) (vir_ಬೈಟುಗಳು) ಬೈಟುಗಳು) ಹಿಂತಿರುಗಿ(E2BIG);

  /* Now try to make the actual virtual copy. */
  ಆದರೆ(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	ಪೂರ್ಣಾಂಕ r;
	ಪ್ರತಿಜ್ಞಾ(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], ಬೈಟುಗಳು);
	ಆದರೆ(r == EFAULT_SRC || r == EFAULT_DST) ಹಿಂತಿರುಗಿ r = EFAULT;
	ಹಿಂತಿರುಗಿ r;
  } ಇಲ್ಲದಿದ್ದರೆ {
	ಹಿಂತಿರುಗಿ( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], ಬೈಟುಗಳು) );
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* (USE_VIRCOPY || USE_PHYSCOPY) */
