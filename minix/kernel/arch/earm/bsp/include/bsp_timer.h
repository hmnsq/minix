#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

ଶୂନ୍ୟ bsp_timer_init(unsigned freq);
ଶୂନ୍ୟ bsp_timer_stop(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  bsp_register_timer_handler(ଅଚଳ irq_handler_t handler);
ଶୂନ୍ୟ bsp_timer_int_handler(ଶୂନ୍ୟ);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
