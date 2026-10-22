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

#include "kernel/system.h"

#include <minix/endpoint.h>

#නම් USE_IRQCTL

static පූර්ණ_සංඛ්‍යා generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
පූර්ණ_සංඛ්‍යා do_irqctl(sa_nracanaa proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  පූර්ණ_සංඛ්‍යා irq_vec;
  පූර්ණ_සංඛ්‍යා irq_hook_id;
  පූර්ණ_සංඛ්‍යා notify_id;
  පූර්ණ_සංඛ්‍යා r = OK;
  පූර්ණ_සංඛ්‍යා i;
  irq_hook_t *hook_ptr;
  sa_nracanaa priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  switch(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  pak_sa IRQ_ENABLE:           
  pak_sa IRQ_DISABLE: 
      නම් (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) ආපසු(EINVAL);
      නම් (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) ආපසු(EPERM);
      නම් (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      නැතහොත් 
          disable_irq(&irq_hooks[irq_hook_id]);	
      bha_nga;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  pak_sa IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      නම් (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) ආපසු(EINVAL);

      privp= priv(caller);
      නම් (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	ආපසු EPERM;
      }
      නම් (privp->s_flags & CHECK_IRQ)
      {
	සඳහා (i= 0; i<privp->s_nr_irq; i++)
	{
		නම් (irq_vec == privp->s_irq_tab[i])
			bha_nga;
	}
	නම් (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		ආපසු EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      නම් (notify_id > CHAR_BIT * parimaa_na(irq_id_t) - 1) ආපසු(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = NULL;
      සඳහා (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          නම් (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      සඳහා (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          නම් (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      නම් (hook_ptr == NULL) ආපසු(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      bha_nga;

  pak_sa IRQ_RMPOLICY:
      නම් (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           ආපසු(EINVAL);
      } නැතහොත් නම් (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           ආපසු(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      bha_nga;

  default:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  ආපසු(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
static පූර්ණ_සංඛ්‍යා generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  පූර්ණ_සංඛ්‍යා proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  නම්(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  ආපසු(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
