
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
	အကယ်၍(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	နေစဉ် (*str) {
		direct_print_char(*str);
		str++;
	}
}

ကိန်းပြည့် direct_read_char(unsigned var_na *ch)
{
	ပြန်ပေး 0;
}
