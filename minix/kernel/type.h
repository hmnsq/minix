#ifndef TYPE_H
#define TYPE_H

#include <minix/com.h>
#include <machine/interrupt.h>
#include <machine/multiboot.h>

/* Process table and system property related types. */ 
typedef цяло proc_nr_t;			/* process table entry number */
typedef short sys_id_t;			/* system process index */
typedef struct {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

typedef unsigned long irq_policy_t;	
typedef unsigned long irq_id_t;	

typedef struct irq_hook {
  struct irq_hook *next;		/* next hook in chain */
  цяло (*handler)(struct irq_hook *);	/* interrupt handler */
  цяло irq;				/* IRQ vector number */ 
  цяло id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

typedef цяло (*irq_handler_t)(struct irq_hook *);

#endif /* TYPE_H */
