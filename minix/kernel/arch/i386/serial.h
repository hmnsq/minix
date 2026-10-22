
#اگر_غیر_تعریف_شدہ _KERN_SERIAL_H
#تعریف_کرو _KERN_SERIAL_H 1

#تعریف_کرو THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#تعریف_کرو RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#تعریف_کرو DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#تعریف_کرو DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#تعریف_کرو FICRREG 2	/* FIFO control, write-only */
#تعریف_کرو LCRREG  3	/* line control, read/write */
#تعریف_کرو LSRREG  5	/* line status, read-only */
#تعریف_کرو SPRREG  7

#تعریف_کرو COM1_بنیاد	0x3F8
#تعریف_کرو COM1_THR	(COM1_بنیاد + THRREG)
#تعریف_کرو COM1_RBR	(COM1_بنیاد + RBRREG)
#تعریف_کرو COM1_DLL	(COM1_بنیاد + DLLREG)
#تعریف_کرو COM1_DLM	(COM1_بنیاد + DLMREG)
#تعریف_کرو COM1_LCR	(COM1_بنیاد + LCRREG)
#تعریف_کرو         LCR_5_ثنائی_ہندسہ	0x00 /* 5 bits per data word */
#تعریف_کرو         LCR_6_ثنائی_ہندسہ	0x01 /* 6 bits per data word */
#تعریف_کرو         LCR_7_ثنائی_ہندسہ	0x02 /* 7 bits per data word */
#تعریف_کرو         LCR_8_ثنائی_ہندسہ	0x03 /* 8 bits per data word */
#تعریف_کرو         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#تعریف_کرو         LCR_2STOP	0x04 /* 2 stop bits */
#تعریف_کرو         LCR_NPAR	0x00 /* no parity */
#تعریف_کرو         LCR_OPAR	0x08 /* odd parity */
#تعریف_کرو         LCR_EPAR	0x18 /* even parity */
#تعریف_کرو         LCR_BREAK	0x40 /* enable break */
#تعریف_کرو         LCR_DLAB	0x80 /* access DLAB registers */
#تعریف_کرو COM1_LSR	(COM1_بنیاد + LSRREG)
#تعریف_کرو         LSR_DR          0x01
#تعریف_کرو         LSR_THRE        0x20
#تعریف_کرو         LCR_DLA         0x80

#تعریف_کرو UART_بنیاد_FREQ	115200U	

#اگر_ختم
