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

#അന്തര്ഭാവ <assert.h>

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
സ്ഥിതം irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
ശൂന്യം put_irq_handler( irq_hook_t* hook, പൂർണ്ണസംഖ്യ irq,
  സ്ഥിരം irq_handler_t handler)
{
  പൂർണ്ണസംഖ്യ id;
  irq_hook_t **വരി;
  unsigned long bitmap;

  എങ്കിൽ( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  വരി = &irq_handlers[irq];

  bitmap = 0;
  ആകുമ്പോൾ ( *വരി != ശൂന്യ ) {
	എങ്കിൽ(hook == *വരി) തിരികെ; /* extra initialization */
	bitmap |= (*വരി)->id;	/* mark ids in use */
	വരി = &(*വരി)->next;
  }

  /* find the lowest id not in use */
  വേണ്ടി (id = 1; id != 0; id <<= 1)
  	എങ്കിൽ (!(bitmap & id)) നിർത്തുക;

  എങ്കിൽ(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = ശൂന്യ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *വരി = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  എങ്കിൽ((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
ശൂന്യം rm_irq_handler( സ്ഥിരം irq_hook_t* hook ) {
  സ്ഥിരം പൂർണ്ണസംഖ്യ irq = hook->irq; 
  സ്ഥിരം പൂർണ്ണസംഖ്യ id = hook->id;
  irq_hook_t **വരി;

  എങ്കിൽ( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  വരി = &irq_handlers[irq];
  ആകുമ്പോൾ( (*വരി) != ശൂന്യ ) {
	എങ്കിൽ((*വരി)->id == id) {
		(*വരി) = (*വരി)->next;
		എങ്കിൽ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	അല്ലെങ്കിൽ {
		വരി = &(*വരി)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  എങ്കിൽ (irq_handlers[irq] == ശൂന്യ) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  അല്ലെങ്കിൽ എങ്കിൽ (irq_actids[irq] == 0) {
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
ശൂന്യം irq_handle(പൂർണ്ണസംഖ്യ irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  പ്രതിജ്ഞാ(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  എങ്കിൽ(hook == ശൂന്യ) {
      സ്ഥിതം പൂർണ്ണസംഖ്യ nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      എങ്കിൽ(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	എങ്കിൽ(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      തിരികെ;
  }

  /* Call list of handlers for an IRQ. */
  ആകുമ്പോൾ( hook != ശൂന്യ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    എങ്കിൽ( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  എങ്കിൽ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
ശൂന്യം enable_irq(സ്ഥിരം irq_hook_t *hook)
{
  എങ്കിൽ((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
പൂർണ്ണസംഖ്യ disable_irq(സ്ഥിരം irq_hook_t *hook)
{
  എങ്കിൽ(irq_actids[hook->irq] & hook->id)  /* already disabled */
    തിരികെ 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  തിരികെ TRUE;
}

