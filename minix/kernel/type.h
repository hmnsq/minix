#अगर_अघोषित TYPE_H
#घोषणा TYPE_H

#समावेश <minix/com.h>
#समावेश <machine/interrupt.h>
#समावेश <machine/multiboot.h>

/* Process table and system property related types. */ 
प्रकार पूर्णांक proc_nr_t;			/* process table entry number */
प्रकार लघु sys_id_t;			/* system process index */
प्रकार काष्ठा {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

प्रकार अचिन्हित दीर्घ irq_policy_t;	
प्रकार अचिन्हित दीर्घ irq_id_t;	

प्रकार काष्ठा irq_hook {
  काष्ठा irq_hook *अग्र;		/* next hook in chain */
  पूर्णांक (*handler)(काष्ठा irq_hook *);	/* interrupt handler */
  पूर्णांक irq;				/* IRQ vector number */ 
  पूर्णांक id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

प्रकार पूर्णांक (*irq_handler_t)(काष्ठा irq_hook *);

#पूर्ण_अगर /* TYPE_H */
