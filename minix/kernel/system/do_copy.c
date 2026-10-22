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

#ضمن "kernel/system.h"
#ضمن "kernel/vm.h"
#ضمن <assert.h>

#إذا (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
صحيح do_copy(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  بنية vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_بائيٽون بائيٽون;		/* number of bytes to copy */
  صحيح i;

#إذا 0
  إذا (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	ساكن صحيح پهريون=1;
	إذا (پهريون)
	{
		پهريون= 0;
		لکو(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#جيڪڏهن_پورو

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].وٿي = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].وٿي = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  بائيٽون = m_ptr->m_lsys_krn_sys_copy.nr_بائيٽون;

  /* Now do some checks for both the source and destination virtual address.
   * This is done once for _SRC_, then once for _DST_. 
   */
  لكل (i=_SRC_; i<=_DST_; i++) {
	صحيح p;
      /* Check if process number was given implicitly with SELF and is valid. */
      إذا (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      إذا (vir_addr[i].proc_nr_e != NONE) {
	إذا(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  لکو("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          أرجع(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  إذا (بائيٽون != (phys_بائيٽون) (vir_بائيٽون) بائيٽون) أرجع(E2BIG);

  /* Now try to make the actual virtual copy. */
  إذا(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	صحيح r;
	أكد(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], بائيٽون);
	إذا(r == EFAULT_SRC || r == EFAULT_DST) أرجع r = EFAULT;
	أرجع r;
  } وإلا {
	أرجع( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], بائيٽون) );
  }
}
#جيڪڏهن_پورو /* (USE_VIRCOPY || USE_PHYSCOPY) */
