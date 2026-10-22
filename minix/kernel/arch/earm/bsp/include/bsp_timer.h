#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#ifndef __ASSEMBLY__

शून्य bsp_timer_init(अचिन्हित freq);
शून्य bsp_timer_stop(शून्य);
पूर्णाङ्क  bsp_register_timer_handler(अचर irq_handler_t handler);
शून्य bsp_timer_int_handler(शून्य);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_TIMER_H_ */
