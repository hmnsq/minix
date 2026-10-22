#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

ശൂന്യം bsp_timer_init(unsigned freq);
ശൂന്യം bsp_timer_stop(ശൂന്യം);
പൂർണ്ണസംഖ്യ  bsp_register_timer_handler(സ്ഥിരം irq_handler_t handler);
ശൂന്യം bsp_timer_int_handler(ശൂന്യം);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
