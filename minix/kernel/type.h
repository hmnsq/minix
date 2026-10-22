#ifndef TYPE_H
#define TYPE_H

#include <minix/com.h>
#include <machine/interrupt.h>
#include <machine/multiboot.h>

/* Process table and system property related types. */ 
typedef පූර්ණ_සංඛ්‍යා proc_nr_t;			/* process table entry number */
typedef short sys_id_t;			/* system process index */
typedef sa_nracanaa {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

typedef unsigned long irq_policy_t;	
typedef unsigned long irq_id_t;	

typedef sa_nracanaa irq_hook {
  sa_nracanaa irq_hook *next;		/* next hook in chain */
  පූර්ණ_සංඛ්‍යා (*handler)(sa_nracanaa irq_hook *);	/* interrupt handler */
  පූර්ණ_සංඛ්‍යා irq;				/* IRQ vector number */ 
  පූර්ණ_සංඛ්‍යා id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

typedef පූර්ණ_සංඛ්‍යා (*irq_handler_t)(sa_nracanaa irq_hook *);

#endif /* TYPE_H */
