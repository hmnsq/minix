
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

#ಯದಿ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ಅನ್ತರ್ಭಾವ "oxpcie.h"
#ಅನ್ತರ್ಭಾವ "serial.h"

ಸ್ಥಿರ unsigned ವರ್ಣ *oxpcie_vaddr = ರಿಕ್ತ;

ಶೂನ್ಯ oxpcie_set_vaddr(unsigned ವರ್ಣ *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ಸ್ಥಿರ ಶೂನ್ಯ oxpcie_init(ಶೂನ್ಯ)
{
	printf("oxpcie_init\n");
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

ಶೂನ್ಯ oxpcie_putc(ವರ್ಣ c)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ inuse = 0;

	ಯದಿ(vm_running && oxpcie_vaddr && !inuse) {
        	ಪೂರ್ಣಾಙ್ಕ i;
		ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ init_done;
		inuse = 1;

		ಯದಿ(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	ಪ್ರತಿ (i= 0; i<100000; i++) {
			ಯದಿ(OXPCIE_LSR & LSR_THRE)
                       		ಭಙ್ಗ;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

ಪೂರ್ಣಾಙ್ಕ oxpcie_in(ಶೂನ್ಯ)
{
	ಯದಿ(vm_running && oxpcie_vaddr) {
		ಪೂರ್ಣಾಙ್ಕ lsr;
		lsr = OXPCIE_LSR;
		ಯದಿ(lsr & LSR_DR)
			ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) OXPCIE_RBR;
	}

	ಪ್ರತ್ಯಾಗಮ -1;
}

#endif
