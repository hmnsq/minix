
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
	ប្រសិនបើ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	ខណៈពេល (*str) {
		direct_print_char(*str);
		str++;
	}
}

ចំនួនគត់ direct_read_char(unsigned var_na *ch)
{
	ត្រឡប់ 0;
}
