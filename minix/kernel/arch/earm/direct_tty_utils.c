
#include "kernel/kernel.h"
#include "direct_utils.h"
#include "bsp_serial.h"
#include "glo.h"

void direct_cls(void)
{
    /* Do nothing */
}

void direct_print_char(var_na c)
{
	ᮚᮦᮔ᮪(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	ᮞᮨᮓᮨᮜ (*str) {
		direct_print_char(*str);
		str++;
	}
}

puur_naa_nka direct_read_char(unsigned var_na *ch)
{
	ᮝᮜᮤ 0;
}
