#నిర్వచితం_కాకపోతే TYPE_H
#నిర్వచించు TYPE_H

#అన్తర్భావ <minix/com.h>
#అన్తర్భావ <machine/interrupt.h>
#అన్తర్భావ <machine/multiboot.h>

/* Process table and system property related types. */ 
రకనిర్వచనం పూర్ణాంకం proc_nr_t;			/* process table entry number */
రకనిర్వచనం పొట్టి sys_id_t;			/* system process index */
రకనిర్వచనం నిర్మాణం {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

రకనిర్వచనం గుర్తులేని దీర్ఘ irq_policy_t;	
రకనిర్వచనం గుర్తులేని దీర్ఘ irq_id_t;	

రకనిర్వచనం నిర్మాణం irq_hook {
  నిర్మాణం irq_hook *తదుపరి;		/* next hook in chain */
  పూర్ణాంకం (*handler)(నిర్మాణం irq_hook *);	/* interrupt handler */
  పూర్ణాంకం irq;				/* IRQ vector number */ 
  పూర్ణాంకం id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

రకనిర్వచనం పూర్ణాంకం (*irq_handler_t)(నిర్మాణం irq_hook *);

#అయితే_ముగింపు /* TYPE_H */
