
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
	만약(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const char *str)
{
	동안 (*str) {
		direct_print_char(*str);
		str++;
	}
}

정수 direct_read_char(unsigned char *ch)
{
	반환 0;
}
