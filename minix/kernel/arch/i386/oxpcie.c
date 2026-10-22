
#अन्तर्भाव "kernel/kernel.h"

#जर CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#अन्तर्भाव "oxpcie.h"
#अन्तर्भाव "serial.h"

स्थिर अचिन्हित अक्षर *oxpcie_vaddr = रिक्त;

शून्य oxpcie_set_vaddr(अचिन्हित अक्षर *vaddr)
{
	oxpcie_vaddr = vaddr;
}

स्थिर शून्य oxpcie_init(शून्य)
{
	म_लिखो("oxpcie_init\n");
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

शून्य oxpcie_putc(अक्षर c)
{
	स्थिर पूर्णांक inuse = 0;

	जर(vm_running && oxpcie_vaddr && !inuse) {
        	पूर्णांक i;
		स्थिर पूर्णांक init_done;
		inuse = 1;

		जर(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	साठी (i= 0; i<100000; i++) {
			जर(OXPCIE_LSR & LSR_THRE)
                       		थांबा;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

पूर्णांक oxpcie_in(शून्य)
{
	जर(vm_running && oxpcie_vaddr) {
		पूर्णांक lsr;
		lsr = OXPCIE_LSR;
		जर(lsr & LSR_DR)
			परत (पूर्णांक) OXPCIE_RBR;
	}

	परत -1;
}

#endif
