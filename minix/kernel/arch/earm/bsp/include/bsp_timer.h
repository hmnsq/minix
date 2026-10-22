#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

শূন্য bsp_timer_init(unsigned freq);
শূন্য bsp_timer_stop(শূন্য);
পূর্ণসংখ্যা  bsp_register_timer_handler(ধ্রুবক irq_handler_t handler);
শূন্য bsp_timer_int_handler(শূন্য);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
