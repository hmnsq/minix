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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"

#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಯದಿ USE_IRQCTL

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_irqctl(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  ಪೂರ್ಣಾಙ್ಕ irq_vec;
  ಪೂರ್ಣಾಙ್ಕ irq_hook_id;
  ಪೂರ್ಣಾಙ್ಕ notify_id;
  ಪೂರ್ಣಾಙ್ಕ r = OK;
  ಪೂರ್ಣಾಙ್ಕ i;
  irq_hook_t *hook_ptr;
  ಸಂರಚನಾ priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  ವಿಕಲ್ಪ(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  ಪಕ್ಷ IRQ_ENABLE:           
  ಪಕ್ಷ IRQ_DISABLE: 
      ಯದಿ (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) ಪ್ರತ್ಯಾಗಮ(EINVAL);
      ಯದಿ (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) ಪ್ರತ್ಯಾಗಮ(EPERM);
      ಯದಿ (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      ಅನ್ಯಥಾ 
          disable_irq(&irq_hooks[irq_hook_id]);	
      ಭಙ್ಗ;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  ಪಕ್ಷ IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      ಯದಿ (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) ಪ್ರತ್ಯಾಗಮ(EINVAL);

      privp= priv(caller);
      ಯದಿ (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	ಪ್ರತ್ಯಾಗಮ EPERM;
      }
      ಯದಿ (privp->s_flags & CHECK_IRQ)
      {
	ಪ್ರತಿ (i= 0; i<privp->s_nr_irq; i++)
	{
		ಯದಿ (irq_vec == privp->s_irq_tab[i])
			ಭಙ್ಗ;
	}
	ಯದಿ (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		ಪ್ರತ್ಯಾಗಮ EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      ಯದಿ (notify_id > CHAR_BIT * ಪರಿಮಾಣ(irq_id_t) - 1) ಪ್ರತ್ಯಾಗಮ(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = ರಿಕ್ತ;
      ಪ್ರತಿ (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ಯದಿ (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      ಪ್ರತಿ (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ಯದಿ (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      ಯದಿ (hook_ptr == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      ಭಙ್ಗ;

  ಪಕ್ಷ IRQ_RMPOLICY:
      ಯದಿ (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           ಪ್ರತ್ಯಾಗಮ(EINVAL);
      } ಅನ್ಯಥಾ ಯದಿ (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           ಪ್ರತ್ಯಾಗಮ(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      ಭಙ್ಗ;

  ಸ್ವಭಾವ:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  ಪ್ರತ್ಯಾಗಮ(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  ಪೂರ್ಣಾಙ್ಕ proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  ಯದಿ(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  ಪ್ರತ್ಯಾಗಮ(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
