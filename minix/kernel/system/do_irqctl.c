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

#ถ้า USE_IRQCTL

static จำนวนเต็ม generic_handler(irq_hook_t *hook);

/*===========================================================================*
 *				do_irqctl				     *
 *===========================================================================*/
จำนวนเต็ม do_irqctl(sa_nracanaa proc * caller, message * m_ptr)
{
  /* Dismember the request message. */
  จำนวนเต็ม irq_vec;
  จำนวนเต็ม irq_hook_id;
  จำนวนเต็ม notify_id;
  จำนวนเต็ม r = OK;
  จำนวนเต็ม i;
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
      ถ้า (irq_hook_id >= NR_IRQ_HOOKS || irq_hook_id < 0 ||
          irq_hooks[irq_hook_id].proc_nr_e == NONE) คืน(EINVAL);
      ถ้า (irq_hooks[irq_hook_id].proc_nr_e != caller->p_endpoint) คืน(EPERM);
      ถ้า (m_ptr->m_lsys_krn_sys_irqctl.request == IRQ_ENABLE) {
          enable_irq(&irq_hooks[irq_hook_id]);	
      }
      มิฉะนั้น 
          disable_irq(&irq_hooks[irq_hook_id]);	
      bha_nga;

  /* Control IRQ policies. Set a policy and needed details in the IRQ table.
   * This policy is used by a generic function to handle hardware interrupts. 
   */
  pak_sa IRQ_SETPOLICY:  

      /* Check if IRQ line is acceptable. */
      ถ้า (irq_vec < 0 || irq_vec >= NR_IRQ_VECTORS) คืน(EINVAL);

      privp= priv(caller);
      ถ้า (!privp)
      {
	printf("do_irqctl: no priv structure!\n");
	คืน EPERM;
      }
      ถ้า (privp->s_flags & CHECK_IRQ)
      {
	สำหรับ (i= 0; i<privp->s_nr_irq; i++)
	{
		ถ้า (irq_vec == privp->s_irq_tab[i])
			bha_nga;
	}
	ถ้า (i >= privp->s_nr_irq)
	{
		printf(
		"do_irqctl: IRQ check failed for proc %d, IRQ %d\n",
			caller->p_endpoint, irq_vec);
		คืน EPERM;
	}
    }

      /* When setting a policy, the caller must provide an identifier that
       * is returned on the notification message if a interrupt occurs.
       */
      notify_id = m_ptr->m_lsys_krn_sys_irqctl.hook_id;
      ถ้า (notify_id > CHAR_BIT * parimaa_na(irq_id_t) - 1) คืน(EINVAL);

      /* Try to find an existing mapping to override. */
      hook_ptr = NULL;
      สำหรับ (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ถ้า (irq_hooks[i].proc_nr_e == caller->p_endpoint
              && irq_hooks[i].notify_id == notify_id) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* existing hook */
              rm_irq_handler(&irq_hooks[irq_hook_id]);
          }
      }

      /* If there is nothing to override, find a free hook for this mapping. */
      สำหรับ (i=0; !hook_ptr && i<NR_IRQ_HOOKS; i++) {
          ถ้า (irq_hooks[i].proc_nr_e == NONE) {
              irq_hook_id = i;
              hook_ptr = &irq_hooks[irq_hook_id];	/* free hook */
          }
      }
      ถ้า (hook_ptr == NULL) คืน(ENOSPC);

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
      ถ้า (irq_hook_id < 0 || irq_hook_id >= NR_IRQ_HOOKS ||
               irq_hooks[irq_hook_id].proc_nr_e == NONE) {
           คืน(EINVAL);
      } มิฉะนั้น ถ้า (caller->p_endpoint != irq_hooks[irq_hook_id].proc_nr_e) {
           คืน(EPERM);
      }
      /* Remove the handler and return. */
      rm_irq_handler(&irq_hooks[irq_hook_id]);
      irq_hooks[irq_hook_id].proc_nr_e = NONE;
      bha_nga;

  default:
      r = EINVAL;				/* invalid IRQ REQUEST */
  }
  คืน(r);
}

/*===========================================================================*
 *			       generic_handler				     *
 *===========================================================================*/
static จำนวนเต็ม generic_handler(irq_hook_t * hook)
{
/* This function handles hardware interrupt in a simple and generic way. All
 * interrupts are transformed into messages to a driver. The IRQ line will be
 * reenabled if the policy says so.
 */
  จำนวนเต็ม proc_nr;

  /* As a side-effect, the interrupt handler gathers random information by 
   * timestamping the interrupt events. This is used for /dev/random.
   */
  get_randomness(&krandom, hook->irq);

  /* Check if the handler is still alive.
   * If it's dead, this should never happen, as processes that die 
   * automatically get their interrupt hooks unhooked.
   */
  ถ้า(!isokendpt(hook->proc_nr_e, &proc_nr))
     panic("invalid interrupt handler: %d", hook->proc_nr_e);

  /* Add a bit for this interrupt to the process' pending interrupts. When 
   * sending the notification message, this bit map will be magically set
   * as an argument. 
   */
  priv(proc_addr(proc_nr))->s_int_pending |= (1 << hook->notify_id);

  /* Build notification message and return. */
  mini_notify(proc_addr(HARDWARE), hook->proc_nr_e);
  คืน(hook->policy & IRQ_REENABLE);
}

#endif /* USE_IRQCTL */
