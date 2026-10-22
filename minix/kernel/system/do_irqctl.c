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

#অন্তর্ভাব "kernel/system.h"

#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_IRQCTL

স্থির পূর্ণসংখ্যা generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_irqctl(কাঠামো proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  পূর্ণসংখ্যা irq_vec;
  পূর্ণসংখ্যা irq_hook_id;
  পূর্ণসংখ্যা notify_id;
  পূর্ণসংখ্যা r = OK;
  পূর্ণসংখ্যা i;
  irq_hook_t *hook_ptr;
  কাঠামো priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  নির্বাচন(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  ক্ষেত্র IRQ_ENABLE:           
  ক্ষেত্র IRQ_DISABLE: 
      যদি (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) ফেরত(EINVAL);
      যদি (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) ফেরত(EPERM);
      যদি (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      অন্যথা 
          disable_irq(&irq_hooks[irq_hook_id]);	
      বিরতি;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  ক্ষেত্র IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      যদি (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) ফেরত(EINVAL);

      privp= priv(caller);
      যদি (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	ফেরত EPERM;
      }
      যদি (privp->s_flags & CHECK_IRQ)
      {
	জন্য (i= 0; i<privp->s_nr_irq; i++)
	{
		যদি (irq_vec == privp->s_irq_tab[i])
			বিরতি;
	}
	যদি (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		ফেরত EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      যদি (notify_id > CHAR_BIT * আকার(irq_id_t) - 1) ফেরত(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = ফাঁকা;
      জন্য (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          যদি (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      জন্য (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          যদি (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      যদি (hook_ptr == ফাঁকা) ফেরত(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      বিরতি;

  ক্ষেত্র IRQ_RMPOLICY:
      যদি (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           ফেরত(EINVAL);
      } অন্যথা যদি (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           ফেরত(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      বিরতি;

  পূর্বনির্ধারিত:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  ফেরত(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
স্থির পূর্ণসংখ্যা generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  পূর্ণসংখ্যা proc_nr;

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
  ফেরত(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
