
#অন্তর্ভাৱ "kernel/kernel.h"

#যদি CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#অন্তর্ভাৱ "oxpcie.h"
#অন্তর্ভাৱ "serial.h"

স্থির unsigned ৱর্ণ *oxpcie_vaddr = রিক্ত;

শূন্য oxpcie_set_vaddr(unsigned ৱর্ণ *vaddr)
{
	oxpcie_vaddr = vaddr;
}

স্থির শূন্য oxpcie_init(শূন্য)
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

শূন্য oxpcie_putc(ৱর্ণ c)
{
	স্থির পূর্ণাঙ্ক inuse = 0;

	যদি(vm_running && oxpcie_vaddr && !inuse) {
        	পূর্ণাঙ্ক i;
		স্থির পূর্ণাঙ্ক init_done;
		inuse = 1;

		যদি(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	বাবে (i= 0; i<100000; i++) {
			যদি(OXPCIE_LSR & LSR_THRE)
                       		ভঙ্গ;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

পূর্ণাঙ্ক oxpcie_in(শূন্য)
{
	যদি(vm_running && oxpcie_vaddr) {
		পূর্ণাঙ্ক lsr;
		lsr = OXPCIE_LSR;
		যদি(lsr & LSR_DR)
			ঘূৰাই (পূর্ণাঙ্ক) OXPCIE_RBR;
	}

	ঘূৰাই -1;
}

#endif
