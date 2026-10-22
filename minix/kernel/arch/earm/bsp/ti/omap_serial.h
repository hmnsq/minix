#यदि_अपरिभाषितम् _OMAP_SERIAL_H
#परिभाषय _OMAP_SERIAL_H

/* UART register map */
#परिभाषय OMAP3_DM37_XX_DEBUG_UART_आधारः 0x49020000 /* UART3 physical address */
#परिभाषय OMAP3_AM335_X_DEBUG_UART_आधारः 0x44E09000 /* UART0? physical address */

/* UART registers */
#परिभाषय OMAP3_THR 0x000 /* Transmit holding register */
#परिभाषय OMAP3_LSR 0x014 /* Line status register */
#परिभाषय OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#परिभाषय OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#परिभाषय OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#परिभाषय OMAP3_SSR_TX_FIFO_पूर्णम् (1 << 0) /* Transmit FIFO full */

#यदि_अपरिभाषितम् __ASSEMBLY__

शून्य omap3_ser_init(शून्य);
शून्य omap3_ser_putc(वर्ण c);

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* _OMAP_SERIAL_H */
