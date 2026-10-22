
#include "kernel/kernel.h"
#include "direct_utils.h"
#include "bsp_serial.h"
#include "glo.h"

void direct_cls(void)
{
    /* Do nothing */
}

void direct_print_char(char c)
{
	якщо(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const char *str)
{
	поки (*str) {
		direct_print_char(*str);
		str++;
	}
}

ціле direct_read_char(unsigned char *ch)
{
	повернути 0;
}
