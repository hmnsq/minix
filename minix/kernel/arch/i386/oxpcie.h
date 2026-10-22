
ಶೂನ್ಯ oxpcie_ಗಣ_vaddr(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *vaddr);
ಶೂನ್ಯ oxpcie_putc(ಅಕ್ಷರ c);
ಪೂರ್ಣಾಂಕ oxpcie_ಒಳ(ಶೂನ್ಯ);

#ಅನ್ತರ್ಭಾವ "serial.h"

/* OXPCIe952 info */
#ವ್ಯಾಖ್ಯಾನಿಸು UART1_ತಳ_550   0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು UART1_ತಳ_650   0x1090
#ವ್ಯಾಖ್ಯಾನಿಸು UART1_ತಳ_950
#ವ್ಯಾಖ್ಯಾನಿಸು BASELINEICR     (UART1_ತಳ_550 + 0xC0)
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_THR      oxpcie_vaddr[UART1_ತಳ_550 + THRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_RBR      oxpcie_vaddr[UART1_ತಳ_550 + RBRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_LSR      oxpcie_vaddr[UART1_ತಳ_550 + LSRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_LCR      oxpcie_vaddr[UART1_ತಳ_550 + LCRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_DLL      oxpcie_vaddr[UART1_ತಳ_550 + 0x00]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_DLM      oxpcie_vaddr[UART1_ತಳ_550 + 0x01]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_FICR     oxpcie_vaddr[UART1_ತಳ_550 + FICRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_SPR      oxpcie_vaddr[UART1_ತಳ_550 + SPRREG]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_EFR      oxpcie_vaddr[UART1_ತಳ_650 + 0x10]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_ICR      oxpcie_vaddr[UART1_ತಳ_950 + 0x05]

#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_CPR      oxpcie_vaddr[BASELINEICR + 0x01]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_TCR      oxpcie_vaddr[BASELINEICR + 0x02]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_CPR2     oxpcie_vaddr[BASELINEICR + 0x03]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_CSR      oxpcie_vaddr[BASELINEICR + 0x0C]
#ವ್ಯಾಖ್ಯಾನಿಸು         OXPCIE_PIDX     oxpcie_vaddr[BASELINEICR + 0x12]

#ವ್ಯಾಖ್ಯಾನಿಸು         LCR_CONFIG      0x03 /* bits 6:0 -= 0x03 => 8N1, no break. */

