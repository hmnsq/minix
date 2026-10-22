#ifndef TYPE_H
#define TYPE_H

#അന്തര്ഭാവ <minix/com.h>
#അന്തര്ഭാവ <machine/interrupt.h>
#അന്തര്ഭാവ <machine/multiboot.h>

/* Process table and system property related types. */ 
തരനിർവചനം പൂർണ്ണസംഖ്യ proc_nr_t;			/* process table entry number */
തരനിർവചനം short sys_id_t;			/* system process index */
തരനിർവചനം ഘടന {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

തരനിർവചനം unsigned long irq_policy_t;	
തരനിർവചനം unsigned long irq_id_t;	

തരനിർവചനം ഘടന irq_hook {
  ഘടന irq_hook *next;		/* next hook in chain */
  പൂർണ്ണസംഖ്യ (*handler)(ഘടന irq_hook *);	/* interrupt handler */
  പൂർണ്ണസംഖ്യ irq;				/* IRQ vector number */ 
  പൂർണ്ണസംഖ്യ id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

തരനിർവചനം പൂർണ്ണസംഖ്യ (*irq_handler_t)(ഘടന irq_hook *);

#endif /* TYPE_H */
