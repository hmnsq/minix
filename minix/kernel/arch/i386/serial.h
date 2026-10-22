
#यदि_अपरिभाषितम् _KERN_SERIAL_H
#परिभाषय _KERN_SERIAL_H 1

#परिभाषय THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#परिभाषय RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#परिभाषय DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#परिभाषय DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#परिभाषय FICRREG 2	/* FIFO control, write-only */
#परिभाषय LCRREG  3	/* line control, read/write */
#परिभाषय LSRREG  5	/* line status, read-only */
#परिभाषय SPRREG  7

#परिभाषय COM1_आधारः	0x3F8
#परिभाषय COM1_THR	(COM1_आधारः + THRREG)
#परिभाषय COM1_RBR	(COM1_आधारः + RBRREG)
#परिभाषय COM1_DLL	(COM1_आधारः + DLLREG)
#परिभाषय COM1_DLM	(COM1_आधारः + DLMREG)
#परिभाषय COM1_LCR	(COM1_आधारः + LCRREG)
#परिभाषय         LCR_5BIT	0x00 /* 5 bits per data word */
#परिभाषय         LCR_6BIT	0x01 /* 6 bits per data word */
#परिभाषय         LCR_7BIT	0x02 /* 7 bits per data word */
#परिभाषय         LCR_8BIT	0x03 /* 8 bits per data word */
#परिभाषय         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#परिभाषय         LCR_2STOP	0x04 /* 2 stop bits */
#परिभाषय         LCR_NPAR	0x00 /* no parity */
#परिभाषय         LCR_OPAR	0x08 /* odd parity */
#परिभाषय         LCR_EPAR	0x18 /* even parity */
#परिभाषय         LCR_BREAK	0x40 /* enable break */
#परिभाषय         LCR_DLAB	0x80 /* access DLAB registers */
#परिभाषय COM1_LSR	(COM1_आधारः + LSRREG)
#परिभाषय         LSR_DR          0x01
#परिभाषय         LSR_THRE        0x20
#परिभाषय         LCR_DLA         0x80

#परिभाषय UART_आधारः_FREQ	115200U	

#यदि_अन्तः
