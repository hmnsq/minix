#నిర్వచితం_కాకపోతే _OMAP_SERIAL_H
#నిర్వచించు _OMAP_SERIAL_H

/* UART register map */
#నిర్వచించు OMAP3_DM37_XX_DEBUG_UART_పునాది 0x49020000 /* UART3 physical address */
#నిర్వచించు OMAP3_AM335_X_DEBUG_UART_పునాది 0x44E09000 /* UART0? physical address */

/* UART registers */
#నిర్వచించు OMAP3_THR 0x000 /* Transmit holding register */
#నిర్వచించు OMAP3_LSR 0x014 /* Line status register */
#నిర్వచించు OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#నిర్వచించు OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#నిర్వచించు OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#నిర్వచించు OMAP3_SSR_TX_FIFO_నిండిన (1 << 0) /* Transmit FIFO full */

#నిర్వచితం_కాకపోతే __ASSEMBLY__

శూన్య omap3_ser_init(శూన్య);
శూన్య omap3_ser_putc(అక్షరం c);

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* _OMAP_SERIAL_H */
