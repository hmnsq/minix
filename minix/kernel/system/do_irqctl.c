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

#ᤀᤡ᤺ USE_IRQCTL

static puur_naa_nka generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
puur_naa_nka do_irqctl(sa_nracanaa proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  puur_naa_nka irq_vec;
  puur_naa_nka irq_hook_id;
  puur_naa_nka notify_id;
  puur_naa_nka r = OK;
  puur_naa_nka i;
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
      ᤀᤡ᤺ (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) ᤀᤡᤴ(EINVAL);
      ᤀᤡ᤺ (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) ᤀᤡᤴ(EPERM);
      ᤀᤡ᤺ (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      ᤀᤡ᤺ᤆᤡ 
          disable_irq(&irq_hooks[irq_hook_id]);	
      bha_nga;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  pak_sa IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      ᤀᤡ᤺ (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) ᤀᤡᤴ(EINVAL);

      privp= priv(caller);
      ᤀᤡ᤺ (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	ᤀᤡᤴ EPERM;
      }
      ᤀᤡ᤺ (privp->s_flags & CHECK_IRQ)
      {
	for (i= 0; i<privp->s_nr_irq; i++)
	{
		ᤀᤡ᤺ (irq_vec == privp->s_irq_tab[i])
			bha_nga;
	}
	ᤀᤡ᤺ (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		ᤀᤡᤴ EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      ᤀᤡ᤺ (notify_id > CHAR_BIT * parimaa_na(irq_id_t) - 1) ᤀᤡᤴ(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = NULL;
      for (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ᤀᤡ᤺ (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      for (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ᤀᤡ᤺ (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      ᤀᤡ᤺ (hook_ptr == NULL) ᤀᤡᤴ(ENOSPC);

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
      ᤀᤡ᤺ (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           ᤀᤡᤴ(EINVAL);
      } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           ᤀᤡᤴ(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      bha_nga;

  default:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  ᤀᤡᤴ(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
static puur_naa_nka generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  puur_naa_nka proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  ᤀᤡ᤺(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  ᤀᤡᤴ(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
