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

#ಯದಿ (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_copy(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  ಸಂರಚನಾ vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_bytes bytes;		/* number of bytes to copy */
  ಪೂರ್ಣಾಙ್ಕ i;

#ಯದಿ 0
  ಯದಿ (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first=1;
	ಯದಿ (first)
	{
		first= 0;
		printf(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#endif

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].offset = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].offset = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  bytes = m_ptr->m_lsys_krn_sys_copy.nr_bytes;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  ಪ್ರತಿ (i=_SRC_; i<=_DST_; i++) {
	ಪೂರ್ಣಾಙ್ಕ p;
      /* Check if process number was given implicitly with SELF and is valid. */
      ಯದಿ (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      ಯದಿ (vir_addr[i].proc_nr_e != NONE) {
	ಯದಿ(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  printf("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          ಪ್ರತ್ಯಾಗಮ(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  ಯದಿ (bytes != (phys_bytes) (vir_bytes) bytes) ಪ್ರತ್ಯಾಗಮ(E2BIG);

  /* Now try to make the actual virtual copy. */
  ಯದಿ(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	ಪೂರ್ಣಾಙ್ಕ r;
	ಪ್ರತಿಜ್ಞಾ(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], bytes);
	ಯದಿ(r == EFAULT_SRC || r == EFAULT_DST) ಪ್ರತ್ಯಾಗಮ r = EFAULT;
	ಪ್ರತ್ಯಾಗಮ r;
  } ಅನ್ಯಥಾ {
	ಪ್ರತ್ಯಾಗಮ( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], bytes) );
  }
}
#endif /* (USE_VIRCOPY || USE_PHYSCOPY) */
