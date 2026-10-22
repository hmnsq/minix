#अगर_अघोषित _OMAP_SERIAL_H
#घोषणा _OMAP_SERIAL_H

/* UART register map */
#घोषणा OMAP3_DM37XX_DEBUG_UART_BASE 0x49020000 /* UART3 physical address */
#घोषणा OMAP3_AM335X_DEBUG_UART_BASE 0x44E09000 /* UART0? physical address */

/* UART registers */
#घोषणा OMAP3_THR 0x000 /* Transmit holding register */
#घोषणा OMAP3_LSR 0x014 /* Line status register */
#घोषणा OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#घोषणा OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#घोषणा OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#घोषणा OMAP3_SSR_TX_FIFO_FULL (1 << 0) /* Transmit FIFO full */

#अगर_अघोषित __ASSEMBLY__

व्योम omap3_ser_init(व्योम);
व्योम omap3_ser_putc(अक्षर c);

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* _OMAP_SERIAL_H */
