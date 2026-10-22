
#include "kernel/kernel.h"

#නම් CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#include "oxpcie.h"
#include "serial.h"

static unsigned var_na *oxpcie_vaddr = NULL;

void oxpcie_set_vaddr(unsigned var_na *vaddr)
{
	oxpcie_vaddr = vaddr;
}

static void oxpcie_init(void)
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

void oxpcie_putc(var_na c)
{
	static පූර්ණ_සංඛ්‍යා inuse = 0;

	නම්(vm_running && oxpcie_vaddr && !inuse) {
        	පූර්ණ_සංඛ්‍යා i;
		static පූර්ණ_සංඛ්‍යා init_done;
		inuse = 1;

		නම්(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	සඳහා (i= 0; i<100000; i++) {
			නම්(OXPCIE_LSR & LSR_THRE)
                       		bha_nga;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

පූර්ණ_සංඛ්‍යා oxpcie_in(void)
{
	නම්(vm_running && oxpcie_vaddr) {
		පූර්ණ_සංඛ්‍යා lsr;
		lsr = OXPCIE_LSR;
		නම්(lsr & LSR_DR)
			ආපසු (පූර්ණ_සංඛ්‍යා) OXPCIE_RBR;
	}

	ආපසු -1;
}

#endif
