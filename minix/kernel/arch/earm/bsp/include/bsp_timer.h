#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

ಶೂನ್ಯ bsp_timer_init(unsigned freq);
ಶೂನ್ಯ bsp_timer_stop(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ  bsp_register_timer_handler(ಧ್ರುವ irq_handler_t handler);
ಶೂನ್ಯ bsp_timer_int_handler(ಶೂನ್ಯ);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
