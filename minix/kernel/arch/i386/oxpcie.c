
#அந்தர்பாவ "kernel/kernel.h"

#எனில் CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#அந்தர்பாவ "oxpcie.h"
#அந்தர்பாவ "serial.h"

நிலையான unsigned எழுத்து *oxpcie_vaddr = வெற்று;

வெறுமை oxpcie_set_vaddr(unsigned எழுத்து *vaddr)
{
	oxpcie_vaddr = vaddr;
}

நிலையான வெறுமை oxpcie_init(வெறுமை)
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

வெறுமை oxpcie_putc(எழுத்து c)
{
	நிலையான முழு inuse = 0;

	எனில்(vm_running && oxpcie_vaddr && !inuse) {
        	முழு i;
		நிலையான முழு init_done;
		inuse = 1;

		எனில்(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	க்கு (i= 0; i<100000; i++) {
			எனில்(OXPCIE_LSR & LSR_THRE)
                       		நிறுத்து;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

முழு oxpcie_in(வெறுமை)
{
	எனில்(vm_running && oxpcie_vaddr) {
		முழு lsr;
		lsr = OXPCIE_LSR;
		எனில்(lsr & LSR_DR)
			திரும்பு (முழு) OXPCIE_RBR;
	}

	திரும்பு -1;
}

#endif
