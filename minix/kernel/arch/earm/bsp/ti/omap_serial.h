#جے_بنا_تعریف _OMAP_SERIAL_H
#تعریف_کرو _OMAP_SERIAL_H

/* UART register map */
#تعریف_کرو OMAP3_DM37_XX_DEBUG_UART_ادھار 0x49020000 /* UART3 physical address */
#تعریف_کرو OMAP3_AM335_X_DEBUG_UART_ادھار 0x44E09000 /* UART0? physical address */

/* UART registers */
#تعریف_کرو OMAP3_THR 0x000 /* Transmit holding register */
#تعریف_کرو OMAP3_LSR 0x014 /* Line status register */
#تعریف_کرو OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#تعریف_کرو OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#تعریف_کرو OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#تعریف_کرو OMAP3_SSR_TX_FIFO_بھریا (1 << 0) /* Transmit FIFO full */

#جے_بنا_تعریف __ASSEMBLY__

فراغ omap3_ser_init(فراغ);
فراغ omap3_ser_putc(حرف c);

#جے_انت /* __ASSEMBLY__ */

#جے_انت /* _OMAP_SERIAL_H */
