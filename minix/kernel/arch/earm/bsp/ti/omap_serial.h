#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _OMAP_SERIAL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _OMAP_SERIAL_H

/* UART register map */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_DM37_XX_DEBUG_UART_ਅਧਾਰ 0x49020000 /* UART3 physical address */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_AM335_X_DEBUG_UART_ਅਧਾਰ 0x44E09000 /* UART0? physical address */

/* UART registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_THR 0x000 /* Transmit holding register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_LSR 0x014 /* Line status register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SSR_TX_FIFO_ਭਰਿਆ (1 << 0) /* Transmit FIFO full */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

ਖਾਲੀ omap3_ser_init(ਖਾਲੀ);
ਖਾਲੀ omap3_ser_putc(ਅੱਖਰ c);

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* _OMAP_SERIAL_H */
