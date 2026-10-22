
فراغ oxpcie_مجموعه_vaddr(بیعلامت حرف *vaddr);
فراغ oxpcie_putc(حرف c);
صحيح oxpcie_درون(فراغ);

#ضمن "serial.h"

/* OXPCIe952 info */
#تعریف_کن UART1_پایه_550   0x1000
#تعریف_کن UART1_پایه_650   0x1090
#تعریف_کن UART1_پایه_950
#تعریف_کن BASELINEICR     (UART1_پایه_550 + 0xC0)
#تعریف_کن         OXPCIE_THR      oxpcie_vaddr[UART1_پایه_550 + THRREG]
#تعریف_کن         OXPCIE_RBR      oxpcie_vaddr[UART1_پایه_550 + RBRREG]
#تعریف_کن         OXPCIE_LSR      oxpcie_vaddr[UART1_پایه_550 + LSRREG]
#تعریف_کن         OXPCIE_LCR      oxpcie_vaddr[UART1_پایه_550 + LCRREG]
#تعریف_کن         OXPCIE_DLL      oxpcie_vaddr[UART1_پایه_550 + 0x00]
#تعریف_کن         OXPCIE_DLM      oxpcie_vaddr[UART1_پایه_550 + 0x01]
#تعریف_کن         OXPCIE_FICR     oxpcie_vaddr[UART1_پایه_550 + FICRREG]
#تعریف_کن         OXPCIE_SPR      oxpcie_vaddr[UART1_پایه_550 + SPRREG]
#تعریف_کن         OXPCIE_EFR      oxpcie_vaddr[UART1_پایه_650 + 0x10]
#تعریف_کن         OXPCIE_ICR      oxpcie_vaddr[UART1_پایه_950 + 0x05]

#تعریف_کن         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#تعریف_کن         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#تعریف_کن         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#تعریف_کن         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#تعریف_کن         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#تعریف_کن         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

