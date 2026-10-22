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

#અન્તર્ભાવ <assert.h>

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
સ્થિર irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
શૂન્ય put_irq_handler( irq_hook_t* hook, પૂર્ણાંક irq,
  અચલ irq_handler_t handler)
{
  પૂર્ણાંક id;
  irq_hook_t **લીટી;
  unsigned long bitmap;

  જો( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  લીટી = &irq_handlers[irq];

  bitmap = 0;
  જ્યારે ( *લીટી != ખાલી ) {
	જો(hook == *લીટી) પાછા; /* extra initialization */
	bitmap |= (*લીટી)->id;	/* mark ids in use */
	લીટી = &(*લીટી)->next;
  }

  /* find the lowest id not in use */
  માટે (id = 1; id != 0; id <<= 1)
  	જો (!(bitmap & id)) તોડો;

  જો(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = ખાલી;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *લીટી = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  જો((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
શૂન્ય rm_irq_handler( અચલ irq_hook_t* hook ) {
  અચલ પૂર્ણાંક irq = hook->irq; 
  અચલ પૂર્ણાંક id = hook->id;
  irq_hook_t **લીટી;

  જો( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  લીટી = &irq_handlers[irq];
  જ્યારે( (*લીટી) != ખાલી ) {
	જો((*લીટી)->id == id) {
		(*લીટી) = (*લીટી)->next;
		જો (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	નહીંતર {
		લીટી = &(*લીટી)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  જો (irq_handlers[irq] == ખાલી) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  નહીંતર જો (irq_actids[irq] == 0) {
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
શૂન્ય irq_handle(પૂર્ણાંક irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  પ્રતિજ્ઞા(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  જો(hook == ખાલી) {
      સ્થિર પૂર્ણાંક nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      જો(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	જો(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      પાછા;
  }

  /* Call list of handlers for an IRQ. */
  જ્યારે( hook != ખાલી ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    જો( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  જો (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
શૂન્ય enable_irq(અચલ irq_hook_t *hook)
{
  જો((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
પૂર્ણાંક disable_irq(અચલ irq_hook_t *hook)
{
  જો(irq_actids[hook->irq] & hook->id)  /* already disabled */
    પાછા 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  પાછા TRUE;
}

