
#اگر_تعریف_نشده _KERN_SERIAL_H
#تعریف_کن _KERN_SERIAL_H 1

#تعریف_کن THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#تعریف_کن RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#تعریف_کن DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#تعریف_کن DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#تعریف_کن FICRREG 2	/* FIFO control, write-only */
#تعریف_کن LCRREG  3	/* line control, read/write */
#تعریف_کن LSRREG  5	/* line status, read-only */
#تعریف_کن SPRREG  7

#تعریف_کن COM1_پایه	0x3F8
#تعریف_کن COM1_THR	(COM1_پایه + THRREG)
#تعریف_کن COM1_RBR	(COM1_پایه + RBRREG)
#تعریف_کن COM1_DLL	(COM1_پایه + DLLREG)
#تعریف_کن COM1_DLM	(COM1_پایه + DLMREG)
#تعریف_کن COM1_LCR	(COM1_پایه + LCRREG)
#تعریف_کن         LCR_5_بیت	0x00 /* 5 bits per data word */
#تعریف_کن         LCR_6_بیت	0x01 /* 6 bits per data word */
#تعریف_کن         LCR_7_بیت	0x02 /* 7 bits per data word */
#تعریف_کن         LCR_8_بیت	0x03 /* 8 bits per data word */
#تعریف_کن         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#تعریف_کن         LCR_2STOP	0x04 /* 2 stop bits */
#تعریف_کن         LCR_NPAR	0x00 /* no parity */
#تعریف_کن         LCR_OPAR	0x08 /* odd parity */
#تعریف_کن         LCR_EPAR	0x18 /* even parity */
#تعریف_کن         LCR_BREAK	0x40 /* enable break */
#تعریف_کن         LCR_DLAB	0x80 /* access DLAB registers */
#تعریف_کن COM1_LSR	(COM1_پایه + LSRREG)
#تعریف_کن         LSR_DR          0x01
#تعریف_کن         LSR_THRE        0x20
#تعریف_کن         LCR_DLA         0x80

#تعریف_کن UART_پایه_FREQ	115200U	

#پایان_اگر
