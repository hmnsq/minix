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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ "kernel/vm.h"
#అన్తర్భావ <assert.h>

#ఒకవేళ (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
పూర్ణాంకం do_copy(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  నిర్మాణం vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_బైట్లు బైట్లు;		/* number of bytes to copy */
  పూర్ణాంకం i;

#ఒకవేళ 0
  ఒకవేళ (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	స్థిర పూర్ణాంకం మొదటి=1;
	ఒకవేళ (మొదటి)
	{
		మొదటి= 0;
		రాయి(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#అయితే_ముగింపు

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].స్థానభ్రంశం = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].స్థానభ్రంశం = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  బైట్లు = m_ptr->m_lsys_krn_sys_copy.nr_బైట్లు;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  కొరకు (i=_SRC_; i<=_DST_; i++) {
	పూర్ణాంకం p;
      /* Check if process number was given implicitly with SELF and is valid. */
      ఒకవేళ (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      ఒకవేళ (vir_addr[i].proc_nr_e != NONE) {
	ఒకవేళ(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  రాయి("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          తిరిగి(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  ఒకవేళ (బైట్లు != (phys_బైట్లు) (vir_బైట్లు) బైట్లు) తిరిగి(E2BIG);

  /* Now try to make the actual virtual copy. */
  ఒకవేళ(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	పూర్ణాంకం r;
	ప్రతిజ్ఞా(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], బైట్లు);
	ఒకవేళ(r == EFAULT_SRC || r == EFAULT_DST) తిరిగి r = EFAULT;
	తిరిగి r;
  } లేకపోతే {
	తిరిగి( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], బైట్లు) );
  }
}
#అయితే_ముగింపు /* (USE_VIRCOPY || USE_PHYSCOPY) */
