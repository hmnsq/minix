#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _OMAP_SERIAL_H
#ವ್ಯಾಖ್ಯಾನಿಸು _OMAP_SERIAL_H

/* UART register map */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_DM37_XX_DEBUG_UART_ತಳ 0x49020000 /* UART3 physical address */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_AM335_X_DEBUG_UART_ತಳ 0x44E09000 /* UART0? physical address */

/* UART registers */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_THR 0x000 /* Transmit holding register */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_LSR 0x014 /* Line status register */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_SSR 0x044 /* Supplementary status register */

/* Line status register fields */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_LSR_TEMT    0x40 /* Transmitter empty */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_LSR_THRE    0x20 /* Transmit-hold-register empty */

/* Supplementary status register fields */
#ವ್ಯಾಖ್ಯಾನಿಸು OMAP3_SSR_TX_FIFO_ತುಂಬಿದ (1 << 0) /* Transmit FIFO full */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

ಶೂನ್ಯ omap3_ser_init(ಶೂನ್ಯ);
ಶೂನ್ಯ omap3_ser_putc(ಅಕ್ಷರ c);

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* _OMAP_SERIAL_H */
