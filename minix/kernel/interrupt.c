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

#समावेश <assert.h>

#समावेश "kernel/kernel.h"
#समावेश "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
अटल irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
व्योम put_irq_handler( irq_hook_t* hook, पूर्णांक irq,
  स्थिर irq_handler_t handler)
{
  पूर्णांक id;
  irq_hook_t **पंक्ति_2;
  अचिन्हित दीर्घ bitmap;

  अगर( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  पंक्ति_2 = &irq_handlers[irq];

  bitmap = 0;
  जबतक ( *पंक्ति_2 != शून्य ) {
	अगर(hook == *पंक्ति_2) वापस; /* extra initialization */
	bitmap |= (*पंक्ति_2)->id;	/* mark ids in use */
	पंक्ति_2 = &(*पंक्ति_2)->अग्र;
  }

  /* find the lowest id not in use */
  क्रम (id = 1; id != 0; id <<= 1)
  	अगर (!(bitmap & id)) अवरोध;

  अगर(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->अग्र = शून्य;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *पंक्ति_2 = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  अगर((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
व्योम rm_irq_handler( स्थिर irq_hook_t* hook ) {
  स्थिर पूर्णांक irq = hook->irq; 
  स्थिर पूर्णांक id = hook->id;
  irq_hook_t **पंक्ति_2;

  अगर( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  पंक्ति_2 = &irq_handlers[irq];
  जबतक( (*पंक्ति_2) != शून्य ) {
	अगर((*पंक्ति_2)->id == id) {
		(*पंक्ति_2) = (*पंक्ति_2)->अग्र;
		अगर (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	अन्यथा {
		पंक्ति_2 = &(*पंक्ति_2)->अग्र;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  अगर (irq_handlers[irq] == शून्य) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  अन्यथा अगर (irq_actids[irq] == 0) {
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
व्योम irq_handle(पूर्णांक irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  निश्चित(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  अगर(hook == शून्य) {
      अटल पूर्णांक nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      अगर(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	म_लिखो("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	अगर(report_interval < पूर्णांक_उच्च/2)
		report_interval *= 2;
      }
      वापस;
  }

  /* Call list of handlers for an IRQ. */
  जबतक( hook != शून्य ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    अगर( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->अग्र;
  }

  /* reenable the IRQ only if there is no active handler */
  अगर (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
व्योम enable_irq(स्थिर irq_hook_t *hook)
{
  अगर((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
पूर्णांक disable_irq(स्थिर irq_hook_t *hook)
{
  अगर(irq_actids[hook->irq] & hook->id)  /* already disabled */
    वापस 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  वापस TRUE;
}

