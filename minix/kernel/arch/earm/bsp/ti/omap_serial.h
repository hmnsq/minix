#اگر_تعریف_نشده _OMAP_SERIAL_H
#تعریف_کن _OMAP_SERIAL_H

/* UART register map */
#تعریف_کن OMAP3_DM37_XX_DEBUG_UART_پایه 0x49020000 /* UART3 physical address */
#تعریف_کن OMAP3_AM335_X_DEBUG_UART_پایه 0x44E09000 /* UART0? physical address */

/* UART registers */
#تعریف_کن OMAP3_THR 0x000 /* Transmit holding register */
#تعریف_کن OMAP3_LSR 0x014 /* Line status register */
#تعریف_کن OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#تعریف_کن OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#تعریف_کن OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#تعریف_کن OMAP3_SSR_TX_FIFO_پر (1 << 0) /* Transmit FIFO full */

#اگر_تعریف_نشده __ASSEMBLY__

فراغ omap3_ser_init(فراغ);
فراغ omap3_ser_putc(حرف c);

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* _OMAP_SERIAL_H */
