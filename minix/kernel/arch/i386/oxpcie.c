
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"

#ଯଦି CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ଅନ୍ତର୍ଭାଵ "oxpcie.h"
#ଅନ୍ତର୍ଭାଵ "serial.h"

ସ୍ଥିର unsigned ଅକ୍ଷର *oxpcie_vaddr = ଖାଲି;

ଶୂନ୍ୟ oxpcie_set_vaddr(unsigned ଅକ୍ଷର *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ସ୍ଥିର ଶୂନ୍ୟ oxpcie_init(ଶୂନ୍ୟ)
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

ଶୂନ୍ୟ oxpcie_putc(ଅକ୍ଷର c)
{
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inuse = 0;

	ଯଦି(vm_running && oxpcie_vaddr && !inuse) {
        	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
		ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_done;
		inuse = 1;

		ଯଦି(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	ପାଇଁ (i= 0; i<100000; i++) {
			ଯଦି(OXPCIE_LSR & LSR_THRE)
                       		ଭାଙ୍ଗ;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା oxpcie_in(ଶୂନ୍ୟ)
{
	ଯଦି(vm_running && oxpcie_vaddr) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsr;
		lsr = OXPCIE_LSR;
		ଯଦି(lsr & LSR_DR)
			ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) OXPCIE_RBR;
	}

	ଫେରନ୍ତୁ -1;
}

#endif
