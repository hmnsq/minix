#اگر_تعریف_نشده _BSP_TIMER_H_
#تعریف_کن _BSP_TIMER_H_

#اگر_تعریف_نشده __ASSEMBLY__

فراغ bsp_timer_init(بیعلامت freq);
فراغ bsp_timer_stop(فراغ);
صحيح  bsp_ثبات_timer_handler(ثابت irq_handler_t handler);
فراغ bsp_timer_int_handler(فراغ);

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* _BSP_TIMER_H_ */
