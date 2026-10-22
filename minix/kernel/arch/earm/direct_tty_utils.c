
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
	གལ་ཏེ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	སྐབས (*str) {
		direct_print_char(*str);
		str++;
	}
}

ཧྲིལ direct_read_char(unsigned var_na *ch)
{
	ཕྱིར 0;
}
