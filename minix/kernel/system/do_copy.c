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

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ "kernel/vm.h"
#অন্তর্ভাৱ <assert.h>

#যদি (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
পূর্ণাঙ্ক do_copy(সংরচনা proc * caller, message * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  সংরচনা vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_bytes bytes;		/* number of bytes to copy */
  পূর্ণাঙ্ক i;

#যদি 0
  যদি (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	স্থির পূর্ণাঙ্ক first=1;
	যদি (first)
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
  বাবে (i=_SRC_; i<=_DST_; i++) {
	পূর্ণাঙ্ক p;
      /* Check if process number was given implicitly with SELF and is valid. */
      যদি (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      যদি (vir_addr[i].proc_nr_e != NONE) {
	যদি(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  printf("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          ঘূৰাই(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  যদি (bytes != (phys_bytes) (vir_bytes) bytes) ঘূৰাই(E2BIG);

  /* Now try to make the actual virtual copy. */
  যদি(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	পূর্ণাঙ্ক r;
	প্রতিজ্ঞা(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], bytes);
	যদি(r == EFAULT_SRC || r == EFAULT_DST) ঘূৰাই r = EFAULT;
	ঘূৰাই r;
  } অন্যথা {
	ঘূৰাই( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], bytes) );
  }
}
#endif /* (USE_VIRCOPY || USE_PHYSCOPY) */
