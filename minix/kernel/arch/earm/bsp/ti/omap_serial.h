#اگر_غیر_تعریف_شدہ _OMAP_SERIAL_H
#تعریف_کرو _OMAP_SERIAL_H

/* UART register map */
#تعریف_کرو OMAP3_DM37_XX_DEBUG_UART_بنیاد 0x49020000 /* UART3 physical address */
#تعریف_کرو OMAP3_AM335_X_DEBUG_UART_بنیاد 0x44E09000 /* UART0? physical address */

/* UART registers */
#تعریف_کرو OMAP3_THR 0x000 /* Transmit holding register */
#تعریف_کرو OMAP3_LSR 0x014 /* Line status register */
#تعریف_کرو OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#تعریف_کرو OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#تعریف_کرو OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#تعریف_کرو OMAP3_SSR_TX_FIFO_بھرا (1 << 0) /* Transmit FIFO full */

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

خالی omap3_ser_init(خالی);
خالی omap3_ser_putc(حرف c);

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* _OMAP_SERIAL_H */
