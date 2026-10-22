
#अगर_अघोषित _KERN_SERIAL_H
#घोषणा _KERN_SERIAL_H 1

#घोषणा THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#घोषणा RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#घोषणा DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#घोषणा DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#घोषणा FICRREG 2	/* FIFO control, write-only */
#घोषणा LCRREG  3	/* line control, read/write */
#घोषणा LSRREG  5	/* line status, read-only */
#घोषणा SPRREG  7

#घोषणा COM1_BASE	0x3F8
#घोषणा COM1_THR	(COM1_BASE + THRREG)
#घोषणा COM1_RBR	(COM1_BASE + RBRREG)
#घोषणा COM1_DLL	(COM1_BASE + DLLREG)
#घोषणा COM1_DLM	(COM1_BASE + DLMREG)
#घोषणा COM1_LCR	(COM1_BASE + LCRREG)
#घोषणा         LCR_5BIT	0x00 /* 5 bits per data word */
#घोषणा         LCR_6BIT	0x01 /* 6 bits per data word */
#घोषणा         LCR_7BIT	0x02 /* 7 bits per data word */
#घोषणा         LCR_8BIT	0x03 /* 8 bits per data word */
#घोषणा         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#घोषणा         LCR_2STOP	0x04 /* 2 stop bits */
#घोषणा         LCR_NPAR	0x00 /* no parity */
#घोषणा         LCR_OPAR	0x08 /* odd parity */
#घोषणा         LCR_EPAR	0x18 /* even parity */
#घोषणा         LCR_BREAK	0x40 /* enable break */
#घोषणा         LCR_DLAB	0x80 /* access DLAB registers */
#घोषणा COM1_LSR	(COM1_BASE + LSRREG)
#घोषणा         LSR_DR          0x01
#घोषणा         LSR_THRE        0x20
#घोषणा         LCR_DLA         0x80

#घोषणा UART_BASE_FREQ	115200U	

#पूर्ण_अगर
