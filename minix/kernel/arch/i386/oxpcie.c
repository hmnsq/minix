
#ضمن "kernel/kernel.h"

#إذا CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ضمن "oxpcie.h"
#ضمن "serial.h"

ساكن بیعلامت حرف *oxpcie_vaddr = عدم;

فراغ oxpcie_مجموعه_vaddr(بیعلامت حرف *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ساكن فراغ oxpcie_init(فراغ)
{
	چاپ_کن("oxpcie_init\n");
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

	إذا(vm_running && oxpcie_vaddr && !inuse) {
        	صحيح i;
		ساكن صحيح init_انجامشده;
		inuse = 1;

		إذا(!init_انجامشده) {
			oxpcie_init();
			init_انجامشده = 1;
		}

        	لكل (i= 0; i<100000; i++) {
			إذا(OXPCIE_LSR & LSR_THRE)
                       		اكسر;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

صحيح oxpcie_درون(فراغ)
{
	إذا(vm_running && oxpcie_vaddr) {
		صحيح lsr;
		lsr = OXPCIE_LSR;
		إذا(lsr & LSR_DR)
			أرجع (صحيح) OXPCIE_RBR;
	}

	أرجع -1;
}

#پایان_اگر
