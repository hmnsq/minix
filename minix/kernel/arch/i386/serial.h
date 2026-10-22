
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _KERN_SERIAL_H
#ವ್ಯಾಖ್ಯಾನಿಸು _KERN_SERIAL_H 1

#ವ್ಯಾಖ್ಯಾನಿಸು THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#ವ್ಯಾಖ್ಯಾನಿಸು RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#ವ್ಯಾಖ್ಯಾನಿಸು DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#ವ್ಯಾಖ್ಯಾನಿಸು DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#ವ್ಯಾಖ್ಯಾನಿಸು FICRREG 2	/* FIFO control, write-only */
#ವ್ಯಾಖ್ಯಾನಿಸು LCRREG  3	/* line control, read/write */
#ವ್ಯಾಖ್ಯಾನಿಸು LSRREG  5	/* line status, read-only */
#ವ್ಯಾಖ್ಯಾನಿಸು SPRREG  7

#ವ್ಯಾಖ್ಯಾನಿಸು COM1_ತಳ	0x3F8
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_THR	(COM1_ತಳ + THRREG)
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_RBR	(COM1_ತಳ + RBRREG)
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_DLL	(COM1_ತಳ + DLLREG)
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_DLM	(COM1_ತಳ + DLMREG)
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_LCR	(COM1_ತಳ + LCRREG)
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_5_ಬಿಟ್	0x00 /* 5 bits per data word */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_6_ಬಿಟ್	0x01 /* 6 bits per data word */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_7_ಬಿಟ್	0x02 /* 7 bits per data word */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_8_ಬಿಟ್	0x03 /* 8 bits per data word */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_2STOP	0x04 /* 2 stop bits */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_NPAR	0x00 /* no parity */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_OPAR	0x08 /* odd parity */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_EPAR	0x18 /* even parity */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_BREAK	0x40 /* enable break */
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_DLAB	0x80 /* access DLAB registers */
#ವ್ಯಾಖ್ಯಾನಿಸು COM1_LSR	(COM1_ತಳ + LSRREG)
#ವ್ಯಾಖ್ಯಾನಿಸು         LSR_DR          0x01
#ವ್ಯಾಖ್ಯಾನಿಸು         LSR_THRE        0x20
#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_DLA         0x80

#ವ್ಯಾಖ್ಯಾನಿಸು UART_ತಳ_FREQ	115200U	

#ಆದರೆ_ಅಂತ್ಯ
