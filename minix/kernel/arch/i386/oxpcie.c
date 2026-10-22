
#अन्तर्भाव "kernel/kernel.h"

#यदि CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#अन्तर्भाव "oxpcie.h"
#अन्तर्भाव "serial.h"

स्थिर unsigned वर्ण *oxpcie_vaddr = रिक्त;

शून्य oxpcie_set_vaddr(unsigned वर्ण *vaddr)
{
	oxpcie_vaddr = vaddr;
}

स्थिर शून्य oxpcie_init(शून्य)
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

शून्य oxpcie_putc(वर्ण c)
{
	स्थिर पूर्णाङ्क inuse = 0;

	यदि(vm_running && oxpcie_vaddr && !inuse) {
        	पूर्णाङ्क i;
		स्थिर पूर्णाङ्क init_done;
		inuse = 1;

		यदि(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	प्रति (i= 0; i<100000; i++) {
			यदि(OXPCIE_LSR & LSR_THRE)
                       		भङ्ग;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

पूर्णाङ्क oxpcie_in(शून्य)
{
	यदि(vm_running && oxpcie_vaddr) {
		पूर्णाङ्क lsr;
		lsr = OXPCIE_LSR;
		यदि(lsr & LSR_DR)
			प्रत्यागम (पूर्णाङ्क) OXPCIE_RBR;
	}

	प्रत्यागम -1;
}

#endif
