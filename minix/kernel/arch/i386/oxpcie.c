
#അന്തര്ഭാവ "kernel/kernel.h"

#എങ്കിൽ CONFIG_OXPCIE

/* Documentation is at http://www.plxtech.com/products/uart/oxpcie952 */

#അന്തര്ഭാവ "oxpcie.h"
#അന്തര്ഭാവ "serial.h"

സ്ഥിതം unsigned അക്ഷരം *oxpcie_vaddr = ശൂന്യ;

ശൂന്യം oxpcie_set_vaddr(unsigned അക്ഷരം *vaddr)
{
	oxpcie_vaddr = vaddr;
}

സ്ഥിതം ശൂന്യം oxpcie_init(ശൂന്യം)
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

ശൂന്യം oxpcie_putc(അക്ഷരം c)
{
	സ്ഥിതം പൂർണ്ണസംഖ്യ inuse = 0;

	എങ്കിൽ(vm_running && oxpcie_vaddr && !inuse) {
        	പൂർണ്ണസംഖ്യ i;
		സ്ഥിതം പൂർണ്ണസംഖ്യ init_done;
		inuse = 1;

		എങ്കിൽ(!init_done) {
			oxpcie_init();
			init_done = 1;
		}

        	വേണ്ടി (i= 0; i<100000; i++) {
			എങ്കിൽ(OXPCIE_LSR & LSR_THRE)
                       		നിർത്തുക;
		}
		OXPCIE_THR = c;
		inuse = 0;
	}
}

പൂർണ്ണസംഖ്യ oxpcie_in(ശൂന്യം)
{
	എങ്കിൽ(vm_running && oxpcie_vaddr) {
		പൂർണ്ണസംഖ്യ lsr;
		lsr = OXPCIE_LSR;
		എങ്കിൽ(lsr & LSR_DR)
			തിരികെ (പൂർണ്ണസംഖ്യ) OXPCIE_RBR;
	}

	തിരികെ -1;
}

#endif
