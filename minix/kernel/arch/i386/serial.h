
#నిర్వచితం_కాకపోతే _KERN_SERIAL_H
#నిర్వచించు _KERN_SERIAL_H 1

#నిర్వచించు THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#నిర్వచించు RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#నిర్వచించు DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#నిర్వచించు DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#నిర్వచించు FICRREG 2	/* FIFO control, write-only */
#నిర్వచించు LCRREG  3	/* line control, read/write */
#నిర్వచించు LSRREG  5	/* line status, read-only */
#నిర్వచించు SPRREG  7

#నిర్వచించు COM1_పునాది	0x3F8
#నిర్వచించు COM1_THR	(COM1_పునాది + THRREG)
#నిర్వచించు COM1_RBR	(COM1_పునాది + RBRREG)
#నిర్వచించు COM1_DLL	(COM1_పునాది + DLLREG)
#నిర్వచించు COM1_DLM	(COM1_పునాది + DLMREG)
#నిర్వచించు COM1_LCR	(COM1_పునాది + LCRREG)
#నిర్వచించు         LCR_5_బిట్టు	0x00 /* 5 bits per data word */
#నిర్వచించు         LCR_6_బిట్టు	0x01 /* 6 bits per data word */
#నిర్వచించు         LCR_7_బిట్టు	0x02 /* 7 bits per data word */
#నిర్వచించు         LCR_8_బిట్టు	0x03 /* 8 bits per data word */
#నిర్వచించు         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#నిర్వచించు         LCR_2STOP	0x04 /* 2 stop bits */
#నిర్వచించు         LCR_NPAR	0x00 /* no parity */
#నిర్వచించు         LCR_OPAR	0x08 /* odd parity */
#నిర్వచించు         LCR_EPAR	0x18 /* even parity */
#నిర్వచించు         LCR_BREAK	0x40 /* enable break */
#నిర్వచించు         LCR_DLAB	0x80 /* access DLAB registers */
#నిర్వచించు COM1_LSR	(COM1_పునాది + LSRREG)
#నిర్వచించు         LSR_DR          0x01
#నిర్వచించు         LSR_THRE        0x20
#నిర్వచించు         LCR_DLA         0x80

#నిర్వచించు UART_పునాది_FREQ	115200U	

#అయితే_ముగింపు
