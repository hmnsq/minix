
#ضمن "kernel/kernel.h"

#ܐܢ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ضمن "oxpcie.h"
#ضمن "serial.h"

ساكن unsigned حرف *oxpcie_vaddr = عدم;

فراغ oxpcie_set_vaddr(unsigned حرف *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ساكن فراغ oxpcie_init(فراغ)
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

فراغ oxpcie_putc(حرف c)
{
	ساكن صحيح inuse = 0;

	ܐܢ(vm_running && oxpcie_vaddr && !inuse) {
        	صحيح i;
		ساكن صحيح init_done;
		inuse = 1;

		ܐܢ(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	لكل (i= 0; i<100000; i++) {
			ܐܢ(OXPCIE_LSR & LSR_THRE)
                       		اكسر;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

صحيح oxpcie_in(فراغ)
{
	ܐܢ(vm_running && oxpcie_vaddr) {
		صحيح lsr;
		lsr = OXPCIE_LSR;
		ܐܢ(lsr & LSR_DR)
			ܦܢܐ (صحيح) OXPCIE_RBR;
	}

	ܦܢܐ -1;
}

#endif
