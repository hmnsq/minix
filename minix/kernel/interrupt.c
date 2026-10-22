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

#अन्तर्भाव <assert.h>

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
स्थिर irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
शून्य put_irq_handler( irq_hook_t* hook, पूर्णांक irq,
  स्थिरांक irq_handler_t handler)
{
  पूर्णांक id;
  irq_hook_t **ओळ;
  अचिन्हित दीर्घ bitmap;

  जर( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  ओळ = &irq_handlers[irq];

  bitmap = 0;
  पर्यंत ( *ओळ != रिक्त ) {
	जर(hook == *ओळ) परत; /* extra initialization */
	bitmap |= (*ओळ)->id;	/* mark ids in use */
	ओळ = &(*ओळ)->next;
  }

  /* find the lowest id not in use */
  साठी (id = 1; id != 0; id <<= 1)
  	जर (!(bitmap & id)) थांबा;

  जर(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = रिक्त;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *ओळ = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  जर((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
शून्य rm_irq_handler( स्थिरांक irq_hook_t* hook ) {
  स्थिरांक पूर्णांक irq = hook->irq; 
  स्थिरांक पूर्णांक id = hook->id;
  irq_hook_t **ओळ;

  जर( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  ओळ = &irq_handlers[irq];
  पर्यंत( (*ओळ) != रिक्त ) {
	जर((*ओळ)->id == id) {
		(*ओळ) = (*ओळ)->next;
		जर (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	अन्यथा {
		ओळ = &(*ओळ)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  जर (irq_handlers[irq] == रिक्त) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  अन्यथा जर (irq_actids[irq] == 0) {
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
शून्य irq_handle(पूर्णांक irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  प्रतिज्ञा(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  जर(hook == रिक्त) {
      स्थिर पूर्णांक nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      जर(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	म_लिखो("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	जर(report_interval < पूर्णांक_उच्च/2)
		report_interval *= 2;
      }
      परत;
  }

  /* Call list of handlers for an IRQ. */
  पर्यंत( hook != रिक्त ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    जर( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  जर (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
शून्य enable_irq(स्थिरांक irq_hook_t *hook)
{
  जर((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
पूर्णांक disable_irq(स्थिरांक irq_hook_t *hook)
{
  जर(irq_actids[hook->irq] & hook->id)  /* already disabled */
    परत 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  परत TRUE;
}

