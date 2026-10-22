
#شامل "kernel/kernel.h"

#اگر CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#شامل "oxpcie.h"
#شامل "serial.h"

ساکن غیرعلامتی حرف *oxpcie_vaddr = تہی;

خالی oxpcie_سیٹ_vaddr(غیرعلامتی حرف *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ساکن خالی oxpcie_init(خالی)
{
	لکھو("oxpcie_init\n");
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

خالی oxpcie_putc(حرف c)
{
	ساکن صحیح inuse = 0;

	اگر(vm_running && oxpcie_vaddr && !inuse) {
        	صحیح i;
		ساکن صحیح init_مکمل;
		inuse = 1;

		اگر(!init_مکمل) {
			oxpcie_init();
			init_مکمل = 1;
		}

        	برائے (i= 0; i<100000; i++) {
			اگر(OXPCIE_LSR & LSR_THRE)
                       		توڑو;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

صحیح oxpcie_اندر(خالی)
{
	اگر(vm_running && oxpcie_vaddr) {
		صحیح lsr;
		lsr = OXPCIE_LSR;
		اگر(lsr & LSR_DR)
			واپس (صحیح) OXPCIE_RBR;
	}

	واپس -1;
}

#اگر_ختم
