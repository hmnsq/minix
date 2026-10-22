
व्योम oxpcie_set_vaddr(अचिन्हित अक्षर *vaddr);
व्योम oxpcie_putc(अक्षर c);
पूर्णांक oxpcie_in(व्योम);

#समावेश "serial.h"

/* OXPCIe952 info */
#घोषणा UART1BASE_550   0x1000
#घोषणा UART1BASE_650   0x1090
#घोषणा UART1BASE_950
#घोषणा BASELINEICR     (UART1BASE_550 + 0xC0)
#घोषणा         OXPCIE_THR      oxpcie_vaddr[UART1BASE_550 + THRREG]
#घोषणा         OXPCIE_RBR      oxpcie_vaddr[UART1BASE_550 + RBRREG]
#घोषणा         OXPCIE_LSR      oxpcie_vaddr[UART1BASE_550 + LSRREG]
#घोषणा         OXPCIE_LCR      oxpcie_vaddr[UART1BASE_550 + LCRREG]
#घोषणा         OXPCIE_DLL      oxpcie_vaddr[UART1BASE_550 + 0x00]
#घोषणा         OXPCIE_DLM      oxpcie_vaddr[UART1BASE_550 + 0x01]
#घोषणा         OXPCIE_FICR     oxpcie_vaddr[UART1BASE_550 + FICRREG]
#घोषणा         OXPCIE_SPR      oxpcie_vaddr[UART1BASE_550 + SPRREG]
#घोषणा         OXPCIE_EFR      oxpcie_vaddr[UART1BASE_650 + 0x10]
#घोषणा         OXPCIE_ICR      oxpcie_vaddr[UART1BASE_950 + 0x05]

#घोषणा         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#घोषणा         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#घोषणा         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#घोषणा         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#घोषणा         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#घोषणा         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

