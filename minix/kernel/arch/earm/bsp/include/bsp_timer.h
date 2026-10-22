#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

فراغ bsp_timer_init(unsigned freq);
فراغ bsp_timer_stop(فراغ);
صحيح  bsp_register_timer_handler(ثابت irq_handler_t handler);
فراغ bsp_timer_int_handler(فراغ);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
