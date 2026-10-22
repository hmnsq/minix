/* The kernel call implemented in this file:
 *   m_type:	SYS_CLEAR
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_clear.endpt		(endpoint of process to clean up)
 */

#ضمن "kernel/system.h"

#ضمن <minix/endpoint.h>

#إذا USE_CLEAR

/*===========================================================================*
 *				do_clear				     *
 *===========================================================================*/
صحيح do_clear(بنية proc * caller, پيغام * m_ptr)
{
/* Handle sys_clear. Only the PM can request other process slots to be cleared
 * when a process has exited.
 * The routine to clean up a process table slot cancels outstanding timers, 
 * possibly removes the process from the message queues, and resets certain 
 * process table fields to the default values.
 */
  بنية proc *rc;
  صحيح exit_p;
  صحيح i;

  إذا(!isokendpt(m_ptr->m_lsys_krn_sys_clear.endpt, &exit_p)) {
      /* get exiting process */
      أرجع EINVAL;
  }
  rc = proc_addr(exit_p);	/* clean up */

  release_پتو_space(rc);

  /* Don't clear if already cleared. */
  إذا(isemptyp(rc)) أرجع OK;

  /* Check the table with IRQ hooks to see if hooks should be released. */
  لكل (i=0; i < NR_IRQ_HOOKS; i++) {
      إذا (rc->p_endpoint == irq_hooks[i].proc_nr_e) {
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
  RTS_SETFLAGS(rc, RTS_جڳهه_FREE);
  
  /* release FPU */
  release_fpu(rc);
  rc->p_misc_flags &= ~MF_FPU_INITIALIZED;

  /* Release the process table slot. If this is a system process, also
   * release its privilege structure.  Further cleanup is not needed at
   * this point. All important fields are reinitialized when the 
   * slots are assigned to another, new process. 
   */
  إذا (priv(rc)->s_flags & SYS_PROC) priv(rc)->s_proc_nr = NONE;

#إذا 0
  /* Clean up virtual memory */
  إذا (rc->p_misc_flags & MF_VM) {
  	vm_map_default(rc);
  }
#جيڪڏهن_پورو

  أرجع OK;
}

#جيڪڏهن_پورو /* USE_CLEAR */
