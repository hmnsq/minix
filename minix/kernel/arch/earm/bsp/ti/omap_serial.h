#جيڪڏهن_وصف_نه_ٿيل _OMAP_SERIAL_H
#وصف_ڪريو _OMAP_SERIAL_H

/* UART register map */
#وصف_ڪريو OMAP3_DM37_XX_DEBUG_UART_بنياد 0x49020000 /* UART3 physical address */
#وصف_ڪريو OMAP3_AM335_X_DEBUG_UART_بنياد 0x44E09000 /* UART0? physical address */

/* UART registers */
#وصف_ڪريو OMAP3_THR 0x000 /* Transmit holding register */
#وصف_ڪريو OMAP3_LSR 0x014 /* Line status register */
#وصف_ڪريو OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#وصف_ڪريو OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#وصف_ڪريو OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#وصف_ڪريو OMAP3_SSR_TX_FIFO_ڀريل (1 << 0) /* Transmit FIFO full */

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

فراغ omap3_ser_init(فراغ);
فراغ omap3_ser_putc(حرف c);

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو /* _OMAP_SERIAL_H */
