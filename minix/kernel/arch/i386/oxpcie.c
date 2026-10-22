
#অন্তর্ভাব "kernel/kernel.h"

#যদি CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#অন্তর্ভাব "oxpcie.h"
#অন্তর্ভাব "serial.h"

স্থির unsigned অক্ষর *oxpcie_vaddr = ফাঁকা;

শূন্য oxpcie_set_vaddr(unsigned অক্ষর *vaddr)
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

শূন্য oxpcie_putc(অক্ষর c)
{
	স্থির পূর্ণসংখ্যা inuse = 0;

	যদি(vm_running && oxpcie_vaddr && !inuse) {
        	পূর্ণসংখ্যা i;
		স্থির পূর্ণসংখ্যা init_done;
		inuse = 1;

		যদি(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	জন্য (i= 0; i<100000; i++) {
			যদি(OXPCIE_LSR & LSR_THRE)
                       		বিরতি;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

পূর্ণসংখ্যা oxpcie_in(শূন্য)
{
	যদি(vm_running && oxpcie_vaddr) {
		পূর্ণসংখ্যা lsr;
		lsr = OXPCIE_LSR;
		যদি(lsr & LSR_DR)
			ফেরত (পূর্ণসংখ্যা) OXPCIE_RBR;
	}

	ফেরত -1;
}

#endif
