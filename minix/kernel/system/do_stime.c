/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_stime.boot_time
 */

#अन्तर्भाव "kernel/system.h"

#अन्तर्भाव <minix/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
पूर्णाङ्क do_stime(संरचना proc * caller, सन्देशः * m_ptr)
{
  समुच्चयः_boottime(m_ptr->m_lsys_krn_sys_stime.boot_time);
  प्रत्यागम(OK);
}
