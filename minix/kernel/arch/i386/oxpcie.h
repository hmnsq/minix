
శూన్య oxpcie_సమితి_vaddr(గుర్తులేని అక్షరం *vaddr);
శూన్య oxpcie_putc(అక్షరం c);
పూర్ణాంకం oxpcie_లోపల(శూన్య);

#అన్తర్భావ "serial.h"

/* OXPCIe952 info */
#నిర్వచించు UART1_పునాది_550   0x1000
#నిర్వచించు UART1_పునాది_650   0x1090
#నిర్వచించు UART1_పునాది_950
#నిర్వచించు BASELINEICR     (UART1_పునాది_550 + 0xC0)
#నిర్వచించు         OXPCIE_THR      oxpcie_vaddr[UART1_పునాది_550 + THRREG]
#నిర్వచించు         OXPCIE_RBR      oxpcie_vaddr[UART1_పునాది_550 + RBRREG]
#నిర్వచించు         OXPCIE_LSR      oxpcie_vaddr[UART1_పునాది_550 + LSRREG]
#నిర్వచించు         OXPCIE_LCR      oxpcie_vaddr[UART1_పునాది_550 + LCRREG]
#నిర్వచించు         OXPCIE_DLL      oxpcie_vaddr[UART1_పునాది_550 + 0x00]
#నిర్వచించు         OXPCIE_DLM      oxpcie_vaddr[UART1_పునాది_550 + 0x01]
#నిర్వచించు         OXPCIE_FICR     oxpcie_vaddr[UART1_పునాది_550 + FICRREG]
#నిర్వచించు         OXPCIE_SPR      oxpcie_vaddr[UART1_పునాది_550 + SPRREG]
#నిర్వచించు         OXPCIE_EFR      oxpcie_vaddr[UART1_పునాది_650 + 0x10]
#నిర్వచించు         OXPCIE_ICR      oxpcie_vaddr[UART1_పునాది_950 + 0x05]

#నిర్వచించు         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#నిర్వచించు         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#నిర్వచించు         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#నిర్వచించు         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#నిర్వచించు         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#నిర్వచించు         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

