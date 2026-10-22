#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

تهی bsp_timer_init(unsigned freq);
تهی bsp_timer_stop(تهی);
صحیح  bsp_register_timer_handler(ثابت irq_handler_t handler);
تهی bsp_timer_int_handler(تهی);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
