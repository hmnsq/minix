/*
 *   The Minix hardware interrupt system.
 *   
 *   This file contains routines for managing the interrupt
 *   controller.
 *  
 *   put_irq_handler: register an interrupt handler.
 *   rm_irq_handler:  deregister an interrupt handler.
 *   irq_handle:     handle a hardware interrupt.
 *                    called by the system dependent part when an
 *                    external interrupt occurs.
 *   enable_irq:      enable hook for IRQ.
 *   disable_irq:     disable hook for IRQ.
 */

#شامل <assert.h>

#شامل "kernel/kernel.h"
#شامل "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
ایستا irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
تهی put_irq_handler( irq_hook_t* hook, صحیح irq,
  ثابت irq_handler_t handler)
{
  صحیح id;
  irq_hook_t **سطر;
  unsigned long bitmap;

  اگر( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  سطر = &irq_handlers[irq];

  bitmap = 0;
  وقتی ( *سطر != پوچ ) {
	اگر(hook == *سطر) بازگشت; /* extra initialization */
	bitmap |= (*سطر)->id;	/* mark ids in use */
	سطر = &(*سطر)->next;
  }

  /* find the lowest id not in use */
  برای (id = 1; id != 0; id <<= 1)
  	اگر (!(bitmap & id)) بشکن;

  اگر(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = پوچ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *سطر = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  اگر((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
تهی rm_irq_handler( ثابت irq_hook_t* hook ) {
  ثابت صحیح irq = hook->irq; 
  ثابت صحیح id = hook->id;
  irq_hook_t **سطر;

  اگر( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  سطر = &irq_handlers[irq];
  وقتی( (*سطر) != پوچ ) {
	اگر((*سطر)->id == id) {
		(*سطر) = (*سطر)->next;
		اگر (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	وگرنه {
		سطر = &(*سطر)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  اگر (irq_handlers[irq] == پوچ) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  وگرنه اگر (irq_actids[irq] == 0) {
	hw_intr_unmask(irq);
  }
}

/*===========================================================================*
 *				irq_handle				     *
 *===========================================================================*/
/*
 * The function first disables interrupt is need be and restores the state at
 * the end. Before returning, it unmasks the IRQ if and only if all active ID
 * bits are cleared, and restart a process.
 */
تهی irq_handle(صحیح irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  أكد(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  اگر(hook == پوچ) {
      ایستا صحیح nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      اگر(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	اگر(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      بازگشت;
  }

  /* Call list of handlers for an IRQ. */
  وقتی( hook != پوچ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    اگر( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  اگر (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
تهی enable_irq(ثابت irq_hook_t *hook)
{
  اگر((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
صحیح disable_irq(ثابت irq_hook_t *hook)
{
  اگر(irq_actids[hook->irq] & hook->id)  /* already disabled */
    بازگشت 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  بازگشت TRUE;
}

