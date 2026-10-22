
#که_نه_تعریف_شوی _KERN_SERIAL_H
#تعریف_کړه _KERN_SERIAL_H 1

#تعریف_کړه THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#تعریف_کړه RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#تعریف_کړه DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#تعریف_کړه DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#تعریف_کړه FICRREG 2	/* FIFO control, write-only */
#تعریف_کړه LCRREG  3	/* line control, read/write */
#تعریف_کړه LSRREG  5	/* line status, read-only */
#تعریف_کړه SPRREG  7

#تعریف_کړه COM1_بنسټ	0x3F8
#تعریف_کړه COM1_THR	(COM1_بنسټ + THRREG)
#تعریف_کړه COM1_RBR	(COM1_بنسټ + RBRREG)
#تعریف_کړه COM1_DLL	(COM1_بنسټ + DLLREG)
#تعریف_کړه COM1_DLM	(COM1_بنسټ + DLMREG)
#تعریف_کړه COM1_LCR	(COM1_بنسټ + LCRREG)
#تعریف_کړه         LCR_5_بټ	0x00 /* 5 bits per data word */
#تعریف_کړه         LCR_6_بټ	0x01 /* 6 bits per data word */
#تعریف_کړه         LCR_7_بټ	0x02 /* 7 bits per data word */
#تعریف_کړه         LCR_8_بټ	0x03 /* 8 bits per data word */
#تعریف_کړه         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#تعریف_کړه         LCR_2STOP	0x04 /* 2 stop bits */
#تعریف_کړه         LCR_NPAR	0x00 /* no parity */
#تعریف_کړه         LCR_OPAR	0x08 /* odd parity */
#تعریف_کړه         LCR_EPAR	0x18 /* even parity */
#تعریف_کړه         LCR_BREAK	0x40 /* enable break */
#تعریف_کړه         LCR_DLAB	0x80 /* access DLAB registers */
#تعریف_کړه COM1_LSR	(COM1_بنسټ + LSRREG)
#تعریف_کړه         LSR_DR          0x01
#تعریف_کړه         LSR_THRE        0x20
#تعریف_کړه         LCR_DLA         0x80

#تعریف_کړه UART_بنسټ_FREQ	115200U	

#د_که_پای
