#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

શૂન્ય bsp_timer_init(unsigned freq);
શૂન્ય bsp_timer_stop(શૂન્ય);
પૂર્ણાંક  bsp_register_timer_handler(અચલ irq_handler_t handler);
શૂન્ય bsp_timer_int_handler(શૂન્ય);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
