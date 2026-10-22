#اگر_غیر_تعریف_شدہ _BSP_TIMER_H_
#تعریف_کرو _BSP_TIMER_H_

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

خالی bsp_timer_init(غیرعلامتی freq);
خالی bsp_timer_stop(خالی);
صحیح  bsp_رجسٹر_timer_handler(ثابت irq_handler_t handler);
خالی bsp_timer_int_handler(خالی);

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* _BSP_TIMER_H_ */
