
#અન્તર્ભાવ "kernel/kernel.h"

#જો CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#અન્તર્ભાવ "oxpcie.h"
#અન્તર્ભાવ "serial.h"

સ્થિર unsigned અક્ષર *oxpcie_vaddr = ખાલી;

શૂન્ય oxpcie_set_vaddr(unsigned અક્ષર *vaddr)
{
	oxpcie_vaddr = vaddr;
}

સ્થિર શૂન્ય oxpcie_init(શૂન્ય)
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

શૂન્ય oxpcie_putc(અક્ષર c)
{
	સ્થિર પૂર્ણાંક inuse = 0;

	જો(vm_running && oxpcie_vaddr && !inuse) {
        	પૂર્ણાંક i;
		સ્થિર પૂર્ણાંક init_done;
		inuse = 1;

		જો(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	માટે (i= 0; i<100000; i++) {
			જો(OXPCIE_LSR & LSR_THRE)
                       		તોડો;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

પૂર્ણાંક oxpcie_in(શૂન્ય)
{
	જો(vm_running && oxpcie_vaddr) {
		પૂર્ણાંક lsr;
		lsr = OXPCIE_LSR;
		જો(lsr & LSR_DR)
			પાછા (પૂર્ણાંક) OXPCIE_RBR;
	}

	પાછા -1;
}

#endif
