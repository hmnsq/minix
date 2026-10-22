
#include "kernel/kernel.h"

#якщо CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#include "oxpcie.h"
#include "serial.h"

static unsigned char *oxpcie_vaddr = NULL;

void oxpcie_set_vaddr(unsigned char *vaddr)
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

void oxpcie_putc(char c)
{
	static ціле inuse = 0;

	якщо(vm_running && oxpcie_vaddr && !inuse) {
        	ціле i;
		static ціле init_done;
		inuse = 1;

		якщо(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	для (i= 0; i<100000; i++) {
			якщо(OXPCIE_LSR & LSR_THRE)
                       		break;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

ціле oxpcie_in(void)
{
	якщо(vm_running && oxpcie_vaddr) {
		ціле lsr;
		lsr = OXPCIE_LSR;
		якщо(lsr & LSR_DR)
			повернути (ціле) OXPCIE_RBR;
	}

	повернути -1;
}

#endif
