#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

வெறுமை bsp_timer_init(unsigned freq);
வெறுமை bsp_timer_stop(வெறுமை);
முழு  bsp_register_timer_handler(மாறிலி irq_handler_t handler);
வெறுமை bsp_timer_int_handler(வெறுமை);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
