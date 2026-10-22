
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
	ถ้า(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	ขณะ (*str) {
		direct_print_char(*str);
		str++;
	}
}

จำนวนเต็ม direct_read_char(unsigned var_na *ch)
{
	คืน 0;
}
