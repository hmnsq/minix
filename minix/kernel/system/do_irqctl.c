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

#समावेश "kernel/system.h"

#समावेश <minix/endpoint.h>

#अगर USE_IRQCTL

अटल पूर्णांक generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
पूर्णांक do_irqctl(काष्ठा proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  पूर्णांक irq_vec;
  पूर्णांक irq_hook_id;
  पूर्णांक notify_id;
  पूर्णांक r = OK;
  पूर्णांक i;
  irq_hook_t *hook_ptr;
  काष्ठा priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  चयन(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  हाल IRQ_ENABLE:           
  हाल IRQ_DISABLE: 
      अगर (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) वापस(EINVAL);
      अगर (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) वापस(EPERM);
      अगर (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      अन्यथा 
          disable_irq(&irq_hooks[irq_hook_id]);	
      अवरोध;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  हाल IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      अगर (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) वापस(EINVAL);

      privp= priv(caller);
      अगर (!privp)
      {
	म_लिखो("do_irqctl: no priv structure!\n");
	वापस EPERM;
      }
      अगर (privp->s_flags & CHECK_IRQ)
      {
	क्रम (i= 0; i<privp->s_nr_irq; i++)
	{
		अगर (irq_vec == privp->s_irq_tab[i])
			अवरोध;
	}
	अगर (i >= privp->s_nr_irq)
	{
		म_लिखो(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		वापस EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      अगर (notify_id > अक्षर_बिट * माप(irq_id_t) - 1) वापस(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = शून्य;
      क्रम (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          अगर (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      क्रम (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          अगर (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      अगर (hook_ptr == शून्य) वापस(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      अवरोध;

  हाल IRQ_RMPOLICY:
      अगर (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           वापस(EINVAL);
      } अन्यथा अगर (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           वापस(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      अवरोध;

  शेष:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  वापस(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
अटल पूर्णांक generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  पूर्णांक proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  अगर(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  वापस(hook->policy & IRQ_REENABLE);
}

#पूर्ण_अगर /* USE_IRQCTL */
