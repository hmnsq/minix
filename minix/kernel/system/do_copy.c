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

#समावेश "kernel/system.h"
#समावेश "kernel/vm.h"
#समावेश <assert.h>

#अगर (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
पूर्णांक do_copy(काष्ठा proc * caller, message * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  काष्ठा vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_bytes bytes;		/* number of bytes to copy */
  पूर्णांक i;

#अगर 0
  अगर (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	अटल पूर्णांक first=1;
	अगर (first)
	{
		first= 0;
		म_लिखो(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#पूर्ण_अगर

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].offset = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].offset = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  bytes = m_ptr->m_lsys_krn_sys_copy.nr_bytes;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  क्रम (i=_SRC_; i<=_DST_; i++) {
	पूर्णांक p;
      /* Check if process number was given implicitly with SELF and is valid. */
      अगर (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      अगर (vir_addr[i].proc_nr_e != NONE) {
	अगर(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  म_लिखो("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          वापस(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  अगर (bytes != (phys_bytes) (vir_bytes) bytes) वापस(E2BIG);

  /* Now try to make the actual virtual copy. */
  अगर(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	पूर्णांक r;
	निश्चित(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], bytes);
	अगर(r == EFAULT_SRC || r == EFAULT_DST) वापस r = EFAULT;
	वापस r;
  } अन्यथा {
	वापस( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], bytes) );
  }
}
#पूर्ण_अगर /* (USE_VIRCOPY || USE_PHYSCOPY) */
