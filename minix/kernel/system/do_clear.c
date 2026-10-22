/* The kernel call implemented in this file:
 *   m_type:	SYS_CLEAR
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_clear.endpt		(endpoint of process to clean up)
 */

#అన్తర్భావ "kernel/system.h"

#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_CLEAR

/*===========================================================================*
 *				do_clear				     *
 *===========================================================================*/
పూర్ణాంకం do_clear(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Handle sys_clear. Only the PM can request other process slots to be cleared
 * when a process has exited.
 * The routine to clean up a process table slot cancels outstanding timers, 
 * possibly removes the process from the message queues, and resets certain 
 * process table fields to the default values.
 */
  నిర్మాణం proc *rc;
  పూర్ణాంకం exit_p;
  పూర్ణాంకం i;

  ఒకవేళ(!isokendpt(m_ptr->m_lsys_krn_sys_clear.endpt, &exit_p)) {
      /* get exiting process */
      తిరిగి EINVAL;
  }
  rc = proc_addr(exit_p);	/* clean up */

  release_చిరునామా_space(rc);

  /* Don't clear if already cleared. */
  ఒకవేళ(isemptyp(rc)) తిరిగి OK;

  /* Check the table with IRQ hooks to see if hooks should be released. */
  కొరకు (i=0; i < NR_IRQ_HOOKS; i++) {
      ఒకవేళ (rc->p_endpoint == irq_hooks[i].proc_nr_e) {
        rm_irq_handler(&irq_hooks[i]);	/* remove interrupt handler */
        irq_hooks[i].proc_nr_e = NONE;	/* mark hook as free */
      } 
  }

  /* Remove the process' ability to send and receive messages */
  clear_endpoint(rc);

  /* Turn off any alarm timers at the clock. */   
  reset_kernel_timer(&priv(rc)->s_alarm_timer);

  /* Make sure that the exiting process is no longer scheduled,
   * and mark slot as FREE. Also mark saved fpu contents as not significant.
   */
  RTS_SETFLAGS(rc, RTS_స్థానం_FREE);
  
  /* release FPU */
  release_fpu(rc);
  rc->p_misc_flags &= ~MF_FPU_INITIALIZED;

  /* Release the process table slot. If this is a system process, also
   * release its privilege structure.  Further cleanup is not needed at
   * this point. All important fields are reinitialized when the 
   * slots are assigned to another, new process. 
   */
  ఒకవేళ (priv(rc)->s_flags & SYS_PROC) priv(rc)->s_proc_nr = NONE;

#ఒకవేళ 0
  /* Clean up virtual memory */
  ఒకవేళ (rc->p_misc_flags & MF_VM) {
  	vm_map_default(rc);
  }
#అయితే_ముగింపు

  తిరిగి OK;
}

#అయితే_ముగింపు /* USE_CLEAR */
