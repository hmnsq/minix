
#جيڪڏهن_وصف_نه_ٿيل _KERN_SERIAL_H
#وصف_ڪريو _KERN_SERIAL_H 1

#وصف_ڪريو THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#وصف_ڪريو RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#وصف_ڪريو DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#وصف_ڪريو DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#وصف_ڪريو FICRREG 2	/* FIFO control, write-only */
#وصف_ڪريو LCRREG  3	/* line control, read/write */
#وصف_ڪريو LSRREG  5	/* line status, read-only */
#وصف_ڪريو SPRREG  7

#وصف_ڪريو COM1_بنياد	0x3F8
#وصف_ڪريو COM1_THR	(COM1_بنياد + THRREG)
#وصف_ڪريو COM1_RBR	(COM1_بنياد + RBRREG)
#وصف_ڪريو COM1_DLL	(COM1_بنياد + DLLREG)
#وصف_ڪريو COM1_DLM	(COM1_بنياد + DLMREG)
#وصف_ڪريو COM1_LCR	(COM1_بنياد + LCRREG)
#وصف_ڪريو         LCR_5_ٻٽو_انگ	0x00 /* 5 bits per data word */
#وصف_ڪريو         LCR_6_ٻٽو_انگ	0x01 /* 6 bits per data word */
#وصف_ڪريو         LCR_7_ٻٽو_انگ	0x02 /* 7 bits per data word */
#وصف_ڪريو         LCR_8_ٻٽو_انگ	0x03 /* 8 bits per data word */
#وصف_ڪريو         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#وصف_ڪريو         LCR_2STOP	0x04 /* 2 stop bits */
#وصف_ڪريو         LCR_NPAR	0x00 /* no parity */
#وصف_ڪريو         LCR_OPAR	0x08 /* odd parity */
#وصف_ڪريو         LCR_EPAR	0x18 /* even parity */
#وصف_ڪريو         LCR_BREAK	0x40 /* enable break */
#وصف_ڪريو         LCR_DLAB	0x80 /* access DLAB registers */
#وصف_ڪريو COM1_LSR	(COM1_بنياد + LSRREG)
#وصف_ڪريو         LSR_DR          0x01
#وصف_ڪريو         LSR_THRE        0x20
#وصف_ڪريو         LCR_DLA         0x80

#وصف_ڪريو UART_بنياد_FREQ	115200U	

#جيڪڏهن_پورو
