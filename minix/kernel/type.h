#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TYPE_H
#ವ್ಯಾಖ್ಯಾನಿಸು TYPE_H

#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <machine/interrupt.h>
#ಅನ್ತರ್ಭಾವ <machine/multiboot.h>

/* Process table and system property related types. */ 
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ proc_nr_t;			/* process table entry number */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಗಿಡ್ಡ sys_id_t;			/* system process index */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ irq_policy_t;	
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ irq_id_t;	

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ irq_hook {
  ರಚನೆ irq_hook *ಮುಂದಿನ;		/* next hook in chain */
  ಪೂರ್ಣಾಂಕ (*handler)(ರಚನೆ irq_hook *);	/* interrupt handler */
  ಪೂರ್ಣಾಂಕ irq;				/* IRQ vector number */ 
  ಪೂರ್ಣಾಂಕ id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*irq_handler_t)(ರಚನೆ irq_hook *);

#ಆದರೆ_ಅಂತ್ಯ /* TYPE_H */
