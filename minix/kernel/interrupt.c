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
void put_irq_handler( irq_hook_t* hook, 整数 irq,
  const irq_handler_t handler)
{
  整数 id;
  irq_hook_t **line;
  unsigned long bitmap;

  如果( irq < 0 || irq >= NR_IRQ_VECTORS )
	panic("invalid call to put_irq_handler: %d",  irq);

  line = &irq_handlers[irq];

  bitmap = 0;
  当 ( *line != NULL ) {
	如果(hook == *line) 返回; /* extra initialization */
	bitmap |= (*line)->id;	/* mark ids in use */
	line = &(*line)->next;
  }

  /* find the lowest id not in use */
  为 (id = 1; id != 0; id <<= 1)
  	如果 (!(bitmap & id)) break;

  如果(id == 0)
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
  如果((irq_actids[hook->irq] &= ~hook->id) == 0) {
	  hw_intr_used(irq);
	  hw_intr_unmask(hook->irq);
  }
}

/*===========================================================================*
 *				rm_irq_handler				     *
 *===========================================================================*/
/* Unregister an interrupt handler.  */
void rm_irq_handler( const irq_hook_t* hook ) {
  const 整数 irq = hook->irq; 
  const 整数 id = hook->id;
  irq_hook_t **line;

  如果( irq < 0 || irq >= NR_IRQ_VECTORS ) 
	panic("invalid call to rm_irq_handler: %d",  irq);

  /* remove the hook  */
  line = &irq_handlers[irq];
  当( (*line) != NULL ) {
	如果((*line)->id == id) {
		(*line) = (*line)->next;
		如果 (irq_actids[irq] & id)
			irq_actids[irq] &= ~id;
    	}
    	否则 {
		line = &(*line)->next;
    	}
  }

  /* Disable the irq if there are no other handlers registered.
   * If the irq is shared, reenable it if there is no active handler.
   */
  如果 (irq_handlers[irq] == NULL) {
	hw_intr_mask(irq);
	hw_intr_not_used(irq);
  }
  否则 如果 (irq_actids[irq] == 0) {
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
void irq_handle(整数 irq)
{
  irq_hook_t * hook;

  /* here we need not to get this IRQ until all the handlers had a say */
  assert(irq >= 0 && irq < NR_IRQ_VECTORS);
  hw_intr_mask(irq);
  hook = irq_handlers[irq];

  /* Check for spurious interrupts. */
  如果(hook == NULL) {
      static 整数 nspurious[NR_IRQ_VECTORS], report_interval = 100;
      nspurious[irq]++;
      如果(nspurious[irq] == 1 || !(nspurious[irq] % report_interval)) {
      	printf("irq_handle: spurious irq %d (count: %d); keeping masked\n",
		irq, nspurious[irq]);
	如果(report_interval < INT_MAX/2)
		report_interval *= 2;
      }
      返回;
  }

  /* Call list of handlers for an IRQ. */
  当( hook != NULL ) {
    /* For each handler in the list, mark it active by setting its ID bit,
     * call the function, and unmark it if the function returns true.
     */
    irq_actids[irq] |= hook->id;
    
    /* Call the hooked function. */
    如果( (*hook->handler)(hook) )
      irq_actids[hook->irq] &= ~hook->id;
    
    /* Next hooked function. */
    hook = hook->next;
  }

  /* reenable the IRQ only if there is no active handler */
  如果 (irq_actids[irq] == 0)
	  hw_intr_unmask(irq);

  hw_intr_ack(irq);
}

/* Enable/Disable a interrupt line.  */
void enable_irq(const irq_hook_t *hook)
{
  如果((irq_actids[hook->irq] &= ~hook->id) == 0) {
    hw_intr_unmask(hook->irq);
  }
}

/* Return true if the interrupt was enabled before call.  */
整数 disable_irq(const irq_hook_t *hook)
{
  如果(irq_actids[hook->irq] & hook->id)  /* already disabled */
    返回 0;
  irq_actids[hook->irq] |= hook->id;
  hw_intr_mask(hook->irq);
  返回 TRUE;
}

