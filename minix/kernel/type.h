#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TYPE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPE_H

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/com.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/multiboot.h>

/* Process table and system property related types. */ 
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ proc_nr_t;			/* process table entry number */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਛੋਟਾ sys_id_t;			/* system process index */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {			/* bitmap for system indexes */
  bitchunk_t chunk[BITMAP_CHUNKS(NR_SYS_PROCS)];
} sys_map_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irq_policy_t;	
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irq_id_t;	

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ irq_hook {
  ਬਣਤਰ irq_hook *ਅਗਲਾ;		/* next hook in chain */
  ਪੂਰਨ (*handler)(ਬਣਤਰ irq_hook *);	/* interrupt handler */
  ਪੂਰਨ irq;				/* IRQ vector number */ 
  ਪੂਰਨ id;				/* id of this hook */
  endpoint_t proc_nr_e;			/* (endpoint) NONE if not in use */
  irq_id_t notify_id;			/* id to return on interrupt */
  irq_policy_t policy;			/* bit mask for policy */
} irq_hook_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*irq_handler_t)(ਬਣਤਰ irq_hook *);

#ਜੇ_ਅੰਤ /* TYPE_H */
