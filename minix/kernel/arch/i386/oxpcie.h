
فراغ oxpcie_سیٹ_vaddr(بنانشانی حرف *vaddr);
فراغ oxpcie_putc(حرف c);
صحيح oxpcie_وچ(فراغ);

#ضمن "serial.h"

/* OXPCIe952 info */
#تعریف_کرو UART1_ادھار_550   0x1000
#تعریف_کرو UART1_ادھار_650   0x1090
#تعریف_کرو UART1_ادھار_950
#تعریف_کرو BASELINEICR     (UART1_ادھار_550 + 0xC0)
#تعریف_کرو         OXPCIE_THR      oxpcie_vaddr[UART1_ادھار_550 + THRREG]
#تعریف_کرو         OXPCIE_RBR      oxpcie_vaddr[UART1_ادھار_550 + RBRREG]
#تعریف_کرو         OXPCIE_LSR      oxpcie_vaddr[UART1_ادھار_550 + LSRREG]
#تعریف_کرو         OXPCIE_LCR      oxpcie_vaddr[UART1_ادھار_550 + LCRREG]
#تعریف_کرو         OXPCIE_DLL      oxpcie_vaddr[UART1_ادھار_550 + 0x00]
#تعریف_کرو         OXPCIE_DLM      oxpcie_vaddr[UART1_ادھار_550 + 0x01]
#تعریف_کرو         OXPCIE_FICR     oxpcie_vaddr[UART1_ادھار_550 + FICRREG]
#تعریف_کرو         OXPCIE_SPR      oxpcie_vaddr[UART1_ادھار_550 + SPRREG]
#تعریف_کرو         OXPCIE_EFR      oxpcie_vaddr[UART1_ادھار_650 + 0x10]
#تعریف_کرو         OXPCIE_ICR      oxpcie_vaddr[UART1_ادھار_950 + 0x05]

#تعریف_کرو         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#تعریف_کرو         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#تعریف_کرو         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#تعریف_کرو         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#تعریف_کرو         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#تعریف_کرو         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

