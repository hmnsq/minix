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

#ଅନ୍ତର୍ଭାଵ <assert.h>

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
ସ୍ଥିର irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
ଶୂନ୍ୟ put_irq_handler( irq_hook_t* hook, ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq,
  ଅଚଳ irq_handler_t handler)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;
  irq_hook_t **ଧାଡ଼ି;
  unsigned long bitmap;

  ଯଦି( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  ଧାଡ଼ି = &irq_handlers[irq];

  bitmap = 0;
  ଯେତେବେଳେ ( *ଧାଡ଼ି != ଖାଲି ) {
	ଯଦି(hook == *ଧାଡ଼ି) ଫେରନ୍ତୁ; /* extra initialization */
	bitmap |= (*ଧାଡ଼ି)->id;	/* mark ids in use */
	ଧାଡ଼ି = &(*ଧାଡ଼ି)->next;
  }

  /* find the lowest id not in use */
  ପାଇଁ (id = 1; id != 0; id <<= 1)
  	ଯଦି (!(bitmap & id)) ଭାଙ୍ଗ;

  ଯଦି(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = ଖାଲି;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *ଧାଡ଼ି = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ଯଦି((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
ଶୂନ୍ୟ rm_irq_handler( ଅଚଳ irq_hook_t* hook ) {
  ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq = hook->irq; 
  ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = hook->id;
  irq_hook_t **ଧାଡ଼ି;

  ଯଦି( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  ଧାଡ଼ି = &irq_handlers[irq];
  ଯେତେବେଳେ( (*ଧାଡ଼ି) != ଖାଲି ) {
	ଯଦି((*ଧାଡ଼ି)->id == id) {
		(*ଧାଡ଼ି) = (*ଧାଡ଼ି)->next;
		ଯଦି (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	ଅନ୍ୟଥା {
		ଧାଡ଼ି = &(*ଧାଡ଼ି)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ଯଦି (irq_handlers[irq] == ଖାଲି) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  ଅନ୍ୟଥା ଯଦି (irq_actids[irq] == 0) {
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
ଶୂନ୍ୟ irq_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ପ୍ରତିଜ୍ଞା(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ଯଦି(hook == ଖାଲି) {
      ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ଯଦି(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ଯଦି(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      ଫେରନ୍ତୁ;
  }

  /* Call list of handlers for an IRQ. */
  ଯେତେବେଳେ( hook != ଖାଲି ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ଯଦି( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  ଯଦି (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
ଶୂନ୍ୟ enable_irq(ଅଚଳ irq_hook_t *hook)
{
  ଯଦି((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା disable_irq(ଅଚଳ irq_hook_t *hook)
{
  ଯଦି(irq_actids[hook->irq] & hook->id)  /* already disabled */
    ଫେରନ୍ତୁ 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  ଫେରନ୍ତୁ TRUE;
}

