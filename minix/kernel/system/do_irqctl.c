/* The kernel call implemented in this file:
 *   m_type:	SYS_IRQCTL
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_irqctl.request	(control operation to perform)
 *    m_lsys_krn_sys_irqctl.vector	(irq line that must be controlled)
 *    m_lsys_krn_sys_irqctl.policy	(irq policy allows reenabling interrupts)
 *    m_lsys_krn_sys_irqctl.hook_id	(provides index to be returned on interrupt)
 *    m_krn_lsys_sys_irqctl.hook_id	(returns index of irq hook assigned at kernel)
 */

#অন্তর্ভাৱ "kernel/system.h"

#অন্তর্ভাৱ <minix/endpoint.h>

#যদি USE_IRQCTL

স্থির পূর্ণাঙ্ক generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_irqctl(সংরচনা proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  পূর্ণাঙ্ক irq_vec;
  পূর্ণাঙ্ক irq_hook_id;
  পূর্ণাঙ্ক notify_id;
  পূর্ণাঙ্ক r = OK;
  পূর্ণাঙ্ক i;
  irq_hook_t *hook_ptr;
  সংরচনা priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  ৱিকল্প(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  পক্ষ IRQ_ENABLE:           
  পক্ষ IRQ_DISABLE: 
      যদি (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) ঘূৰাই(EINVAL);
      যদি (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) ঘূৰাই(EPERM);
      যদি (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      অন্যথা 
          disable_irq(&irq_hooks[irq_hook_id]);	
      ভঙ্গ;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  পক্ষ IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      যদি (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) ঘূৰাই(EINVAL);

      privp= priv(caller);
      যদি (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	ঘূৰাই EPERM;
      }
      যদি (privp->s_flags & CHECK_IRQ)
      {
	বাবে (i= 0; i<privp->s_nr_irq; i++)
	{
		যদি (irq_vec == privp->s_irq_tab[i])
			ভঙ্গ;
	}
	যদি (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		ঘূৰাই EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      যদি (notify_id > CHAR_BIT * পরিমাণ(irq_id_t) - 1) ঘূৰাই(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = রিক্ত;
      বাবে (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          যদি (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      বাবে (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          যদি (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      যদি (hook_ptr == রিক্ত) ঘূৰাই(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      ভঙ্গ;

  পক্ষ IRQ_RMPOLICY:
      যদি (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           ঘূৰাই(EINVAL);
      } অন্যথা যদি (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           ঘূৰাই(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      ভঙ্গ;

  স্ৱভাৱ:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  ঘূৰাই(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
স্থির পূর্ণাঙ্ক generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  পূর্ণাঙ্ক proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  যদি(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  ঘূৰাই(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
