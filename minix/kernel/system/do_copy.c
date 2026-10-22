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

#include "kernel/system.h"
#include "kernel/vm.h"
#include <assert.h>

#အကယ်၍ (USE_VIRCOPY || USE_PHYSCOPY)

/*===========================================================================*
 *				do_copy					     *
 *===========================================================================*/
ကိန်းပြည့် do_copy(sa_nracanaa proc * caller, message * m_ptr)
{
/* Handle sys_vircopy() and sys_physcopy().  Copy data using virtual or
 * physical addressing. Although a single handler function is used, there 
 * are two different kernel calls so that permissions can be checked. 
 */
  sa_nracanaa vir_addr vir_addr[2];	/* virtual source and destination address */
  phys_bytes bytes;		/* number of bytes to copy */
  ကိန်းပြည့် i;

#အကယ်၍ 0
  အကယ်၍ (caller->p_endpoint != PM_PROC_NR && caller->p_endpoint != VFS_PROC_NR &&
	caller->p_endpoint != RS_PROC_NR && caller->p_endpoint != MEM_PROC_NR &&
	caller->p_endpoint != VM_PROC_NR)
  {
	static ကိန်းပြည့် first=1;
	အကယ်၍ (first)
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
  အတွက် (i=_SRC_; i<=_DST_; i++) {
	ကိန်းပြည့် p;
      /* Check if process number was given implicitly with SELF and is valid. */
      အကယ်၍ (vir_addr[i].proc_nr_e == SELF)
	vir_addr[i].proc_nr_e = caller->p_endpoint;
      အကယ်၍ (vir_addr[i].proc_nr_e != NONE) {
	အကယ်၍(! isokendpt(vir_addr[i].proc_nr_e, &p)) {
	  printf("do_copy: %d: %d not ok endpoint\n", i, vir_addr[i].proc_nr_e);
          ပြန်ပေး(EINVAL); 
        }
      }
  }

  /* Check for overflow. This would happen for 64K segments and 16-bit 
   * vir_bytes. Especially copying by the PM on do_fork() is affected. 
   */
  အကယ်၍ (bytes != (phys_bytes) (vir_bytes) bytes) ပြန်ပေး(E2BIG);

  /* Now try to make the actual virtual copy. */
  အကယ်၍(m_ptr->m_lsys_krn_sys_copy.flags & CP_FLAG_TRY) {
	ကိန်းပြည့် r;
	assert(caller->p_endpoint == VFS_PROC_NR);
	r = virtual_copy(&vir_addr[_SRC_], &vir_addr[_DST_], bytes);
	အကယ်၍(r == EFAULT_SRC || r == EFAULT_DST) ပြန်ပေး r = EFAULT;
	ပြန်ပေး r;
  } မဟုတ်ပါက {
	ပြန်ပေး( virtual_copy_vmcheck(caller, &vir_addr[_SRC_],
			  	&vir_addr[_DST_], bytes) );
  }
}
#endif /* (USE_VIRCOPY || USE_PHYSCOPY) */
