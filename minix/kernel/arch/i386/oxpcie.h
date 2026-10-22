
शून्य oxpcie_समुच्चयः_vaddr(अचिह्नित वर्ण *vaddr);
शून्य oxpcie_putc(वर्ण c);
पूर्णाङ्क oxpcie_अन्तर्(शून्य);

#अन्तर्भाव "serial.h"

/* OXPCIe952 info */
#परिभाषय UART1_आधारः_550   0x1000
#परिभाषय UART1_आधारः_650   0x1090
#परिभाषय UART1_आधारः_950
#परिभाषय BASELINEICR     (UART1_आधारः_550 + 0xC0)
#परिभाषय         OXPCIE_THR      oxpcie_vaddr[UART1_आधारः_550 + THRREG]
#परिभाषय         OXPCIE_RBR      oxpcie_vaddr[UART1_आधारः_550 + RBRREG]
#परिभाषय         OXPCIE_LSR      oxpcie_vaddr[UART1_आधारः_550 + LSRREG]
#परिभाषय         OXPCIE_LCR      oxpcie_vaddr[UART1_आधारः_550 + LCRREG]
#परिभाषय         OXPCIE_DLL      oxpcie_vaddr[UART1_आधारः_550 + 0x00]
#परिभाषय         OXPCIE_DLM      oxpcie_vaddr[UART1_आधारः_550 + 0x01]
#परिभाषय         OXPCIE_FICR     oxpcie_vaddr[UART1_आधारः_550 + FICRREG]
#परिभाषय         OXPCIE_SPR      oxpcie_vaddr[UART1_आधारः_550 + SPRREG]
#परिभाषय         OXPCIE_EFR      oxpcie_vaddr[UART1_आधारः_650 + 0x10]
#परिभाषय         OXPCIE_ICR      oxpcie_vaddr[UART1_आधारः_950 + 0x05]

#परिभाषय         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#परिभाषय         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#परिभाषय         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#परिभाषय         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#परिभाषय         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#परिभाषय         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

