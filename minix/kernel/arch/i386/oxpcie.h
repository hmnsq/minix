
فراغ oxpcie_سېټ_vaddr(بینښه حرف *vaddr);
فراغ oxpcie_putc(حرف c);
صحيح oxpcie_دننه(فراغ);

#ضمن "serial.h"

/* OXPCIe952 info */
#تعریف_کړه UART1_بنسټ_550   0x1000
#تعریف_کړه UART1_بنسټ_650   0x1090
#تعریف_کړه UART1_بنسټ_950
#تعریف_کړه BASELINEICR     (UART1_بنسټ_550 + 0xC0)
#تعریف_کړه         OXPCIE_THR      oxpcie_vaddr[UART1_بنسټ_550 + THRREG]
#تعریف_کړه         OXPCIE_RBR      oxpcie_vaddr[UART1_بنسټ_550 + RBRREG]
#تعریف_کړه         OXPCIE_LSR      oxpcie_vaddr[UART1_بنسټ_550 + LSRREG]
#تعریف_کړه         OXPCIE_LCR      oxpcie_vaddr[UART1_بنسټ_550 + LCRREG]
#تعریف_کړه         OXPCIE_DLL      oxpcie_vaddr[UART1_بنسټ_550 + 0x00]
#تعریف_کړه         OXPCIE_DLM      oxpcie_vaddr[UART1_بنسټ_550 + 0x01]
#تعریف_کړه         OXPCIE_FICR     oxpcie_vaddr[UART1_بنسټ_550 + FICRREG]
#تعریف_کړه         OXPCIE_SPR      oxpcie_vaddr[UART1_بنسټ_550 + SPRREG]
#تعریف_کړه         OXPCIE_EFR      oxpcie_vaddr[UART1_بنسټ_650 + 0x10]
#تعریف_کړه         OXPCIE_ICR      oxpcie_vaddr[UART1_بنسټ_950 + 0x05]

#تعریف_کړه         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#تعریف_کړه         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#تعریف_کړه         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#تعریف_کړه         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#تعریف_کړه         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#تعریف_کړه         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

