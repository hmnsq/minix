
ਖਾਲੀ oxpcie_ਸੈੱਟ_vaddr(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *vaddr);
ਖਾਲੀ oxpcie_putc(ਅੱਖਰ c);
ਪੂਰਨ oxpcie_ਵਿੱਚ(ਖਾਲੀ);

#ਅਨ੍ਤਰ੍ਭਾਵ "serial.h"

/* OXPCIe952 info */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART1_ਅਧਾਰ_550   0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART1_ਅਧਾਰ_650   0x1090
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART1_ਅਧਾਰ_950
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BASELINEICR     (UART1_ਅਧਾਰ_550 + 0xC0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_THR      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + THRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_RBR      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + RBRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_LSR      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + LSRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_LCR      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + LCRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_DLL      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + 0x00]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_DLM      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + 0x01]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_FICR     oxpcie_vaddr[UART1_ਅਧਾਰ_550 + FICRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_SPR      oxpcie_vaddr[UART1_ਅਧਾਰ_550 + SPRREG]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_EFR      oxpcie_vaddr[UART1_ਅਧਾਰ_650 + 0x10]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_ICR      oxpcie_vaddr[UART1_ਅਧਾਰ_950 + 0x05]

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

