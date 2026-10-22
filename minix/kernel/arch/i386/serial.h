
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _KERN_SERIAL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _KERN_SERIAL_H 1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ THRREG  0	/* transmitter holding, write-only, DLAB must be clear */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBRREG  0	/* receiver buffer, read-only, DLAB must be clear */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLLREG  0	/* divisor latch LSB, read/write, DLAB must be set */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLMREG  1	/* divisor latch MSB, read/write, DLAB must be set */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FICRREG 2	/* FIFO control, write-only */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LCRREG  3	/* line control, read/write */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSRREG  5	/* line status, read-only */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPRREG  7

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_ਅਧਾਰ	0x3F8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_THR	(COM1_ਅਧਾਰ + THRREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_RBR	(COM1_ਅਧਾਰ + RBRREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_DLL	(COM1_ਅਧਾਰ + DLLREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_DLM	(COM1_ਅਧਾਰ + DLMREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_LCR	(COM1_ਅਧਾਰ + LCRREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_5_ਦੋਆਧਾਰੀ_ਅੰਕ	0x00 /* 5 bits per data word */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_6_ਦੋਆਧਾਰੀ_ਅੰਕ	0x01 /* 6 bits per data word */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_7_ਦੋਆਧਾਰੀ_ਅੰਕ	0x02 /* 7 bits per data word */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_8_ਦੋਆਧਾਰੀ_ਅੰਕ	0x03 /* 8 bits per data word */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_1STOP	0x00 /* 1/1.5 stop bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_2STOP	0x04 /* 2 stop bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_NPAR	0x00 /* no parity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_OPAR	0x08 /* odd parity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_EPAR	0x18 /* even parity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_BREAK	0x40 /* enable break */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_DLAB	0x80 /* access DLAB registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM1_LSR	(COM1_ਅਧਾਰ + LSRREG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LSR_DR          0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LSR_THRE        0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_DLA         0x80

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਅਧਾਰ_FREQ	115200U	

#ਜੇ_ਅੰਤ
