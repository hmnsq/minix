
#समावेश "kernel/kernel.h"

#अगर CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#समावेश "oxpcie.h"
#समावेश "serial.h"

अटल अचिन्हित अक्षर *oxpcie_vaddr = शून्य;

व्योम oxpcie_set_vaddr(अचिन्हित अक्षर *vaddr)
{
	oxpcie_vaddr = vaddr;
}

अटल व्योम oxpcie_init(व्योम)
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

व्योम oxpcie_putc(अक्षर c)
{
	अटल पूर्णांक inuse = 0;

	अगर(vm_running && oxpcie_vaddr && !inuse) {
        	पूर्णांक i;
		अटल पूर्णांक init_done;
		inuse = 1;

		अगर(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	क्रम (i= 0; i<100000; i++) {
			अगर(OXPCIE_LSR & LSR_THRE)
                       		अवरोध;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

पूर्णांक oxpcie_in(व्योम)
{
	अगर(vm_running && oxpcie_vaddr) {
		पूर्णांक lsr;
		lsr = OXPCIE_LSR;
		अगर(lsr & LSR_DR)
			वापस (पूर्णांक) OXPCIE_RBR;
	}

	वापस -1;
}

#पूर्ण_अगर
