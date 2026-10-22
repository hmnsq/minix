
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

#ਜੇ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#ਅਨ੍ਤਰ੍ਭਾਵ "oxpcie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "serial.h"

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *oxpcie_vaddr = ਨਿਰਾ;

ਖਾਲੀ oxpcie_ਸੈੱਟ_vaddr(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *vaddr)
{
	oxpcie_vaddr = vaddr;
}

ਸਥਿਰ ਖਾਲੀ oxpcie_init(ਖਾਲੀ)
{
	ਲਿਖੋ("oxpcie_init\n");
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

ਖਾਲੀ oxpcie_putc(ਅੱਖਰ c)
{
	ਸਥਿਰ ਪੂਰਨ inuse = 0;

	ਜੇ(vm_running && oxpcie_vaddr && !inuse) {
        	ਪੂਰਨ i;
		ਸਥਿਰ ਪੂਰਨ init_ਪੂਰਾ;
		inuse = 1;

		ਜੇ(!init_ਪੂਰਾ) {
			oxpcie_init();
			init_ਪੂਰਾ = 1;
		}

        	ਲਈ (i= 0; i<100000; i++) {
			ਜੇ(OXPCIE_LSR & LSR_THRE)
                       		ਰੋਕੋ;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

ਪੂਰਨ oxpcie_ਵਿੱਚ(ਖਾਲੀ)
{
	ਜੇ(vm_running && oxpcie_vaddr) {
		ਪੂਰਨ lsr;
		lsr = OXPCIE_LSR;
		ਜੇ(lsr & LSR_DR)
			ਵਾਪਸ (ਪੂਰਨ) OXPCIE_RBR;
	}

	ਵਾਪਸ -1;
}

#ਜੇ_ਅੰਤ
