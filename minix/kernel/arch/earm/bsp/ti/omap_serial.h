#که_نه_تعریف_شوی _OMAP_SERIAL_H
#تعریف_کړه _OMAP_SERIAL_H

/* UART register map */
#تعریف_کړه OMAP3_DM37_XX_DEBUG_UART_بنسټ 0x49020000 /* UART3 physical address */
#تعریف_کړه OMAP3_AM335_X_DEBUG_UART_بنسټ 0x44E09000 /* UART0? physical address */

/* UART registers */
#تعریف_کړه OMAP3_THR 0x000 /* Transmit holding register */
#تعریف_کړه OMAP3_LSR 0x014 /* Line status register */
#تعریف_کړه OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#تعریف_کړه OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#تعریف_کړه OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#تعریف_کړه OMAP3_SSR_TX_FIFO_ډک (1 << 0) /* Transmit FIFO full */

#که_نه_تعریف_شوی __ASSEMBLY__

فراغ omap3_ser_init(فراغ);
فراغ omap3_ser_putc(حرف c);

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای /* _OMAP_SERIAL_H */
