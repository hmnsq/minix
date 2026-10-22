
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಆದರೆ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ಅನ್ತರ್ಭಾವ "oxpcie.h"
#ಅನ್ತರ್ಭಾವ "serial.h"

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *oxpcie_vaddr = ಖಾಲಿ;

ಶೂನ್ಯ oxpcie_ಗಣ_vaddr(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ಸ್ಥಿರ ಶೂನ್ಯ oxpcie_init(ಶೂನ್ಯ)
{
	ಬರೆ("oxpcie_init\n");
	/* Enable access to EFR and DLM+DLL */
	OXPCIE_LCR = 0xBF;

	/* Set FICR[1] to increase FIFO */
	OXPCIE_FICR = 0x01;

	/* Set enhanced mode [4]
	 * no RTS/CTS [7:6]
	 * no special char detection [5]
	 * no in-band receive flow control [1:0]
	 * no in-band transmit flow control [3:2]
	 */
	OXPCIE_EFR  = 0x10; 

	/* Set divisor register to 115200 baud. */
	OXPCIE_DLM = 0x00;
	OXPCIE_DLL = 0x22;

	/* Forget DLM and DLL, set LCR to config. */
	OXPCIE_LCR = LCR_CONFIG;
	OXPCIE_LCR = LCR_CONFIG;

	OXPCIE_TCR = 0x01;
	OXPCIE_CPR = 0x20;
	OXPCIE_CPR2 = 0;
}

ಶೂನ್ಯ oxpcie_putc(ಅಕ್ಷರ c)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ inuse = 0;

	ಆದರೆ(vm_running && oxpcie_vaddr && !inuse) {
        	ಪೂರ್ಣಾಂಕ i;
		ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ init_ಮುಗಿದ;
		inuse = 1;

		ಆದರೆ(!init_ಮುಗಿದ) {
			oxpcie_init();
			init_ಮುಗಿದ = 1;
		}

        	ಗಾಗಿ (i= 0; i<100000; i++) {
			ಆದರೆ(OXPCIE_LSR & LSR_THRE)
                       		ನಿಲ್ಲಿಸು;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

ಪೂರ್ಣಾಂಕ oxpcie_ಒಳ(ಶೂನ್ಯ)
{
	ಆದರೆ(vm_running && oxpcie_vaddr) {
		ಪೂರ್ಣಾಂಕ lsr;
		lsr = OXPCIE_LSR;
		ಆದರೆ(lsr & LSR_DR)
			ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) OXPCIE_RBR;
	}

	ಹಿಂತಿರುಗಿ -1;
}

#ಆದರೆ_ಅಂತ್ಯ
