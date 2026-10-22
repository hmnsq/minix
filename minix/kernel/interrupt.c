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

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
ਸਥਿਰ irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
ਖਾਲੀ put_irq_handler( irq_hook_t* hook, ਪੂਰਨ irq,
  ਅਚਲ irq_handler_t handler)
{
  ਪੂਰਨ id;
  irq_hook_t **ਸਤਰ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bitmap;

  ਜੇ( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  ਸਤਰ = &irq_handlers[irq];

  bitmap = 0;
  ਜਦੋਂ ( *ਸਤਰ != ਨਿਰਾ ) {
	ਜੇ(hook == *ਸਤਰ) ਵਾਪਸ; /* extra initialization */
	bitmap |= (*ਸਤਰ)->id;	/* mark ids in use */
	ਸਤਰ = &(*ਸਤਰ)->ਅਗਲਾ;
  }

  /* find the lowest id not in use */
  ਲਈ (id = 1; id != 0; id <<= 1)
  	ਜੇ (!(bitmap & id)) ਰੋਕੋ;

  ਜੇ(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->ਅਗਲਾ = ਨਿਰਾ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *ਸਤਰ = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ਜੇ((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_ਵਰਤਿਆ(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
ਖਾਲੀ rm_irq_handler( ਅਚਲ irq_hook_t* hook ) {
  ਅਚਲ ਪੂਰਨ irq = hook->irq; 
  ਅਚਲ ਪੂਰਨ id = hook->id;
  irq_hook_t **ਸਤਰ;

  ਜੇ( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  ਸਤਰ = &irq_handlers[irq];
  ਜਦੋਂ( (*ਸਤਰ) != ਨਿਰਾ ) {
	ਜੇ((*ਸਤਰ)->id == id) {
		(*ਸਤਰ) = (*ਸਤਰ)->ਅਗਲਾ;
		ਜੇ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	ਨਹੀਂਤਾਂ {
		ਸਤਰ = &(*ਸਤਰ)->ਅਗਲਾ;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ਜੇ (irq_handlers[irq] == ਨਿਰਾ) {
	hw_intr_ਨਕਾਬ(irq);
	hw_intr_not_ਵਰਤਿਆ(irq);
  }
  ਨਹੀਂਤਾਂ ਜੇ (irq_actids[irq] == 0) {
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
ਖਾਲੀ irq_handle(ਪੂਰਨ irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ਪ੍ਰਤਿਜ੍ਞਾ(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_ਨਕਾਬ(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ਜੇ(hook == ਨਿਰਾ) {
      ਸਥਿਰ ਪੂਰਨ nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ਜੇ(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	ਲਿਖੋ("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ਜੇ(report_interval < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ/2)
		report_interval *= 2;
      }
      ਵਾਪਸ;
  }

  /* Call list of handlers for an IRQ. */
  ਜਦੋਂ( hook != ਨਿਰਾ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ਜੇ( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->ਅਗਲਾ;
  }

  /* reenable the IRQ only if there is no active handler */
  ਜੇ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
ਖਾਲੀ enable_irq(ਅਚਲ irq_hook_t *hook)
{
  ਜੇ((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
ਪੂਰਨ disable_irq(ਅਚਲ irq_hook_t *hook)
{
  ਜੇ(irq_actids[hook->irq] & hook->id)  /* already disabled */
    ਵਾਪਸ 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_ਨਕਾਬ(hook->irq);
  ਵਾਪਸ TRUE;
}

