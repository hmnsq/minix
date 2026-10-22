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
صحيح do_copy(بنية proc * caller, پیغام * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  بنية vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_بایټونه بایټونه;		/* number of bytes to copy */
  صحيح i;

#إذا 0
  إذا (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	ساكن صحيح لومړی=1;
	إذا (لومړی)
	{
		لومړی= 0;
		چاپ_کړه(
"do_copy: got request from %d (source %d, destination %d)\n",
			caller->p_endpoint,
			m_ptr->m_lsys_krn_sys_copy.src_endpt,
			m_ptr->m_lsys_krn_sys_copy.dst_endpt);
	}
  }
#د_که_پای

  /* Dismember the command message. */
  vir_addr[_SRC_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.src_endpt;
  vir_addr[_DST_].proc_nr_e = m_ptr->m_lsys_krn_sys_copy.dst_endpt;

  vir_addr[_SRC_].دننه_واټن = m_ptr->m_lsys_krn_sys_copy.src_addr;
  vir_addr[_DST_].دننه_واټن = m_ptr->m_lsys_krn_sys_copy.dst_addr;
  بایټونه = m_ptr->m_lsys_krn_sys_copy.nr_بایټونه;

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
	  چاپ_کړه("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          أرجع(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  إذا (بایټونه != (phys_بایټونه) (vir_بایټونه) بایټونه) أرجع(E2BIG);

  /* Now try to make the actual virtual copy. */
  إذا(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	صحيح r;
	أكد(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], بایټونه);
	إذا(r == EFAULT_SRC || r == EFAULT_DST) أرجع r = EFAULT;
	أرجع r;
  } وإلا {
	أرجع( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], بایټونه) );
  }
}
#د_که_پای /* (USE_VIRCOPY || USE_PHYSCOPY) */
