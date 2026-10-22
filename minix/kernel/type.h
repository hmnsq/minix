#ifndef TYPE_H
#define TYPE_H

#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <machine/interrupt.h>
#ಅನ್ತರ್ಭಾವ <machine/multiboot.h>

/* Process table and system property related types. */ 
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ proc_nr_t;			/* process table entry number */
ಪ್ರಕಾರನಿರ್ದೇಶ short sys_id_t;			/* system process index */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long irq_policy_t;	
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long irq_id_t;	

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ irq_hook {
  ಸಂರಚನಾ irq_hook *next;		/* next hook in chain */
  ಪೂರ್ಣಾಙ್ಕ (*handler)(ಸಂರಚನಾ irq_hook *);	/* interrupt handler */
  ಪೂರ್ಣಾಙ್ಕ irq;				/* IRQ vector number */ 
  ಪೂರ್ಣಾಙ್ಕ id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*irq_handler_t)(ಸಂರಚನಾ irq_hook *);

#endif /* TYPE_H */
