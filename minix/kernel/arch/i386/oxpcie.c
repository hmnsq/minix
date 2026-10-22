
#అన్తర్భావ "kernel/kernel.h"

#ఒకవేళ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#అన్తర్భావ "oxpcie.h"
#అన్తర్భావ "serial.h"

స్థిర గుర్తులేని అక్షరం *oxpcie_vaddr = ఖాళీ;

శూన్య oxpcie_సమితి_vaddr(గుర్తులేని అక్షరం *vaddr)
{
	oxpcie_vaddr = vaddr;
}

స్థిర శూన్య oxpcie_init(శూన్య)
{
	రాయి("oxpcie_init\n");
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

శూన్య oxpcie_putc(అక్షరం c)
{
	స్థిర పూర్ణాంకం inuse = 0;

	ఒకవేళ(vm_running && oxpcie_vaddr && !inuse) {
        	పూర్ణాంకం i;
		స్థిర పూర్ణాంకం init_పూర్తి;
		inuse = 1;

		ఒకవేళ(!init_పూర్తి) {
			oxpcie_init();
			init_పూర్తి = 1;
		}

        	కొరకు (i= 0; i<100000; i++) {
			ఒకవేళ(OXPCIE_LSR & LSR_THRE)
                       		ఆపు;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

పూర్ణాంకం oxpcie_లోపల(శూన్య)
{
	ఒకవేళ(vm_running && oxpcie_vaddr) {
		పూర్ణాంకం lsr;
		lsr = OXPCIE_LSR;
		ఒకవేళ(lsr & LSR_DR)
			తిరిగి (పూర్ణాంకం) OXPCIE_RBR;
	}

	తిరిగి -1;
}

#అయితే_ముగింపు
