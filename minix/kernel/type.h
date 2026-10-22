#که_نه_تعریف_شوی TYPE_H
#تعریف_کړه TYPE_H

#ضمن <minix/com.h>
#ضمن <machine/interrupt.h>
#ضمن <machine/multiboot.h>

/* Process table and system property related types. */ 
تعريفنوع صحيح proc_nr_t;			/* process table entry number */
تعريفنوع لنډ sys_id_t;			/* system process index */
تعريفنوع بنية {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

تعريفنوع بینښه اوږد irq_policy_t;	
تعريفنوع بینښه اوږد irq_id_t;	

تعريفنوع بنية irq_hook {
  بنية irq_hook *بل;		/* next hook in chain */
  صحيح (*handler)(بنية irq_hook *);	/* interrupt handler */
  صحيح irq;				/* IRQ vector number */ 
  صحيح id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

تعريفنوع صحيح (*irq_handler_t)(بنية irq_hook *);

#د_که_پای /* TYPE_H */
