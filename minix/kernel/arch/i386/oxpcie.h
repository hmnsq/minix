
فراغ oxpcie_سيٽ_vaddr(بنانشان حرف *vaddr);
فراغ oxpcie_putc(حرف c);
صحيح oxpcie_اندريون(فراغ);

#ضمن "serial.h"

/* OXPCIe952 info */
#وصف_ڪريو UART1_بنياد_550   0x1000
#وصف_ڪريو UART1_بنياد_650   0x1090
#وصف_ڪريو UART1_بنياد_950
#وصف_ڪريو BASELINEICR     (UART1_بنياد_550 + 0xC0)
#وصف_ڪريو         OXPCIE_THR      oxpcie_vaddr[UART1_بنياد_550 + THRREG]
#وصف_ڪريو         OXPCIE_RBR      oxpcie_vaddr[UART1_بنياد_550 + RBRREG]
#وصف_ڪريو         OXPCIE_LSR      oxpcie_vaddr[UART1_بنياد_550 + LSRREG]
#وصف_ڪريو         OXPCIE_LCR      oxpcie_vaddr[UART1_بنياد_550 + LCRREG]
#وصف_ڪريو         OXPCIE_DLL      oxpcie_vaddr[UART1_بنياد_550 + 0x00]
#وصف_ڪريو         OXPCIE_DLM      oxpcie_vaddr[UART1_بنياد_550 + 0x01]
#وصف_ڪريو         OXPCIE_FICR     oxpcie_vaddr[UART1_بنياد_550 + FICRREG]
#وصف_ڪريو         OXPCIE_SPR      oxpcie_vaddr[UART1_بنياد_550 + SPRREG]
#وصف_ڪريو         OXPCIE_EFR      oxpcie_vaddr[UART1_بنياد_650 + 0x10]
#وصف_ڪريو         OXPCIE_ICR      oxpcie_vaddr[UART1_بنياد_950 + 0x05]

#وصف_ڪريو         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#وصف_ڪريو         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#وصف_ڪريو         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#وصف_ڪريو         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#وصف_ڪريو         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#وصف_ڪريو         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

