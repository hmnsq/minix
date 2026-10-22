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
शून्य put_irq_handler( irq_hook_t* hook, पूर्णाङ्क irq,
  ध्रुव irq_handler_t handler)
{
  पूर्णाङ्क id;
  irq_hook_t **पङ्क्तिः;
  अचिह्नित दीर्घ bitmap;

  यदि( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  पङ्क्तिः = &irq_handlers[irq];

  bitmap = 0;
  यावत् ( *पङ्क्तिः != रिक्त ) {
	यदि(hook == *पङ्क्तिः) प्रत्यागम; /* extra initialization */
	bitmap |= (*पङ्क्तिः)->id;	/* mark ids in use */
	पङ्क्तिः = &(*पङ्क्तिः)->अग्र;
  }

  /* find the lowest id not in use */
  प्रति (id = 1; id != 0; id <<= 1)
  	यदि (!(bitmap & id)) भङ्ग;

  यदि(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->अग्र = रिक्त;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *पङ्क्तिः = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  यदि((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_प्रयुक्तम्(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
शून्य rm_irq_handler( ध्रुव irq_hook_t* hook ) {
  ध्रुव पूर्णाङ्क irq = hook->irq; 
  ध्रुव पूर्णाङ्क id = hook->id;
  irq_hook_t **पङ्क्तिः;

  यदि( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  पङ्क्तिः = &irq_handlers[irq];
  यावत्( (*पङ्क्तिः) != रिक्त ) {
	यदि((*पङ्क्तिः)->id == id) {
		(*पङ्क्तिः) = (*पङ्क्तिः)->अग्र;
		यदि (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	अन्यथा {
		पङ्क्तिः = &(*पङ्क्तिः)->अग्र;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  यदि (irq_handlers[irq] == रिक्त) {
	hw_intr_आवरणम्(irq);
	hw_intr_not_प्रयुक्तम्(irq);
  }
  अन्यथा यदि (irq_actids[irq] == 0) {
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
शून्य irq_handle(पूर्णाङ्क irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  प्रतिज्ञा(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_आवरणम्(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  यदि(hook == रिक्त) {
      स्थिर पूर्णाङ्क nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      यदि(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	प्रारूप_लिख("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	यदि(report_interval < अधिकतम_पूर्णाङ्कः/2)
		report_interval *= 2;
      }
      प्रत्यागम;
  }

  /* Call list of handlers for an IRQ. */
  यावत्( hook != रिक्त ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    यदि( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->अग्र;
  }

  /* reenable the IRQ only if there is no active handler */
  यदि (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
शून्य enable_irq(ध्रुव irq_hook_t *hook)
{
  यदि((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
पूर्णाङ्क disable_irq(ध्रुव irq_hook_t *hook)
{
  यदि(irq_actids[hook->irq] & hook->id)  /* already disabled */
    प्रत्यागम 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_आवरणम्(hook->irq);
  प्रत्यागम TRUE;
}

