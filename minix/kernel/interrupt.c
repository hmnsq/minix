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

#include <assert.h>

#include "kernel/kernel.h"
#include "hw_intr.h"


/* number of lists of IRQ hooks, one list per supported line. */
static irq_hook_t* irq_handlers[NR_IRQ_VECTORS] = {0};

/*===========================================================================*
 *				put_irq_handler				     *
 *===========================================================================*/
/* Register an interrupt handler.  */
void put_irq_handler( irq_hook_t* hook, цео irq,
  const irq_handler_t handler)
{
  цео id;
  irq_hook_t **line;
  unsigned long bitmap;

  ако( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  line = &irq_handlers[irq];

  bitmap = 0;
  док ( *line != NULL ) {
	ако(hook == *line) врати; /* extra initialization */
	bitmap |= (*line)->id;	/* mark ids in use */
	line = &(*line)->next;
  }

  /* find the lowest id not in use */
  за (id = 1; id != 0; id <<= 1)
  	ако (!(bitmap & id)) break;

  ако(id == 0)
  	panic("Too many handlers for irq: %d",  irq);
  
  hook->next = NULL;
  hook->handler = handler;
  hook->irq = irq;
  hook->id = id;
  *line = hook;

  /* And as last enable the irq at the hardware.
   *
   * Internal this activates the line or source of the given interrupt.
   */
  ако((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
void rm_irq_handler( const irq_hook_t* hook ) {
  const цео irq = hook->irq; 
  const цео id = hook->id;
  irq_hook_t **line;

  ако( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  line = &irq_handlers[irq];
  док( (*line) != NULL ) {
	ако((*line)->id == id) {
		(*line) = (*line)->next;
		ако (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	инако {
		line = &(*line)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  ако (irq_handlers[irq] == NULL) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  инако ако (irq_actids[irq] == 0) {
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
void irq_handle(цео irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  assert(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  ако(hook == NULL) {
      static цео nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      ако(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	ако(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      врати;
  }

  /* Call list of handlers for an IRQ. */
  док( hook != NULL ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    ако( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  ако (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
void enable_irq(const irq_hook_t *hook)
{
  ако((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
цео disable_irq(const irq_hook_t *hook)
{
  ако(irq_actids[hook->irq] & hook->id)  /* already disabled */
    врати 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  врати TRUE;
}

