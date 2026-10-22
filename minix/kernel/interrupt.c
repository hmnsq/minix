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

#అన్తర్భావ <assert.h>

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
స్థిర irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
శూన్య put_irq_handler( irq_hook_t* hook, పూర్ణాంకం irq,
  స్థిరరాశి irq_handler_t handler)
{
  పూర్ణాంకం id;
  irq_hook_t **పంక్తి;
  గుర్తులేని దీర్ఘ bitmap;

  ఒకవేళ( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  పంక్తి = &irq_handlers[irq];

  bitmap = 0;
  అయితే ( *పంక్తి != ఖాళీ ) {
	ఒకవేళ(hook == *పంక్తి) తిరిగి; /* extra initialization */
	bitmap |= (*పంక్తి)->id;	/* mark ids in use */
	పంక్తి = &(*పంక్తి)->తదుపరి;
  }

  /* find the lowest id not in use */
  కొరకు (id = 1; id != 0; id <<= 1)
  	ఒకవేళ (!(bitmap & id)) ఆపు;

  ఒకవేళ(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->తదుపరి = ఖాళీ;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *పంక్తి = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ఒకవేళ((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_వాడిన(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
శూన్య rm_irq_handler( స్థిరరాశి irq_hook_t* hook ) {
  స్థిరరాశి పూర్ణాంకం irq = hook->irq; 
  స్థిరరాశి పూర్ణాంకం id = hook->id;
  irq_hook_t **పంక్తి;

  ఒకవేళ( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  పంక్తి = &irq_handlers[irq];
  అయితే( (*పంక్తి) != ఖాళీ ) {
	ఒకవేళ((*పంక్తి)->id == id) {
		(*పంక్తి) = (*పంక్తి)->తదుపరి;
		ఒకవేళ (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	లేకపోతే {
		పంక్తి = &(*పంక్తి)->తదుపరి;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ఒకవేళ (irq_handlers[irq] == ఖాళీ) {
	hw_intr_ముసుగు(irq);
	hw_intr_not_వాడిన(irq);
  }
  లేకపోతే ఒకవేళ (irq_actids[irq] == 0) {
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
శూన్య irq_handle(పూర్ణాంకం irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  ప్రతిజ్ఞా(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_ముసుగు(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ఒకవేళ(hook == ఖాళీ) {
      స్థిర పూర్ణాంకం nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ఒకవేళ(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	రాయి("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ఒకవేళ(report_interval < గరిష్ఠ_పూర్ణాంకం/2)
		report_interval *= 2;
      }
      తిరిగి;
  }

  /* Call list of handlers for an IRQ. */
  అయితే( hook != ఖాళీ ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ఒకవేళ( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->తదుపరి;
  }

  /* reenable the IRQ only if there is no active handler */
  ఒకవేళ (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
శూన్య enable_irq(స్థిరరాశి irq_hook_t *hook)
{
  ఒకవేళ((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
పూర్ణాంకం disable_irq(స్థిరరాశి irq_hook_t *hook)
{
  ఒకవేళ(irq_actids[hook->irq] & hook->id)  /* already disabled */
    తిరిగి 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_ముసుగు(hook->irq);
  తిరిగి TRUE;
}

