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

#만약 USE_IRQCTL

static 정수 generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
정수 do_irqctl(struct proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  정수 irq_vec;
  정수 irq_hook_id;
  정수 notify_id;
  정수 r = OK;
  정수 i;
  irq_hook_t *hook_ptr;
  struct priv *privp;

  /* Hook identifiers start at 1 and end at NR_IRQ_HOOKS. */
  irq_hook_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id - 1;
  irq_vec = m_ptr->m_lsys_krn_sys_irqctl.vector;

  /* See what is requested and take needed actions. */
  switch(m_ptr->m_lsys_krn_sys_irqctl.request) {

  /* Enable or disable IRQs. This is straightforward. */
  case IRQ_ENABLE:           
  case IRQ_DISABLE: 
      만약 (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) 반환(EINVAL);
      만약 (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) 반환(EPERM);
      만약 (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      아니면 
          disable_irq(&irq_hooks[irq_hook_id]);	
      break;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  case IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      만약 (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) 반환(EINVAL);

      privp= priv(caller);
      만약 (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	반환 EPERM;
      }
      만약 (privp->s_flags & CHECK_IRQ)
      {
	위해 (i= 0; i<privp->s_nr_irq; i++)
	{
		만약 (irq_vec == privp->s_irq_tab[i])
			break;
	}
	만약 (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		반환 EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      만약 (notify_id > CHAR_BIT * sizeof(irq_id_t) - 1) 반환(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = NULL;
      위해 (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          만약 (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      위해 (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          만약 (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      만약 (hook_ptr == NULL) 반환(ENOSPC);

      /* Install the handler. */
      hook_ptr->proc_nr_e = caller->p_endpoint;	/* process to notify */
      hook_ptr->notify_id = notify_id;		/* identifier to pass */   	
      hook_ptr->policy = m_ptr->m_lsys_krn_sys_irqctl.policy;	/* policy for interrupts */
      put_irq_handler(hook_ptr, irq_vec, generic_handler);
      DEBUGBASIC(("IRQ %d handler registered by %s / %d\n",
			      irq_vec, caller->p_name, caller->p_endpoint));

      /* Return index of the IRQ hook in use. */
      m_ptr->m_krn_lsys_sys_irqctl.hook_id = irq_hook_id + 1;
      break;

  case IRQ_RMPOLICY:
      만약 (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           반환(EINVAL);
      } 아니면 만약 (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           반환(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      break;

  default:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  반환(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
static 정수 generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  정수 proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  만약(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  반환(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
